#include <winsock2.h>
#include "sf_platform.h"
#include "sf_provider.h"
#include <cimple/Buffer.h>
#include <cimple/Strings.h>

#include <wbemprov.h>
#include <tchar.h>
#include <cimple/log.h>
#include <cimple/wmi/BString.h>
#include <cimple/wmi/utils.h>

#include <ws2ipdef.h>
#include <iphlpapi.h>

#include <SF_EthernetPort.h>

#include <sf_wmi.h>

/// Memory allocation for WinAPI calls
#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x)) 
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

/// This value was taken from
/// v5-incoming:src/driver/win/bus/driver/bus_ioctl.h
/// It is used to obtain BootROM version via
/// EFX_Port_Firmware_Version_Read method.
#define REFLASH_TARGET_BOOTROM 2

/// VPD tags
#define VPD_TAG_ID   0x02
#define VPD_TAG_END  0x0f
#define VPD_TAG_R    0x10
#define VPD_TAG_W    0x11

/// Get VPD keyword value from symbols
#define VPD_KEYWORD(a, b)         ((a) | ((b) << 8))

/// Can be returned by methods of EFX_* classes as an indication of success
/// with need of calling changes applying method aftewards.
#define BUS_WMI_COMPLETION_CODE_APPLY_REQUIRED  0x20000000

#define WMI_QUERY_MAX 1000

#define MAC_ADDR_BYTES 6

#define EFX_MAX_MTU 9216

/// Job states in EFX_DiagnosticJob
enum {
  JobCreated = 0,
  JobPending,
  JobRunning,
  JobWaiting,
  JobAbortingFromRun,
  JobComplete,
  JobAbortingFromOther
};

namespace solarflare 
{
    using cimple::BString;
    using cimple::bstr2cstr;
    using cimple::wstr2str;
    using cimple::uint8;
    using cimple::uint16;
    using cimple::Mutex;
    using cimple::Auto_Mutex;
    using cimple::Ref;
    using cimple::SF_EthernetPort;
    using cimple::cast;
    using cimple::Enum_Instances_Handler;
    using cimple::Enum_Instances_Status;

    ///
    /// Information about network interface
    ///
    class InterfaceInfo
    {
    public:
        String Name;          ///< Interface name
        String Descr;         ///< Interface description
        DWORD Index;          ///< Interface index
        DWORD MTU;            ///< Interface MTU
        DWORD AdminStatus;    ///< Interface administrative status
        Array<int> PhysAddr;  ///< Interface physical address

        // Dummy operator to make possible using of cimple::Array
        inline bool operator== (const InterfaceInfo &rhs)
        {
            UNUSED(rhs);
            return false;
        }
    };

    ///
    /// Description of Ethernet port.
    ///
    class PortDescr
    {
    public:
        int pci_fn;             ///< PCI function
        int pci_bus;            ///< PCI bus ID
        int pci_device;         ///< PCI device ID
        int pci_bridge_bus;     ///< PCI brige bus ID
        int pci_bridge_device;  ///< PCI bridge device ID
        int pci_bridge_fn;      ///< PCI bridge function ID
        String deviceInstanceName; ///< WDM device instance name

        int port_id;            ///< Id property value of corresponding
                                ///  EFX_Port instance

        Array<int> permanentMAC;  ///< Permanent MAC address
        Array<int> currentMAC;    ///< Permanent MAC address

        bool linkUp() const;      ///< Link status
        uint64 linkSpeed() const; ///< Link speed
        int linkDuplex() const;   ///< Link duplex mode
        bool autoneg() const;     ///< True if autonegotiation is
                                  ///  available

        InterfaceInfo ifInfo;   ///< Network interface-related information

        String mcfwVersion() const;     ///< MC firmware version
        String bootROMVersion() const;  ///< BootROM version

        String productName() const;   ///< Get product name from VPD
        String productNumber() const; ///< Get product number from VPD
        String serialNumber() const;  ///< Get serial number from VPD

        IWbemClassObject *efxPort;  ///< Associated EFX_Port object

        PortDescr() : efxPort(NULL) {};

        inline void clear()
        {
            if (efxPort != NULL)
                efxPort->Release();

            efxPort = NULL;
        }
        // Dummy operator to make possible using of cimple::Array
        inline bool operator== (const PortDescr &rhs)
        {
            UNUSED(rhs);
            return false;
        }

        inline int pci_compare(const PortDescr &x)
        {
            if (pci_bridge_bus > x.pci_bridge_bus)
                return 1;
            else if (pci_bridge_bus < x.pci_bridge_bus)
                return -1;
            else if (pci_bridge_device > x.pci_bridge_device)
                return 1;
            else if (pci_bridge_device < x.pci_bridge_device)
                return -1;
            else if (pci_bridge_fn > x.pci_bridge_fn)
                return 1;
            else if (pci_bridge_fn < x.pci_bridge_fn)
                return -1;
            else if (pci_bus > x.pci_bus)
                return 1;
            else if (pci_bus < x.pci_bus)
                return -1;
            else if (pci_device > x.pci_device)
                return 1;
            else if (pci_device < x.pci_device)
                return -1;
            else if (pci_fn > x.pci_fn)
                return 1;
            else if (pci_fn < x.pci_fn)
                return -1;
            else
                return 0;
        }
    };

    ///
    /// Description of NIC.
    ///
    class NICDescr
    {
    public:
        int pci_bus;                    ///< PCI bus ID
        int pci_device;                 ///< PCI device ID
        int pci_bridge_bus;             ///< PCI brige bus ID
        int pci_bridge_device;          ///< PCI bridge device ID
        int pci_bridge_fn;              ///< PCI bridge function ID

        Array<PortDescr> ports;         ///< NIC ports

        // Dummy operator to make possible using of cimple::Array
        inline bool operator== (const NICDescr &rhs)
        {
            UNUSED(rhs);
            return false;
        }

        inline void clear()
        {
            unsigned int i;

            for (i = 0; i < ports.size(); i++)
                ports[i].clear();
        }
    };

    ///
    /// Vector of NIC descriptions.
    ///
    typedef Array<NICDescr> NICDescrs;

    ///
    /// Get BootROM version for Ethernet port.
    ///
    /// @param port        EFX_Port object pointer
    /// @param vers  [out] BootROM version
    ///
    /// @return 0 on success or -1 on failure
    ///
    /// @note Do not call this function on dummy EFX_Port instance,
    ///       it will result in crash.
    ///
    static int getPortBootROMVersion(IWbemClassObject *port,
                                     String &vers)
    {
        HRESULT hr;
        int     rc;
        String  objPath;
        int     CompletionCode;
        String  methodName("EFX_Port_Firmware_Version_Read");
        VARIANT targetType;
        
        IWbemClassObject *pIn = NULL;
        IWbemClassObject *pOut = NULL;

        if (wmiEstablishConn() != 0)
            return -1;

        if (port == NULL)
            return -1;

        rc = wmiPrepareMethodCall(port, methodName, objPath, &pIn);
        if (rc < 0)
            return rc;

        targetType.vt = VT_I4;
        targetType.lVal = REFLASH_TARGET_BOOTROM;
        hr = pIn->Put(BString("TargetType").rep(), 0,
                      &targetType, 0);
        VariantClear(&targetType);
        if (FAILED(hr))
        {
            CIMPLE_ERR(("%s():   failed to set TargetType for firmrmware "
                        "version reading method, rc=%lx",
                        __FUNCTION__, hr));
            pIn->Release();
            return -1;
        }

        rc = wmiExecMethod(objPath, methodName, pIn, &pOut);
        if (rc < 0)
        {
            pIn->Release();
            return rc;
        }

        rc = wmiGetStringProp(pOut, "VersionString", vers);
        if (rc < 0)
        {
            pIn->Release();
            pOut->Release();
            return rc;
        }

        pIn->Release();
        pOut->Release();
        return 0;
    }

    ///
    /// Get VPD field value for Solarflare Ethernet port.
    ///
    /// @param port                 EFX_Port object pointer
    /// @param tag                  VPD tag
    /// @param keyword              VPD keyword
    /// @param value          [out] Where to save field value
    ///
    /// @return 0 on success or -1 on failure
    ///
    static int getPortVPDField(IWbemClassObject *port,
                               uint8 tag,
                               uint16 keyword,
                               String &value)
    {
        HRESULT hr;
        int     rc;
        String  objPath;
        int     CompletionCode;
        String  methodName("EFX_Port_ReadVpdKeyword");
        VARIANT propTag;
        VARIANT propKeyword;
        
        IWbemClassObject *pIn = NULL;
        IWbemClassObject *pOut = NULL;

        CIMPLE_DBG(("%s: tag=%u, keyword=%u", __FUNCTION__, tag, keyword));

        if (wmiEstablishConn() != 0)
            return -1;

        if (port == NULL)
            return -1;

        rc = wmiPrepareMethodCall(port, methodName, objPath, &pIn);
        if (rc < 0)
            return rc;

        propTag.vt = VT_UI1;
        propTag.bVal = tag;
        hr = pIn->Put(BString("Tag").rep(), 0,
                      &propTag, 0);
        VariantClear(&propTag);
        if (FAILED(hr))
        {
            CIMPLE_ERR(("%s():   failed to set Tag for VPD "
                        "field reading method, rc=%lx", __FUNCTION__, hr));
            pIn->Release();
            return -1;
        }

        propKeyword.vt = VT_I4;
        propKeyword.lVal = keyword;
        hr = pIn->Put(BString("Keyword").rep(), 0,
                      &propKeyword, 0);
        VariantClear(&propKeyword);
        if (FAILED(hr))
        {
            CIMPLE_ERR(("%s():   failed to set Keyword for VPD "
                        "field reading method, rc=%lx", __FUNCTION__, hr));
            pIn->Release();
            return -1;
        }

        rc = wmiExecMethod(objPath, methodName, pIn, &pOut);
        if (rc < 0)
        {
            pIn->Release();
            return rc;
        }

        rc = wmiGetStringProp(pOut, "Value", value);
        if (rc < 0)
        {
            pIn->Release();
            pOut->Release();
            return rc;
        }

        pIn->Release();
        pOut->Release();
        return 0;
    }

    ///
    /// Get VPD fields for Solarflare Ethernet port.
    ///
    /// @param port                 EFX_Port object pointer
    /// @param productName    [out] Where to save product name
    /// @param productNumber  [out] Where to save product number
    /// @param serialNumber   [out] Where to save serial number
    ///
    /// @return 0 on success or -1 on failure
    ///
    static int getPortVPDFields(IWbemClassObject *port,
                                String &productName,
                                String &productNumber,
                                String &serialNumber)
    {
        if (port == NULL)
            return -1;

        if (getPortVPDField(port, VPD_TAG_ID, 0,
                            productName) != 0 ||
            getPortVPDField(port, VPD_TAG_R, VPD_KEYWORD('P', 'N'),
                            productNumber) != 0 ||
            getPortVPDField(port, VPD_TAG_R, VPD_KEYWORD('S', 'N'),
                            serialNumber) != 0)
            return -1;

        return 0;
    }

    /// Get link status
    bool PortDescr::linkUp() const
    {
        bool result = false;
    
        wmiGetBoolProp(efxPort, "LinkUp",
                       &result);
        return result;
    }

    /// Get link speed
    uint64 PortDescr::linkSpeed() const
    {
        uint64 speed = 0;
    
        wmiGetIntProp<uint64>(efxPort,
                              "LinkCurrentSpeed",
                              &speed);
        return speed;
    }

    /// Get link duplex mode
    int PortDescr::linkDuplex() const
    {
        int duplex = 0;
    
        wmiGetIntProp<int>(efxPort, "LinkDuplex",
                           &duplex);
        return duplex;
    }

    /// Check whether autonegotiation is available
    bool PortDescr::autoneg() const
    {
        bool result = false;
    
        wmiGetBoolProp(efxPort,
                       "FlowControlAutonegotiation",
                       &result);
        return result;
    }

    /// Get MC firmware version
    String PortDescr::mcfwVersion() const
    {
        String vers;
    
        wmiGetStringProp(efxPort, "McfwVersion",
                         vers);
        return vers;
    }

    /// Get BootROM version
    String PortDescr::bootROMVersion() const
    {
        String vers;
    
        getPortBootROMVersion(efxPort, vers);
        return vers;
    }

    /// Get product name from VPD for SF Ethernet port
    String PortDescr::productName() const
    {
        String pname; 

        getPortVPDField(efxPort, VPD_TAG_ID, 0,
                        pname);
        return pname;
    }

    /// Get product number from VPD for SF Ethernet port
    String PortDescr::productNumber() const
    {
        String pnum; 

        getPortVPDField(efxPort, VPD_TAG_R, VPD_KEYWORD('P', 'N'),
                        pnum);
        return pnum;
    }

    /// Get serial number from VPD for SF Ethernet port
    String PortDescr::serialNumber() const
    {
        String snum; 

        getPortVPDField(efxPort, VPD_TAG_R, VPD_KEYWORD('S', 'N'),
                        snum);
        return snum;
    }

    ///
    /// Get information about available network interfaces.
    ///
    /// @param info          [out] Where to save interfaces information
    ///
    /// @return 0 on success or -1 on failure
    ///
    static int getInterfacesInfo(Array<InterfaceInfo> &info)
    {
        PIP_INTERFACE_INFO pInfo = NULL;
        ULONG              outBufLen = 0;
        DWORD              dwRetVal;
        ULONG              ulRetVal;
        int                i;
        unsigned int       j;

        dwRetVal = GetInterfaceInfo(NULL, &outBufLen);
        if (dwRetVal == ERROR_INSUFFICIENT_BUFFER)
        {
            pInfo = reinterpret_cast<IP_INTERFACE_INFO *>
                                            (MALLOC(outBufLen));
            if (pInfo == NULL)
            {
                CIMPLE_ERR(("Failed to allocate memory "
                            "for GetInterfaceInfo()"));
                return -1;
            }
        }

        dwRetVal = GetInterfaceInfo(pInfo, &outBufLen);
        if (dwRetVal != NO_ERROR)
        {
            CIMPLE_ERR(("The second call of GetInterfaceInfo() failed, "
                        "dwRetVal=%d", dwRetVal));
            FREE(pInfo);
            return -1;
        }

        for (i = 0; i < pInfo->NumAdapters; i++)
        {
            MIB_IFROW     mibIfRow;
            InterfaceInfo intfInfo;

            memset(&mibIfRow, 0, sizeof(mibIfRow));
            mibIfRow.dwIndex = pInfo->Adapter[i].Index;

            ulRetVal = GetIfEntry(&mibIfRow);
            if (ulRetVal != NO_ERROR)
            {
                CIMPLE_ERR(("GetIfEntry(dwIndex=%d) failed with rc=%lu",
                            ulRetVal));
                FREE(pInfo);
                info.clear();
                return -1;
            }

            intfInfo.Name = wstr2str(pInfo->Adapter[i].Name);
            intfInfo.Descr = String("");
            intfInfo.Descr.append(reinterpret_cast<char *>(mibIfRow.bDescr),
                                  mibIfRow.dwDescrLen);
            intfInfo.Index = pInfo->Adapter[i].Index;
            intfInfo.MTU = mibIfRow.dwMtu;
            intfInfo.AdminStatus =  mibIfRow.dwAdminStatus;
            intfInfo.PhysAddr.clear();
            for (j = 0; j < mibIfRow.dwPhysAddrLen; j++)
                intfInfo.PhysAddr.append(
                            static_cast<int>(mibIfRow.bPhysAddr[j]));
            info.append(intfInfo);
        }

        FREE(pInfo);
        return 0;
    }

    ///
    /// Get descriptions for all Solarflare NICs on the machine.
    ///
    /// @param nics          [out] Where to save NIC descriptions
    ///
    /// @return 0 on success or -1 on failure
    ///
    static int getNICs(NICDescrs &nics)
    {
        int                       rc;
        Array<IWbemClassObject *> ports;
        Array<IWbemClassObject *> pciInfos;
        Array<PortDescr>          portDescrs;
        Array<InterfaceInfo>      intfsInfo;
        PortDescr                 portDescr;
        unsigned int              i;
        unsigned int              j;
        bool                      clearPciInfos = false;
        int                       pci_cmp_rc;

        rc = wmiEnumInstancesQuery(NULL,
                                   "SELECT * FROM EFX_Port "
                                   "WHERE DummyInstance='false'",
                                   ports);
        if (rc < 0)
            return rc;

        rc = getInterfacesInfo(intfsInfo);
        if (rc < 0)
            goto cleanup;

        for (i = 0; i < ports.size(); i++)
        {
            if (wmiGetIntProp<int>(ports[i], "Id",
                                   &portDescr.port_id) != 0)
            {
                rc = -1;
                goto cleanup;
            }

            if (wmiGetStringProp(ports[i], "InstanceName",
                                 portDescr.deviceInstanceName) != 0)
            {
                rc = -1;
                goto cleanup;
            }
            if (wmiGetIntArrProp<int>(ports[i], "PermanentMacAddress",
                                      portDescr.permanentMAC) != 0 ||
                wmiGetIntArrProp<int>(ports[i], "CurrentMacAddress",
                                      portDescr.currentMAC) != 0)

            {
                rc = -1;
                goto cleanup;
            }

            for (j = 0; j < intfsInfo.size(); j++)
            {
                if (intfsInfo[j].PhysAddr == portDescr.currentMAC)
                    break;
            }

            if (j == intfsInfo.size())
            {
                CIMPLE_ERR(("Failed to find network interface information "
                            "for portId=%d", portDescr.port_id));
                rc = -1;
                goto cleanup;
            }

            portDescr.ifInfo = intfsInfo[j];

            if (wmiEnumInstances(NULL, "EFX_PciInformation",
                                 pciInfos) != 0)
            {
                rc = -1;
                goto cleanup;
            }

            for (j = 0; j < pciInfos.size(); j++)
            {
                int pciPortId;

                if (wmiGetIntProp<int>(pciInfos[j], "PortId",
                                       &pciPortId) != 0)
                {
                    rc = -1;
                    clearPciInfos = true;
                    goto cleanup;
                }

                if (pciPortId == portDescr.port_id)
                    break;
            }
            if (j == pciInfos.size())
            {
                CIMPLE_ERR(("Failed to find matching EFX_PciInformation for "
                            "PortId=%d", portDescr.port_id));
                rc = -1;
                clearPciInfos = true;
                goto cleanup;
            }

            if (wmiGetIntProp<int>(pciInfos[j], "BridgeBusNumber",
                                   &portDescr.pci_bridge_bus) != 0 ||
                wmiGetIntProp<int>(pciInfos[j], "BridgeDeviceNumber",
                                   &portDescr.pci_bridge_device) != 0 ||
                wmiGetIntProp<int>(pciInfos[j], "BridgeFunctionNumber",
                                   &portDescr.pci_bridge_fn) != 0 ||
                wmiGetIntProp<int>(pciInfos[j], "BusNumber",
                                   &portDescr.pci_bus) != 0 ||
                wmiGetIntProp<int>(pciInfos[j], "DeviceNumber",
                                   &portDescr.pci_device) != 0 ||
                wmiGetIntProp<int>(pciInfos[j], "FunctionNumber",
                                   &portDescr.pci_fn) != 0)
            {
                CIMPLE_ERR(("Failed to obtain all required EFX_PciInformation "
                            "properties for PortId=%d", portDescr.port_id));
                rc = -1;
                clearPciInfos = true;
                goto cleanup;
            }

            for (j = 0; j < pciInfos.size(); j++)
                pciInfos[j]->Release();
            pciInfos.clear();

            for (j = 0; j < portDescrs.size(); j++)
            {
                pci_cmp_rc = portDescr.pci_compare(portDescrs[j]);
                if (pci_cmp_rc <= 0)
                    break;
            }
            if (pci_cmp_rc != 0)
            {
                portDescr.efxPort = ports[i];
                ports[i] = NULL;
                portDescrs.insert(j, portDescr);
            }
            else
            {
                ports[i]->Release();
                ports[i] = NULL;
            }
        }

        for (j = 0; j < portDescrs.size(); j++)
        {
            int last = nics.size() - 1;
        
            if (last >= 0 && 
                portDescrs[j].pci_bridge_bus == 
                                  nics[last].pci_bridge_bus &&
                portDescrs[j].pci_bridge_device == 
                                  nics[last].pci_bridge_device &&
                portDescrs[j].pci_bridge_fn == 
                                  nics[last].pci_bridge_fn &&
                portDescrs[j].pci_bus == 
                                  nics[last].pci_bus &&
                portDescrs[j].pci_device == 
                                  nics[last].pci_device)
            {
                nics[last].ports.append(portDescrs[j]);
            }
            else
            {
                NICDescr nic;

                nic.pci_bridge_bus = portDescrs[j].pci_bridge_bus;
                nic.pci_bridge_device = portDescrs[j].pci_bridge_device;
                nic.pci_bridge_fn = portDescrs[j].pci_bridge_fn;
                nic.pci_bus = portDescrs[j].pci_bus;
                nic.pci_device = portDescrs[j].pci_device;

                nic.ports.append(portDescrs[j]);
                nics.append(nic);
            }
        }

cleanup:
        if (clearPciInfos)
        {
            for (j = 0; j < pciInfos.size(); j++)
                pciInfos[j]->Release();
            pciInfos.clear();
        }

        for (i = 0; i < ports.size(); i++)
            if (ports[i] != NULL)
                ports[i]->Release();
        ports.clear();

        if (rc != 0)
        {
            for (i = 0; i < portDescrs.size(); i++)
                portDescrs[i].clear();
        }

        return rc;
    }

    class WindowsDiagnostic : public Diagnostic {
        const Port *owner;
        Result testPassed;
        static const String diagGenName;
        IWbemClassObject *efxDiagTest;
        IWbemClassObject *currentJob;
        mutable Mutex currentJobLock;
    public:
        WindowsDiagnostic(const Port *o) :
            Diagnostic(""), owner(o), testPassed(NotKnown),
            efxDiagTest(NULL), currentJob(NULL),
            currentJobLock(false) {}
        void setEfxDiagTest (IWbemClassObject *obj)
        {
            efxDiagTest = obj;
        }
        void clear()
        {
            if (efxDiagTest != NULL)
                efxDiagTest->Release();
            efxDiagTest = NULL;
        }
        virtual Result syncTest();
        virtual Result result() const { return testPassed; }
        virtual const NIC *nic() const { return owner->nic(); }
        virtual void initialize()
        {
            String d;

            if (efxDiagTest == NULL)
                return;

            if (wmiGetStringProp(efxDiagTest, "Description", d) == 0)
                setDescription(d);
        };
        virtual unsigned percentage() const
        {
            Auto_Mutex    guard(currentJobLock);
            unsigned int  progress;
            int           rc;

            if (currentJob == NULL)
                return 100;

            rc = wmiGetIntProp<unsigned int>(currentJob, "Progress",
                                             &progress);
            if (rc != 0)
                return 0;
            else
                return progress;
        }
        virtual void stop()
        {
            Auto_Mutex    guard(currentJobLock);
            String        abortMethod("EFX_DiagnosticJob_Abort");
            String        jobPath;
            int           rc;
        
            if (currentJob == NULL)
                return;

            rc = wmiGetStringProp(currentJob, "__Path", jobPath);
            if (rc != 0)
                return;

            wmiExecMethod(jobPath, abortMethod,
                          NULL, NULL);
        };
        virtual bool isDestructive() const
        {
            bool          requiresQuiescence = false;
            int           rc;

            if (efxDiagTest == NULL)
                return false;

            rc = wmiGetBoolProp(efxDiagTest, "RequiresQuiescence",
                                &requiresQuiescence);
            if (rc != 0)
                return true; // Suppose the worst

            return requiresQuiescence;
        }
        virtual const String& genericName() const { return diagGenName; }

        String name() const
        {
            String testName;
            Buffer buf;

            buf.appends(owner->name().c_str());
            buf.append(' ');
            buf.append_uint16(owner->elementId());
            buf.appends(": ");

            if (wmiGetStringProp(efxDiagTest, "Name", testName) == 0)
                buf.appends(testName.c_str());
            else
                buf.appends("Unknown test");

            return buf.data();
        }

        // Dummy operator to make possible using of cimple::Array
        inline bool operator== (const WindowsDiagnostic &rhs)
        {
            UNUSED(rhs);
            return false;
        }
    };

    const String WindowsDiagnostic::diagGenName = "Diagnostic";

    /// Forward declaration
    static int windowsFillPortAlertsInfo(Array<AlertInfo *> &info,
                                         const Port *port);

    class WindowsPort : public Port {
        const NIC *owner;
        Array<WindowsDiagnostic *> diags;
        friend int windowsFillPortAlertsInfo(Array<AlertInfo *> &info,
                                             const Port *port);
    public:
        PortDescr portInfo;

        WindowsPort(const NIC *up, unsigned i, PortDescr &descr) :
            Port(i), 
            owner(up), 
            portInfo(descr) {}

        void clear()
        {
            unsigned int i;

            portInfo.clear();

            for (i = 0; i < diags.size(); i++)
            {
                diags[i]->clear();
                delete diags[i];
                diags[i] = NULL;
            }
            diags.clear();
        }
        
        virtual bool linkStatus() const
        {
            return portInfo.linkUp();
        }
        virtual Speed linkSpeed() const
        {
            return speedValue(portInfo.linkSpeed());
        }            
        virtual void linkSpeed(Speed sp) { }
            
        /// @return full-duplex state
        virtual bool fullDuplex() const
        {
            return (portInfo.linkDuplex() > 0 ? true : false);
        }
        /// enable or disable full-duplex mode depending on @p fd
        virtual void fullDuplex(bool fd) { };
        /// @return true if autonegotiation is available
        virtual bool autoneg() const
        {
            return portInfo.autoneg();
        };
        /// enable/disable autonegotiation according to @p an
        virtual void autoneg(bool an) { }
        
        /// causes a renegotiation like 'ethtool -r'
        virtual void renegotiate() {};

        /// @return Manufacturer-supplied MAC address
        virtual MACAddress permanentMAC() const
        {
            return MACAddress(portInfo.permanentMAC[0],
                              portInfo.permanentMAC[1],
                              portInfo.permanentMAC[2],
                              portInfo.permanentMAC[3],
                              portInfo.permanentMAC[4],
                              portInfo.permanentMAC[5]);
        };

        virtual const NIC *nic() const { return owner; }
        virtual PCIAddress pciAddress() const
        {
            return owner->pciAddress().fn(0);
        }

        virtual void initialize()
        {
            int                       rc = 0;
            Array<IWbemClassObject *> efxDiagTests;
            unsigned int              i;
            char                      query[WMI_QUERY_MAX];
 
            WindowsDiagnostic *diag = NULL;

            // WillBlock='false' means that we do not run tests
            // from provider which "require user assistance".
            rc = snprintf(query, sizeof(query),
                          "SELECT * FROM EFX_DiagnosticTest "
                          "WHERE DummyInstance='false' AND PortId=%d "
                          "AND WillBlock='false'",
                          portInfo.port_id);
            if (rc < 0 || rc >= static_cast<int>(sizeof(query)))
            {
                CIMPLE_ERR(("%s(): failed to construct a query",
                            __FUNCTION__));
                return;
            }
            rc = wmiEnumInstancesQuery(NULL, query, efxDiagTests);
            if (rc != 0)
                return;

            for (i = 0; i < efxDiagTests.size(); i++)
            {
                if (diag != NULL)
                    delete diag;
                diag = new WindowsDiagnostic(this);
                diag->setEfxDiagTest(efxDiagTests[i]);
                efxDiagTests[i] = NULL;
                diag->initialize();
                diags.append(diag);
                diag = NULL;
            }

cleanup:
            if (diag != NULL)
                delete diag;

            for (i = 0; i < efxDiagTests.size(); i++)
                if (efxDiagTests[i] != NULL)
                    efxDiagTests[i]->Release();

            if (rc != 0)
            {
                for (i = 0; i < diags.size(); i++)
                {
                    diags[i]->clear();
                    delete diags[i];
                    diags[i] = NULL;
                }
                diags.clear();
            }
        };

        virtual bool forAllDiagnostics(ElementEnumerator& en)
        {
            unsigned int i;

            for (i = 0; i < diags.size(); i++)
                if (!en.process(*(diags[i])))
                    return false;

            return true;
        }
        
        virtual bool forAllDiagnostics(ConstElementEnumerator& en) const
        {
            unsigned int i;

            for (i = 0; i < diags.size(); i++)
                if (!en.process(*(diags[i])))
                    return false;

            return true;
        }

        // Dummy operator to make possible using of cimple::Array
        inline bool operator== (const WindowsPort &rhs)
        {
            UNUSED(rhs);
            return false;
        }
    };

    class WindowsInterface : public Interface {
        const NIC *owner;
        WindowsPort *boundPort;
    public:
        WindowsInterface(const NIC *up, unsigned i) : 
            Interface(i), 
            owner(up), 
            boundPort(NULL)
        { }
        virtual bool ifStatus() const
        {
            PortDescr *portInfo = &boundPort->portInfo;

            return
              (portInfo->ifInfo.AdminStatus == MIB_IF_ADMIN_STATUS_UP ? 
                                                            true : false);
        }

        virtual void enable(bool st)
        {
            DWORD      rc;
            MIB_IFROW  mibIfRow;
            PortDescr *portInfo = &boundPort->portInfo;
        
            memset(&mibIfRow, 0, sizeof(mibIfRow));
            mibIfRow.dwIndex = portInfo->ifInfo.Index;
            if (st)
                mibIfRow.dwAdminStatus = MIB_IF_ADMIN_STATUS_UP;
            else
                mibIfRow.dwAdminStatus = MIB_IF_ADMIN_STATUS_DOWN;

            rc = SetIfEntry(&mibIfRow);
            if (rc != NO_ERROR)
                CIMPLE_ERR(("SetIfEntry() failed with rc=0x%lx",
                            static_cast<long int>(rc)));
        }

        /// @return current MTU
        virtual uint64 mtu() const
        {
            PortDescr *portInfo = &boundPort->portInfo;

            return portInfo->ifInfo.MTU;
        }            
        /// change MTU to @p u
        virtual void mtu(uint64 u) { };
        /// @return system interface name (e.g. ethX for Linux)
        virtual String ifName() const;
        /// @return MAC address actually in use
        virtual MACAddress currentMAC() const
        {
            PortDescr *portInfo = &boundPort->portInfo;

            return MACAddress(portInfo->currentMAC[0],
                              portInfo->currentMAC[1],
                              portInfo->currentMAC[2],
                              portInfo->currentMAC[3],
                              portInfo->currentMAC[4],
                              portInfo->currentMAC[5]);
        }        
        /// Change the current MAC address to @p mac
        virtual void currentMAC(const MACAddress& mac);

        virtual const NIC *nic() const { return owner; }
        virtual PCIAddress pciAddress() const
        {
            return owner->pciAddress().fn(0);
        }

        virtual Port *port() { return boundPort; }
        virtual const Port *port() const { return boundPort; }

        void bindToPort(Port *p)
        {
            boundPort = dynamic_cast<WindowsPort *>(p);
        }
            
        virtual void initialize() {};

        // Dummy operator to make possible using of cimple::Array
        inline bool operator== (const WindowsInterface &rhs)
        {
            UNUSED(rhs);
            return false;
        }
    };

    String WindowsInterface::ifName() const
    {
        PortDescr *portInfo = &boundPort->portInfo;

        return portInfo->ifInfo.Name;
    }
    
    void WindowsInterface::currentMAC(const MACAddress& mac)
    {
        PortDescr *portInfo = &boundPort->portInfo;

        Array<IWbemClassObject *>   efxNetAdapters;

        unsigned int              i;
        LONG                      j;
        int                       rc;
        String                    objPath;
        long int                  CompletionCode;
        IWbemClassObject         *pIn = NULL;
        IWbemClassObject         *pOut = NULL;

        HRESULT   hr;
        VARIANT   macValue;

        SAFEARRAYBOUND bound[1];

        String methodName("EFX_NetworkAdapter_SetNetworkAddress");

        if (wmiEnumInstancesQuery(NULL,
                                  "SELECT * FROM EFX_NetworkAdapter "
                                  "WHERE DummyInstance='false'",
                                  efxNetAdapters) != 0)
            return;

        for (i = 0; i < efxNetAdapters.size(); i++)
        {
            Array<int> netAddr;

            if (wmiGetIntArrProp<int>(efxNetAdapters[i], "NetworkAddress",
                                      netAddr) != 0)
            {
                CIMPLE_ERR(("%s():   failed to get NetworkAddress property "
                            "value for EFX_NetworkAdapter class",
                            __FUNCTION__));
                goto cleanup;
            }

            if (netAddr == portInfo->currentMAC)
                break;
        }

        if (i == efxNetAdapters.size())
        {
            CIMPLE_ERR(("%s():   failed to find matching EFX_NetworkAdapter "
                        "instance", __FUNCTION__));
            goto cleanup;
        }

        if (wmiPrepareMethodCall(efxNetAdapters[i], methodName,
                                 objPath, &pIn) != 0)
        {
            pIn = NULL;
            goto cleanup;
        }

        macValue.vt = (VT_BOOL | VT_ARRAY);
        bound[0].lLbound = 0;
        bound[0].cElements = MAC_ADDR_BYTES;
        V_ARRAY(&macValue) = SafeArrayCreate(VT_BOOL, 1, bound);
        if (V_ARRAY(&macValue) == NULL)
        {
            CIMPLE_ERR(("%s():   failed to create "
                        "array for MAC representation",
                        __FUNCTION__));
            goto cleanup;
        }

        for (j = 0; j < MAC_ADDR_BYTES; j++)
        {
            hr = SafeArrayPutElement(
                          V_ARRAY(&macValue), &j,
                          reinterpret_cast<void *>(
                            const_cast<unsigned char *>(&mac.address[j])));
            if (FAILED(hr))
            {
                CIMPLE_ERR(("%s():   failed to prepare MAC value for "
                            "calling SetNetwordAddress(), rc=%lx",
                            __FUNCTION__, hr));
                VariantClear(&macValue);
                goto cleanup;
            }
        }

        hr = pIn->Put(BString("Value").rep(), 0, &macValue, 0);
        VariantClear(&macValue);
        if (FAILED(hr))
        {
            CIMPLE_ERR(("%s():   failed to set MAC value for calling "
                        "SetNetwordAddress(), rc=%lx", __FUNCTION__, hr));
            goto cleanup;
        }

        CoImpersonateClient();
        hr = rootWMIConn->ExecMethod(BString(objPath).rep(),
                                     BString(methodName).rep(),
                                     0, NULL, pIn, &pOut, NULL);
        CoRevertToSelf();
        if (FAILED(hr))
        {
            CIMPLE_ERR(("%s():   failed to call "
                        "EFX_NetworkAdapter_SetNetworkAddress() "
                        "method, rc=%lx", __FUNCTION__, hr));
            pOut = NULL;
            goto cleanup;
        }

        if (wmiGetIntProp<long int>(pOut, "CompletionCode",
                                    &CompletionCode) != 0)
            goto cleanup;

        if (CompletionCode != 0 &&
            CompletionCode != BUS_WMI_COMPLETION_CODE_APPLY_REQUIRED)
        {
            CIMPLE_ERR(("%s():   wrong completion code %ld (0x%lx)) "
                        "returned by method "
                        "EFX_NetworkAdapter_SetNetworkAddress()",
                        __FUNCTION__, CompletionCode, CompletionCode));
            goto cleanup;
        }

        if (CompletionCode == BUS_WMI_COMPLETION_CODE_APPLY_REQUIRED)
        {
            String methodApplyName("EFX_NetworkAdapter_ApplyChanges");
            
            if (pOut != NULL)
                pOut->Release();
            pOut = NULL;

            rc = wmiExecMethod(objPath, methodApplyName,
                               NULL, &pOut);
            if (rc < 0)
            {
                CIMPLE_ERR(("%s():   failed to call "
                            "EFX_NetworkAdapter_ApplyChanges() "
                            "method, rc=%lx", __FUNCTION__, hr));
                goto cleanup;
            }
        }

cleanup:

        if (pIn != NULL)
            pIn->Release();
        if (pOut != NULL)
            pOut->Release();

        for (i = 0; i < efxNetAdapters.size(); i++)
        {
            efxNetAdapters[i]->Release();
            efxNetAdapters[i] = NULL;
        }
    }

    class WindowsNICFirmware : public NICFirmware {
        const NIC *owner;
    public:
        WindowsNICFirmware(const NIC *o) :
            owner(o) {}
        virtual const NIC *nic() const { return owner; }
        virtual VersionInfo version() const;
        virtual bool syncInstall(const char *)
        {
            return true;
        }
        virtual void initialize() {};
    };

    class WindowsBootROM : public BootROM {
        const NIC *owner;
    public:
        WindowsBootROM(const NIC *o) :
            owner(o) {}
        virtual const NIC *nic() const { return owner; }
        virtual VersionInfo version() const;
        virtual bool syncInstall(const char *) { return true; }
        virtual void initialize() {};
    };

    class WindowsDriver : public Driver {
        VersionInfo vers;
    public:
        WindowsDriver(const NICDescr &ndesc);
        WindowsDriver(const String& d, const String& sn,
                     const VersionInfo& v) :
            Driver(d, sn), vers(v) {}
        virtual VersionInfo version() const { return vers; }
        virtual void initialize() {};
        virtual bool syncInstall(const char *) { return false; }
        virtual const String& genericName() const { return description(); }
        virtual const Package *package() const { return NULL; }
    };

    WindowsDriver::WindowsDriver(const NICDescr &ndesc) : Driver("", "")
    {
        String path("SELECT * FROM Win32_PnPSignedDriver WHERE DeviceID='");
        
        const char *inst = ndesc.ports[0].deviceInstanceName.c_str();
        const char *lastUnderscore = strrchr(inst, '_');

        unsigned int i = 0;

        for (; *inst != '\0' && inst != lastUnderscore; inst++)
        {
            if (*inst == '\\')
                path.append('\\');
            path.append(*inst);
        }
        path.append('\'');

        if (wmiEstablishConn() != 0)
            return;

        Array<IWbemClassObject *> drivers;
        if (wmiEnumInstancesQuery(cimWMIConn, path.c_str(), drivers) < 0)
            return;

        if (drivers.size() > 0)
        {
            String version;
            String description;
            String sysname;
            wmiGetStringProp(drivers[0], "DriverVersion", version);
            wmiGetStringProp(drivers[0], "Description", description);
            wmiGetStringProp(drivers[0], "InfName", sysname);
            *this = WindowsDriver(description, sysname,
                                  VersionInfo(version.c_str()));
        }

        for (i = 0; i < drivers.size(); i++)
            drivers[i]->Release();
    }

    class WindowsNIC : public NIC {
        WindowsNICFirmware nicFw;
        WindowsBootROM rom;
        mutable WindowsDriver nicDriver;
        PCIAddress pciAddr;
    public:
        Array<WindowsPort> ports;
        Array<WindowsInterface> intfs;

    protected:
        virtual void setupPorts()
        {
            int i = 0;

            for (i = 0; i < static_cast<int>(ports.size()); i++)
                ports[i].initialize();
        }
        virtual void setupInterfaces()
        {
            int i = 0;

            for (i = 0; i < static_cast<int>(ports.size()); i++)
            {
                intfs[i].initialize();
                intfs[i].bindToPort(&ports[i]);
            }
        }
        virtual void setupFirmware()
        {
            nicFw.initialize();
            rom.initialize();
        }
        virtual void setupDiagnostics() { }
    public:
        WindowsNIC(unsigned idx, NICDescr &descr) :
            NIC(idx),
            nicFw(this),
            rom(this),
            nicDriver(descr),
            pciAddr(0, descr.pci_bus, descr.pci_device)
        {
            int i = 0;

            for (i = 0; i < static_cast<int>(descr.ports.size()); i++)
            {
                ports.append(WindowsPort(this, i, descr.ports[i]));
                intfs.append(WindowsInterface(this, i));
            }
        }

        void clear()
        {
            unsigned int i = 0;

            for (i = 0; i < ports.size(); i++)
                ports[i].clear();
        }

        virtual VitalProductData vitalProductData() const 
        {
          
            if (ports.size() > 0)
            {
                const PortDescr *portInfo = &ports[0].portInfo;

                return VitalProductData(portInfo->serialNumber(),
                                        "",
                                        portInfo->serialNumber(),
                                        portInfo->productNumber(),
                                        portInfo->productName(),
                                        portInfo->productNumber());
            }
            else
                return VitalProductData("", "", "", "", "", "");
        }
        Connector connector() const
        {
            VitalProductData        vpd = vitalProductData();
            const char             *part = vpd.part().c_str();
            char                    last = 'T';

            if (*part != '\0')
                last = part[strlen(part) - 1];

            switch (last)
            {
                case 'F': return NIC::SFPPlus;
                case 'K': // fallthrough
                case 'H': return NIC::Mezzanine;
                case 'T': return NIC::RJ45;

                default: return NIC::RJ45;
            }
        }
        uint64 supportedMTU() const
        {
            // This one defined in the driver (EFX_MAX_MTU)
            return EFX_MAX_MTU;
        }
        virtual bool forAllFw(ElementEnumerator& en)
        {
            if(!en.process(nicFw))
                return false;
            return en.process(rom);
        }
        virtual bool forAllFw(ConstElementEnumerator& en) const
        {
            if(!en.process(nicFw))
                return false;
            return en.process(rom);
        }
        virtual bool forAllPorts(ElementEnumerator& en)
        {
            int i = 0;

            for (i = 0; i < static_cast<int>(ports.size()); i++)
                if (!en.process(ports[i]))
                    return false;

            return true;
        }
        
        virtual bool forAllPorts(ConstElementEnumerator& en) const
        {
            int i = 0;

            for (i = 0; i < static_cast<int>(ports.size()); i++)
                if (!en.process(ports[i]))
                    return false;

            return true;
        }

        virtual bool forAllInterfaces(ElementEnumerator& en)
        {
            int i = 0;

            for (i = 0; i < static_cast<int>(intfs.size()); i++)
            {
                if (!en.process(intfs[i]))
                    return false;
            }

            return true;
        }
        
        virtual bool forAllInterfaces(ConstElementEnumerator& en) const
        {
            int i = 0;

            for (i = 0; i < static_cast<int>(intfs.size()); i++)
            {
                if (!en.process(intfs[i]))
                    return false;
            }

            return true;
        }

        virtual bool forAllDiagnostics(ElementEnumerator& en)
        {
            unsigned int i;
            bool         ret = true;

            for (i = 0; i < ports.size(); i++)
                ret = ret && ports[i].forAllDiagnostics(en);

            return ret;
        }
        
        virtual bool forAllDiagnostics(ConstElementEnumerator& en) const
        {
            unsigned int i;
            bool         ret = true;

            for (i = 0; i < ports.size(); i++)
                ret = ret && ports[i].forAllDiagnostics(en);

            return ret;
        }

        // I failed to find PCI domain on Windows; instead there are
        // separate bus, device and function numbers for PCI bridge.
        // Anyway, we do not use PCI data for any other purpose that for
        // distinguishing ports belonging to different NICs.
        virtual PCIAddress pciAddress() const
        {
            return pciAddr;
        }

        virtual Driver *driver() const { return &nicDriver; }
    };

    class WindowsLibrary : public Library {
        const Package *owner;
        const VersionInfo vers;
        WindowsLibrary(const Package *pkg, const char *dllName,
                       const char *descr, const VersionInfo& vi) :
            Library(descr, dllName), owner(pkg), vers(vi) {}
    public:
        static WindowsLibrary dllLibrary(const Package *pkg,
                                         const char *dllName);
        static WindowsLibrary moduleLibrary(const Package *pkg);
        virtual VersionInfo version() const { return vers; };
        virtual void initialize() {};
        virtual bool syncInstall(const char *) { return false; }
        virtual const String& genericName() const { return description(); }
        virtual const Package *package() const { return owner; }
    };

    WindowsLibrary WindowsLibrary::dllLibrary(const Package *pkg,
                                              const char *dllName)
    {
        DWORD size = GetFileVersionInfoSizeA(dllName, NULL);
        void *data = new char[size];
        GetFileVersionInfoA(dllName, 0, size, data);
        VS_FIXEDFILEINFO *fixedInfo;
        const char *descr;
        const char *vstr;
        unsigned blocklen;
        VersionInfo ver;

        VerQueryValue(data, _T("\\"), reinterpret_cast<void **>(&fixedInfo),
                      &blocklen);
        VerQueryValue(
                  data,
                  _T("\\StringFileInfo\\040904E4\\FileDescription"),
                  reinterpret_cast<void **>(const_cast<char **>(&descr)),
                  &blocklen);
        VerQueryValue(
                  data,
                  _T("\\StringFileInfo\\040904E4\\ProductVersion"),
                  reinterpret_cast<void **>(const_cast<char **>(&vstr)),
                  &blocklen);

        ver =  VersionInfo(fixedInfo->dwProductVersionMS >> 16,
                           fixedInfo->dwProductVersionMS & 0xFFFFU,
                           fixedInfo->dwProductVersionLS >> 16,
                           fixedInfo->dwProductVersionLS & 0xFFFFU,
                           vstr,
                           Datetime(((uint64)fixedInfo->dwFileDateMS << 32) |
                                    fixedInfo->dwFileDateLS));
        delete[] reinterpret_cast<char *>(data);
        
        return WindowsLibrary(pkg, dllName, descr, ver);
    }

    WindowsLibrary WindowsLibrary::moduleLibrary(const Package *pkg)
    {
        HMODULE hmod;
        char path[MAX_PATH + 1];

        GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT |
                          GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
                          reinterpret_cast<LPCTSTR>(&moduleLibrary),
                          &hmod);
        GetModuleFileNameA(hmod,  path, sizeof(path));
        return dllLibrary(pkg, path);
    }

    /// @brief stub-only implementation of a software package
    /// with provider library
    class WindowsManagementPackage : public Package {
        WindowsLibrary providerLibrary;

       static const char *getUninstallerPath()
       {
           static char path[MAX_PATH];
           DWORD len = sizeof(path);
           RegGetValue(
              HKEY_LOCAL_MACHINE,
              _T("Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall"),
              _T("UninstallString"),
              RRF_RT_REG_SZ,
              NULL,
              path,
              &len);
           return path;
       }

    protected:
        virtual void setupContents() { providerLibrary.initialize(); };
    public:
        WindowsManagementPackage() :
            Package("CIM Provider MSI", getUninstallerPath()),
            providerLibrary(WindowsLibrary::moduleLibrary(this)) {}
        virtual PkgType type() const { return MSI; }
        virtual VersionInfo version() const
        {
            return providerLibrary.version();
        }
        virtual bool syncInstall(const char *) { return true; }
        virtual bool forAllSoftware(ElementEnumerator& en)
        {
            return en.process(providerLibrary);
        }
        virtual bool forAllSoftware(ConstElementEnumerator& en) const
        {
            return en.process(providerLibrary);
        }
        virtual const String& genericName() const { return description(); }
    };

    /// @brief stub-only System implementation
    /// @note all structures are initialised statically,
    /// so initialize() does nothing 
    class WindowsSystem : public System {
        WindowsManagementPackage mgmtPackage;
        WindowsSystem()
        {
            onAlert.setFillPortAlertsInfo(windowsFillPortAlertsInfo);
        };
        ~WindowsSystem();
    protected:
        void setupNICs()
        {
            // Do nothing
        };

        void setupPackages()
        {
            mgmtPackage.initialize();
        };
        bool forAllDrivers(ConstElementEnumerator& en) const;
        bool forAllDrivers(ElementEnumerator& en);
    public:
        static WindowsSystem target;
        bool is64bit() const { return true; }
        OSType osType() const
        {
            OSVERSIONINFOEX verInfo;

            memset(&verInfo, 0, sizeof(verInfo));
            verInfo.dwOSVersionInfoSize = sizeof(verInfo);

            if (!GetVersionEx(reinterpret_cast<OSVERSIONINFO *>(&verInfo)))
            {
                CIMPLE_ERR(("%s():    failed to call GetVersionEx()",
                            __FUNCTION__));
                // FIXME: no "unknown" value in enum
                return WindowsServer2003;
            }

            if (verInfo.dwMajorVersion == 6 &&
                verInfo.dwMinorVersion == 1 &&
                verInfo.wProductType != VER_NT_WORKSTATION)
                return WindowsServer2008R2;
            else if (verInfo.dwMajorVersion == 6 &&
                verInfo.dwMinorVersion == 0 &&
                verInfo.wProductType != VER_NT_WORKSTATION)
                return WindowsServer2008;
            else if (verInfo.dwMajorVersion == 5 &&
                verInfo.dwMinorVersion == 2 &&
                GetSystemMetrics(SM_SERVERR2) == 0)
                return WindowsServer2003;

            // FIXME: no "unknown" value in enum
            return WindowsServer2003;
        }
        bool forAllNICs(ConstElementEnumerator& en) const;
        bool forAllNICs(ElementEnumerator& en);
        bool forAllPackages(ConstElementEnumerator& en) const;
        bool forAllPackages(ElementEnumerator& en);
        bool forAllSoftware(ConstElementEnumerator& en) const;
        bool forAllSoftware(ElementEnumerator& en);
    };

    WindowsSystem::~WindowsSystem()
    {
        wmiReleaseConn();
    }

    bool WindowsSystem::forAllNICs(ElementEnumerator& en)
    {
        NICDescrs   nics;
        unsigned    i;
        bool        res;
        bool        ret = true;
        int         rc;

        if ((rc = getNICs(nics)) < 0)
        {
            CIMPLE_ERR(("Failed to obtain info about NICs"));
            return false;
        }

        for (i = 0; i < nics.size(); i++)
        {
            WindowsNIC  nic(i, nics[i]);

            nic.initialize();
            if (ret)
                res = en.process(nic);
            ret = ret && res;
            nic.clear();
        }

        return ret;
    }

    bool WindowsSystem::forAllNICs(ConstElementEnumerator& en) const
    {
        return const_cast<WindowsSystem *>
                    (this)->forAllNICs((ElementEnumerator&) en); 
    }

    bool WindowsSystem::forAllDrivers(ElementEnumerator &en)
    {
        int  rc;
        bool result = true;
        Array<IWbemClassObject *> drivers;

        rc = wmiEnumInstancesQuery(cimWMIConn, 
                                   "SELECT * FROM Win32_PnPSignedDriver "
                                   "WHERE Manufacturer='Solarflare'",
                                   drivers);
        if (rc < 0)
            return false;

        for (unsigned i = 0; i < drivers.size(); i++)
        {
            String version;
            String description;
            String sysname;

            wmiGetStringProp(drivers[i], "DriverVersion", version);
            wmiGetStringProp(drivers[i], "Description", description);
            wmiGetStringProp(drivers[i], "InfName", sysname);

            WindowsDriver drv(description, sysname,
                              VersionInfo(version.c_str()));

            if (!en.process(drv))
            {
                result = false;
                break;
            }
        }

        for (unsigned i = 0; i < drivers.size(); i++)
            drivers[i]->Release();

        return result;
    }

    bool WindowsSystem::forAllDrivers(ConstElementEnumerator& en) const
    {
        return const_cast<WindowsSystem *>
                  (this)->forAllDrivers((ElementEnumerator&) en); 
    }
    
    bool WindowsSystem::forAllPackages(ConstElementEnumerator& en) const
    {
        return en.process(mgmtPackage);
    }
    
    bool WindowsSystem::forAllPackages(ElementEnumerator& en)
    {
        return en.process(mgmtPackage);
    }

    bool WindowsSystem::forAllSoftware(ConstElementEnumerator& en) const
    {
        if (!System::forAllSoftware(en))
            return false;
        return forAllDrivers(en);
    }
    
    bool WindowsSystem::forAllSoftware(ElementEnumerator& en)
    {
        if (!System::forAllSoftware(en))
            return false;
        return forAllDrivers(en);
    }

    WindowsSystem WindowsSystem::target;

    System& System::target = WindowsSystem::target;

    VersionInfo WindowsNICFirmware::version() const
    {
        if ((dynamic_cast<const WindowsNIC *>(owner))->ports.size() == 0)
            return VersionInfo("0.0.0");
        else
            return VersionInfo(
                        (dynamic_cast<const WindowsNIC *>(owner))->ports[0].
                                        portInfo.mcfwVersion().c_str());
    }

    VersionInfo WindowsBootROM::version() const
    {
        if ((dynamic_cast<const WindowsNIC *>(owner))->ports.size() == 0)
            return VersionInfo("0.0.0");
        else
            return VersionInfo(
                  (dynamic_cast<const WindowsNIC *>(owner))->ports[0].
                                        portInfo.bootROMVersion().c_str());
    }

    ///
    /// Class defining link state alert indication to be
    /// generated on Windows
    ///
    class WindowsLinkStateAlertInfo : public LinkStateAlertInfo {
        int  portId;                ///< Id of EFX_Port instance
                                    ///  to be checked for alert
                                    ///  conditions.

    protected:

        virtual int updateLinkState()
        {
            char query[WMI_QUERY_MAX]; 
            int  rc;

            unsigned int i;
            bool         linkUp = false;
            int          result = 0;

            Array<IWbemClassObject *> ports;

            rc = snprintf(query, sizeof(query), "SELECT * FROM EFX_Port "
                          "WHERE Id='%d' AND DummyInstance='False'",
                          this->portId);
            if (rc < 0 || rc >= static_cast<int>(sizeof(query)))
                return -1;

            rc = wmiEnumInstancesQuery(NULL, query, ports);
            if (rc < 0)
                return rc;

            if (ports.size() != 1)
            {
                CIMPLE_ERR(("%s(): for id=%d wrong number "
                            "%u of matching ports obtained",
                            __FUNCTION__, this->portId,
                            ports.size()));
                result = -1;
                goto cleanup;
            }

            if ((rc = wmiGetBoolProp(ports[0], "LinkUp", &linkUp)) < 0)
            {
                result = rc;
                goto cleanup;
            }
            curLinkState = linkUp;
cleanup:
            for (i = 0; i < ports.size(); i++)
                ports[i]->Release();
            return result;
        }

    public:
        WindowsLinkStateAlertInfo() : portId(-1) {};

        friend int windowsFillPortAlertsInfo(Array<AlertInfo *> &info,
                                             const Port *port);
    };

    ///
    /// Fill array of alert indication descriptions.
    ///
    /// @param info   [out] Array to be filled
    /// @param port         Reference to port class instance
    ///
    /// @return -1 on failure, 0 on success.
    ///
    static int windowsFillPortAlertsInfo(Array<AlertInfo *> &info,
                                         const Port *port)
    {
        WindowsLinkStateAlertInfo *instInfo = NULL;

        const WindowsPort *windowsPort =
                      dynamic_cast<const WindowsPort *>(port);

        instInfo = new WindowsLinkStateAlertInfo();
        instInfo->portId = windowsPort->portInfo.port_id;
        info.append(instInfo);

        return 0;
    }

    static int EFXPortQuiesce(IWbemClassObject *efxPort,
                              const String &portName,
                              bool &awakePort)
    {
        bool    quiescenceSupported = false;
        bool    quiescent = false;
        String  quiescingMethod("EFX_Port_Quiesce");
        String  portPath;
        int     rc;

        rc = wmiGetBoolProp(efxPort, "QuiescenceSupported",
                            &quiescenceSupported);
        if (rc != 0)
            return rc;
        if (!quiescenceSupported)
        {
            CIMPLE_ERR(("%s():   ethernet port %s cannot be quiesced",
                        __FUNCTION__, portName.c_str()));
            return -1;
        }

        rc = wmiGetBoolProp(efxPort, "Quiescent", &quiescent);
        if (rc != 0)
            return rc;

        if (!quiescent)
        {
            rc = wmiPrepareMethodCall(efxPort, quiescingMethod,
                                      portPath, NULL);
            if (rc != 0)
                return rc;

            rc = wmiExecMethod(portPath, quiescingMethod,
                               NULL, NULL);
            if (rc != 0)
                return rc;

            awakePort = true;
        }

        return 0;
    }

    Diagnostic::Result WindowsDiagnostic::syncTest() 
    {
        static Mutex  lock(false);
        Auto_Mutex    guard(lock);

        IWbemClassObject *pIn = NULL;
        IWbemClassObject *pOut = NULL;

        HRESULT   hr;
        String    diagTestPath;
        String    jobPath;
        String    createJobMethod("EFX_DiagnosticTest_CreateJob");
        String    deleteJobMethod("EFX_DiagnosticJob_Delete");
        String    startJobMethod("EFX_DiagnosticJob_Start");
        int       rc;
        long int  completionCode;
        uint64    jobId;
        char      query[WMI_QUERY_MAX];

        Array<IWbemClassObject *> jobs;
        Array<IWbemClassObject *> jobConfs;
        unsigned int              iterNum;
        unsigned int              i;
        VARIANT                   argWait;
        bool                      job_created = false;
        bool                      requiresQuiescence = false;
        unsigned int              likelyRunTime = 0;
        unsigned int              maxRunTime = 0;
        unsigned int              elapsedRunTime = 0;
        unsigned int              timeToWait = 100;
        unsigned int              percentage_prev = 0;
        unsigned int              percentage_cur = 0;
        bool                      awakePort = false;
        String                    portPath;

        testPassed = Failed;

        if (wmiEstablishConn() != 0)
            goto cleanup;

        rc = wmiGetStringProp(efxDiagTest, "__Path",
                              diagTestPath);
        if (rc != 0)
            goto cleanup;

        rc = wmiGetIntProp<unsigned int>(efxDiagTest, "LikelyRunTime",
                                         &likelyRunTime);
        if (rc != 0)
            goto cleanup;

        rc = wmiGetBoolProp(efxDiagTest, "RequiresQuiescence",
                            &requiresQuiescence);
        if (rc != 0)
            goto cleanup;

        if (requiresQuiescence)
        {
            rc = EFXPortQuiesce(
              (dynamic_cast<const WindowsPort *>(owner))->portInfo.efxPort,
              (dynamic_cast<const WindowsPort *>(owner))->name(),
              awakePort);
            if (rc != 0)
                goto cleanup;
        }

        rc = wmiExecMethod(diagTestPath, createJobMethod,
                           NULL, &pOut);
        if (rc != 0)
            goto cleanup;

        rc = wmiGetIntProp<uint64>(pOut, "JobId",
                                   &jobId);
        if (rc != 0)
            goto cleanup;

        rc = snprintf(query, sizeof(query), "Select * From "
                      "EFX_DiagnosticJob Where Id=%lu",
                      (long unsigned int)jobId);
        if (rc < 0 || rc >= static_cast<int>(sizeof(query)))
        {
            CIMPLE_ERR(("%s():   failed to create query to search "
                     "for matching EFX_DiagnosticJob",
                     __FUNCTION__));
            goto cleanup;
        }

        rc = wmiEnumInstancesQuery(NULL, query, jobs);
        if (rc != 0)
            goto cleanup;
        if (jobs.size() != 1)
        {
            CIMPLE_ERR(("%s():   incorrect number %u of matching "
                     "diagnostic jobs found", __FUNCTION__,
                     jobs.size()));
            rc = -1;
            goto cleanup;
        }

        rc = wmiGetStringProp(jobs[0], "__Path", jobPath);
        if (rc != 0)
            goto cleanup;
        job_created = true;

        rc = snprintf(query, sizeof(query), "Select * From "
                      "EFX_DiagnosticConfigurationParams Where Id=%lu",
                      static_cast<long unsigned int>(jobId));
        if (rc < 0 || rc >= static_cast<int>(sizeof(query)))
        {
            CIMPLE_ERR(("%s():   failed to create query to search "
                     "for matching EFX_DiagnosticConfigurationParams",
                     __FUNCTION__));
            rc = -1;
            goto cleanup;
        }

        rc = wmiEnumInstancesQuery(NULL, query, jobConfs);
        if (jobConfs.size() != 1)
        {
            CIMPLE_ERR(("%s():   incorrect number %u of matching "
                        "diagnostic job configurations found",
                        __FUNCTION__,
                        jobConfs.size()));
            goto cleanup;
        }

        rc = wmiGetIntProp<unsigned int>(jobConfs[0], "Iterations",
                                         &iterNum);
        if (rc != 0)
            goto cleanup;

        currentJobLock.lock();
        currentJob = jobs[0];
        currentJobLock.unlock();

        pOut->Release();
        pOut = NULL;

        rc = wmiPrepareMethodCall(jobs[0], startJobMethod,
                                  jobPath, &pIn);
        if (rc != 0)
            goto cleanup;

        argWait.vt = VT_BOOL;
        argWait.boolVal = false;
        hr = pIn->Put(BString("wait").rep(), 0,
                      &argWait, 0);
        VariantClear(&argWait);
        if (FAILED(hr))
        {
            CIMPLE_ERR(("%s():   failed to set wait argument for %s"
                        "method, rc=%lx", startJobMethod.c_str(),
                        __FUNCTION__, hr));
            rc = -1;
            goto cleanup;
        }

        rc = wmiExecMethod(jobPath, startJobMethod,
                           pIn, NULL);
        if (rc != 0)
            goto cleanup;

        maxRunTime = (likelyRunTime * 5) + 1000;
        percentage_prev = percentage_cur = percentage();
        do {
            int jobState;

            Sleep(timeToWait);
            elapsedRunTime += timeToWait;

            currentJobLock.lock();
            rc = updateWbemClassObject(&currentJob);
            if (rc != 0)
            {
                currentJobLock.unlock();
                goto cleanup;
            }
            jobs[0] = currentJob;
            currentJobLock.unlock();

            percentage_prev = percentage_cur;
            percentage_cur = percentage();
            rc = wmiGetIntProp<int>(jobs[0], "State", &jobState);
            if (rc != 0)
                goto cleanup;
            if (jobState == JobComplete)
                break;
            if (percentage_prev != percentage_cur)
                onJobProgress.notify(*this);
        } while (elapsedRunTime < maxRunTime);

cleanup:
        if (rc == 0)
        {
            unsigned int iterPassedNum = 0;

            rc = wmiGetIntProp<unsigned int>(jobs[0], "PassCount",
                                             &iterPassedNum);
            if (rc == 0)
            {
                if (iterPassedNum == iterNum)
                    testPassed = Passed;
                else
                    CIMPLE_ERR(("%s():   only %u of %u iteration passed",
                             __FUNCTION__, iterPassedNum, iterNum));
            }
        }

        if (pOut != NULL)
            pOut->Release();

        if (pIn != NULL)
            pIn->Release();

        currentJobLock.lock();
        currentJob = NULL;
        currentJobLock.unlock();
        if (job_created)
            wmiExecMethod(jobPath, deleteJobMethod,
                          NULL, NULL);

        if (awakePort)
        {
            String  awakeMethod("EFX_Port_Awaken");

            wmiExecMethod(portPath, awakeMethod,
                          NULL, NULL);
        }

        for (i = 0; i < jobs.size(); i++)
            jobs[i]->Release();
        for (i = 0; i < jobConfs.size(); i++)
            jobConfs[i]->Release();

        log().logStatus(testPassed == Passed ? "passed" : "failed");

        return testPassed;
    }
}
