#include <winsock2.h>
#include "sf_platform.h"
#include <cimple/Buffer.h>
#include <cimple/Strings.h>

#include <wbemprov.h>
#include <cimple/wmi/log.h>
#include <cimple/wmi/BString.h>
#include <cimple/wmi/utils.h>

#include <ws2ipdef.h>
#include <iphlpapi.h>

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

namespace solarflare 
{
    using cimple::BString;
    using cimple::bstr2cstr;
    using cimple::wstr2str;
    using cimple::uint8;
    using cimple::uint16;

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

        int port_id;            ///< Id property value of corresponding
                                ///  EFX_Port instance

        Array<int> permanentMAC;  ///< Permanent MAC address
        Array<int> currentMAC;    ///< Permanent MAC address

        bool linkUp;            ///< Link status
        uint64 linkSpeed;       ///< Link speed
        int linkDuplex;         ///< Link duplex mode
        bool autoneg;           ///< True if autonegotiation is
                                ///  available

        InterfaceInfo ifInfo;   ///< Network interface-related information

        String mcfwVersion;     ///< MC firmware version
        String bootROMVersion;  ///< BootROM version

        String productName;     ///< Product name from VPD
        String productNumber;   ///< Product number from VPD
        String serialNumber;    ///< Serial number from VPD

        // Dummy operator to make possible using of cimple::Array
        inline bool operator== (const PortDescr &rhs)
        {
            UNUSED(rhs);
            return false;
        }

        inline bool pci_compare(const PortDescr &x)
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
    };

    ///
    /// Vector of NIC descriptions.
    ///
    typedef Array<NICDescr> NICDescrs;

    ///
    /// WMI connection (with namespace root\WMI).
    ///
    static IWbemServices *rootWMIConn = NULL;

    ///
    /// Establish WMI connection.
    ///
    /// @return 0 on success, -1 on failure
    ///
    static int wmiEstablishConn()
    {
        HRESULT       hr;
        IWbemLocator *wbemLocator = NULL;

        if (rootWMIConn != NULL)
            return 0;

        hr = CoCreateInstance(CLSID_WbemLocator, NULL,
                              CLSCTX_INPROC_SERVER,
                              IID_IWbemLocator, (LPVOID *)&wbemLocator);
        if (FAILED(hr))
        {
            LOG_DATA("CoCreateInstance() failed, rc = %lx", hr);
            return -1;
        }

        hr = wbemLocator->ConnectServer(BString("\\\\.\\ROOT\\WMI").rep(),
                                        NULL, NULL, NULL, 0, NULL, NULL,
                                        &rootWMIConn);
        if (FAILED(hr))
        {
            LOG_DATA("ConnectServer() failed, rc = %lx", hr);
            rootWMIConn = NULL;
            return -1;
        }

        wbemLocator->Release();
        return 0;
    }

    ///
    /// Release WMI connection.
    ///
    static void wmiReleaseConn()
    {
        if (rootWMIConn != NULL)
            rootWMIConn->Release();
        rootWMIConn = NULL;
    }

    ///
    /// Get string property value of WMI object.
    ///
    /// @param wbemObj            WMI object pointer
    /// @param propName           Property name
    /// @param value        [out] Where to save obtained value
    ///
    /// @return 0 on success or error code
    ///
    static int wmiGetStringProp(IWbemClassObject *wbemObj,
                                const char *propName,
                                String &str)
    {
        HRESULT  hr;
        VARIANT  wbemObjProp;
        char    *value;

        str = String("");

        hr = wbemObj->Get(BString(propName).rep(), 0,
                          &wbemObjProp, NULL, NULL);
        if (FAILED(hr))
        {
            LOG_DATA("Failed to obtain value of '%s'", propName);
            return -1;
        }

        if (wbemObjProp.vt != VT_BSTR)
        {
            LOG_DATA("Wrong variant type 0x%hx", wbemObjProp.vt);
            VariantClear(&wbemObjProp);
            return -1;
        }
        value = bstr2cstr(wbemObjProp.bstrVal);
        VariantClear(&wbemObjProp);

        if (value == NULL)
        {
            LOG_DATA("%s(): null string obtained", __FUNCTION__);
            return -1;
        }

        str = String(value);
        delete[] value;

        return 0;
    }

    ///
    /// Get integer property value of WMI object.
    ///
    /// @param wbemObj            WMI object pointer
    /// @param propName           Property name
    /// @param value        [out] Where to save obtained value
    ///
    /// @return 0 on success or error code
    ///
    template <class IntType>
    static int wmiGetIntProp(IWbemClassObject *wbemObj,
                             const char *propName,
                             IntType *value)
    {
        HRESULT  hr;
        VARIANT  wbemObjProp;

        hr = wbemObj->Get(BString(propName).rep(), 0,
                          &wbemObjProp, NULL, NULL);
        if (FAILED(hr))
        {
            LOG_DATA("Failed to obtain value of '%s'", propName);
            return -1;
        }

        if (wbemObjProp.vt == VT_I2)
            *value = wbemObjProp.iVal & 0x0000ffff;
        else if (wbemObjProp.vt == VT_I4)
            *value = wbemObjProp.lVal;
        else if (wbemObjProp.vt == VT_INT)
            *value = wbemObjProp.intVal;
        else if (wbemObjProp.vt == VT_UI2)
            *value = wbemObjProp.uiVal & 0x0000ffff;
        else if (wbemObjProp.vt == VT_UI4)
            *value = wbemObjProp.ulVal;
        else if (wbemObjProp.vt == VT_UINT)
            *value = wbemObjProp.uintVal;
        else if (wbemObjProp.vt == VT_UI1)
            *value = wbemObjProp.bVal;
        else if (wbemObjProp.vt == VT_BSTR)
        {
            char *str_val = NULL;
            char *endptr = NULL;

            str_val = bstr2cstr(wbemObjProp.bstrVal);
            *value = strtoll(str_val, &endptr, 10);
            if (endptr == NULL || *endptr != '\0')
            {
                LOG_DATA("Failed to convert '%s' string to int",
                         str_val);
                VariantClear(&wbemObjProp);
                delete[] str_val;
                return -1;
            }
            delete[] str_val;
        }
        else
        {
            LOG_DATA("Wrong variant type 0x%hx", wbemObjProp.vt);
            VariantClear(&wbemObjProp);
            return -1;
        }
        VariantClear(&wbemObjProp);

        return 0;
    }

    ///
    /// Get integer array property value of WMI object.
    ///
    /// @param wbemObj            WMI object pointer
    /// @param propName           Property name
    /// @param value        [out] Where to save obtained value
    ///
    /// @return 0 on success or error code
    ///
    template <class IntType>
    static int wmiGetIntArrProp(IWbemClassObject *wbemObj,
                                const char *propName,
                                Array<IntType> &value)
    {
        HRESULT  hr;
        VARIANT  wbemObjProp;
        VARTYPE  vt;
        int      dims_number;
        LONG     LBound;
        LONG     UBound;
        LONG     i;

        SAFEARRAY *pArray = NULL;

        value.clear();

        hr = wbemObj->Get(BString(propName).rep(), 0,
                          &wbemObjProp, NULL, NULL);
        if (FAILED(hr))
        {
            LOG_DATA("Failed to obtain value of '%s'", propName);
            return -1;
        }

        vt = wbemObjProp.vt;
        if (!(vt & VT_ARRAY))
        {
            LOG_DATA("%s(): Array flag is not set in "
                     "variant type 0x%hx",
                     __FUNCTION__, wbemObjProp.vt);
            VariantClear(&wbemObjProp);
            return -1;
        }

        vt &= (~VT_ARRAY);
        if (vt != VT_I2 && vt != VT_I4 && vt != VT_INT &&
            vt != VT_UI2 && vt != VT_UI4 && vt != VT_UINT &&
            vt != VT_UI1)
        {
            LOG_DATA("%s(): variant type 0x%hx is not among "
                     "known integer types",
                     __FUNCTION__, vt);
            VariantClear(&wbemObjProp);
            return -1;
        }

        pArray = V_ARRAY(&wbemObjProp);
        dims_number = SafeArrayGetDim(pArray);
        if (dims_number != 1)
        {
            LOG_DATA("%s(): wrong number of array dimensions %d",
                     __FUNCTION__, dims_number);
            VariantClear(&wbemObjProp);
            return -1;
        }

        hr = SafeArrayGetLBound(pArray, 1, &LBound);
        if (FAILED(hr))
        {
            LOG_DATA("%s(): failed to obtain lower bound of dimension, "
                     "rc=%lx", __FUNCTION__, hr);
            VariantClear(&wbemObjProp);
            return -1;
        }

        hr = SafeArrayGetUBound(pArray, 1, &UBound);
        if (FAILED(hr))
        {
            LOG_DATA("%s(): failed to obtain upper bound of dimension, "
                     "rc=%lx", __FUNCTION__, hr);
            VariantClear(&wbemObjProp);
            return -1;
        }

        for (i = LBound; i<= UBound; i++)
        {
            long long int el;

            hr = SafeArrayGetElement(pArray, &i, (void *)&el);
            if (FAILED(hr))
            {
                LOG_DATA("%s(): failed to get %d element, "
                         "rc=%lx", __FUNCTION__, i, hr);
                VariantClear(&wbemObjProp);
                value.clear();
                return -1;
            }

            if (vt == VT_I2)
                value.append(*((SHORT *)&el));
            else if (vt == VT_I4)
                value.append(*((LONG *)&el));
            else if (vt == VT_INT)
                value.append(*((INT *)&el));
            else if (vt == VT_UI2)
                value.append(*((USHORT *)&el));
            else if (vt == VT_UI4)
                value.append(*((ULONG *)&el));
            else if (vt == VT_UINT)
                value.append(*((UINT *)&el));
            else if (vt == VT_UI1)
                value.append(*((BYTE *)&el));
        }

        VariantClear(&wbemObjProp);
        return 0;
    }

    ///
    /// Get boolean property value of WMI object.
    ///
    /// @param wbemObj            WMI object pointer
    /// @param propName           Property name
    /// @param value        [out] Where to save obtained value
    ///
    /// @return 0 on success or error code
    ///
    static int wmiGetBoolProp(IWbemClassObject *wbemObj, const char *propName,
                              bool *value)
    {
        HRESULT  hr;
        VARIANT  wbemObjProp;

        hr = wbemObj->Get(BString(propName).rep(), 0,
                          &wbemObjProp, NULL, NULL);
        if (FAILED(hr))
        {
            LOG_DATA("Failed to obtain value of '%s'", propName);
            return -1;
        }

        if (wbemObjProp.vt == VT_BOOL)
            *value = (wbemObjProp.boolVal ? true : false);
        else
        {
            LOG_DATA("Wrong variant type 0x%hx", wbemObjProp.vt);
            VariantClear(&wbemObjProp);
            return -1;
        }
        VariantClear(&wbemObjProp);

        return 0;
    }

    ///
    /// Enumerate WMI object instances.
    ///
    /// @param className          Object class name
    /// @param instances    [out] Where to save obtained instances
    ///
    /// @return 0 on success or error code
    ///
    static int wmiEnumInstances(const char *className,
                                Array<IWbemClassObject *> &instances)
    {
        HRESULT               hr;
        IEnumWbemClassObject *enumWbemObj = NULL;
        IWbemClassObject     *wbemObj = NULL;
        int                   rc = 0;
        ULONG                 count;
        unsigned int          i;
        BString               bstr(className);

        if (wmiEstablishConn() != 0)
            return -1;

        hr = rootWMIConn->CreateInstanceEnum(bstr.rep(),
                                             WBEM_FLAG_SHALLOW, NULL,
                                             &enumWbemObj);
        if (FAILED(hr))
        {
            LOG_DATA("CreateInstanceEnum() failed, rc = %lx", hr);
            return -1;
        }

        while (1)
        {
            hr = enumWbemObj->Next(WBEM_INFINITE, 1, &wbemObj, &count);
            if (hr != WBEM_S_NO_ERROR && hr != WBEM_S_FALSE)
            {
                LOG_DATA("%s(): IEnumWbemClasObject::Next() "
                         "failed with rc=%lx", __FUNCTION__, hr);
                for (i = 0; i < instances.size(); i++)
                    instances[i]->Release();
                instances.clear();
                rc = -1;
                break;
            }
            if (count == 0)
                break;

            instances.append(wbemObj);
        }

        enumWbemObj->Release();
        return rc;
    }

    ///
    /// Get WMI object by a given path.
    ///
    /// @param objPath            Object path
    /// @param obj          [out] Where to save obtained object pointer
    ///
    /// @return 0 on success or error code
    ///
    static int wmiGetObject(const char *objPath, IWbemClassObject **obj)
    {
        HRESULT hr;

        if (wmiEstablishConn() != 0)
            return -1;

        hr = rootWMIConn->GetObject(BString(objPath).rep(),
                                    WBEM_FLAG_RETURN_WBEM_COMPLETE,
                                    NULL, obj, NULL);
        if (FAILED(hr))
        {
            LOG_DATA("%s(): IWbemClassObject::GetObject(%s) failed with "
                     "rc = %lx", __FUNCTION__, objPath, hr);
            return -1;
        }

        return 0;
    }

    ///
    /// Get WMI path and input parameters object to call a given method
    /// for a given instance.
    ///
    /// @param instance         WMI object instance pointer
    /// @param methodName       Method name
    /// @param objPath    [out] Instance path to be used for calling method
    /// @param pIn        [out] Input parameters object
    ///
    static int wmiPrepareMethodCall(IWbemClassObject *instance,
                                    BString &methodName,
                                    BString &objPath,
                                    IWbemClassObject **pIn)
    {
        HRESULT hr;

        VARIANT className;
        VARIANT path;

        IWbemClassObject *classObj = NULL;
        IWbemClassObject *pInDef = NULL;

        bool className_got = false;
        bool path_got = false;
        bool classObj_got = false;
        bool pInDef_got = false;
        int  rc = 0;

        if (wmiEstablishConn() != 0)
            return -1;

        hr = instance->Get(BString("__Class").rep(), 0,
                           &className, NULL, NULL);
        if (FAILED(hr))
        {
            LOG_DATA("%s(): failed to obtain class name of WMI object, "
                     "rc=%lx", __FUNCTION__, hr);
            return -1;
        }
        className_got = true;

        hr = instance->Get(BString("__Path").rep(), 0,
                           &path, NULL, NULL);
        if (FAILED(hr))
        {
            LOG_DATA("%s(): failed to obtain path of WMI object, "
                     "rc=%lx", __FUNCTION__, hr);
            rc = -1;
            goto cleanup;
        }
        path_got = true;

        hr = rootWMIConn->GetObject(className.bstrVal, 0, NULL,
                                    &classObj, NULL);
        if (FAILED(hr))
        {
            LOG_DATA("%s(): failed to obtain class definition "
                     "of WMI object, rc=%lx", __FUNCTION__, hr);
            rc = -1;
            goto cleanup;
        }
        classObj_got = true;

        hr = classObj->GetMethod(methodName.rep(), 0, &pInDef,
                                 NULL);
        if (FAILED(hr))
        {
            LOG_DATA("%s(): failed to obtain method definition "
                     "for WMI object, rc=%lx", __FUNCTION__, hr);
            rc = -1;
            goto cleanup;
        }
        pInDef_got = true;

        hr = pInDef->SpawnInstance(0, pIn);
        if (FAILED(hr))
        {
            LOG_DATA("%s(): failed to obtain input parameters instance, "
                     "rc=%lx", __FUNCTION__, hr);
            rc = -1;
            goto cleanup;
        }

        objPath.set(path.bstrVal, cimple::BSTR_TAG);
        path.bstrVal = NULL;
cleanup:
        if (className_got)
            VariantClear(&className);
        if (path_got)
            VariantClear(&path);
        if (classObj_got)
            classObj->Release();
        if (pInDef_got)
            pInDef->Release();

        return rc;
    }

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
        BString objPath;
        int     CompletionCode;
        BString methodName("EFX_Port_Firmware_Version_Read");
        VARIANT targetType;
        
        IWbemClassObject *pIn = NULL;
        IWbemClassObject *pOut = NULL;

        if (wmiEstablishConn() != 0)
            return -1;

        rc = wmiPrepareMethodCall(port, methodName, objPath, &pIn);
        if (rc < 0)
            return rc;

        targetType.vt = VT_I4;
        targetType.lVal = REFLASH_TARGET_BOOTROM;
        hr = pIn->Put(BString("TargetType").rep(), 0,
                      &targetType, 0);
        if (FAILED(hr))
        {
            LOG_DATA("%s(): failed to set TargetType for firmrmware "
                     "version reading method, rc=%lx", __FUNCTION__, hr);
            pIn->Release();
            return -1;
        }

        hr = rootWMIConn->ExecMethod(objPath.rep(), methodName.rep(),
                                     0, NULL, pIn, &pOut, NULL);
        if (FAILED(hr))
        {
            LOG_DATA("%s(): failed to call firmware version reading "
                     "method, rc=%lx", __FUNCTION__, hr);
            pIn->Release();
            return -1;
        }

        rc = wmiGetIntProp<int>(pOut, "CompletionCode", &CompletionCode);
        if (rc < 0)
        {
            pIn->Release();
            pOut->Release();
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
        BString objPath;
        int     CompletionCode;
        BString methodName("EFX_Port_ReadVpdKeyword");
        VARIANT propTag;
        VARIANT propKeyword;
        
        IWbemClassObject *pIn = NULL;
        IWbemClassObject *pOut = NULL;

        if (wmiEstablishConn() != 0)
            return -1;

        rc = wmiPrepareMethodCall(port, methodName, objPath, &pIn);
        if (rc < 0)
            return rc;

        propTag.vt = VT_UI1;
        propTag.bVal = tag;
        hr = pIn->Put(BString("Tag").rep(), 0,
                      &propTag, 0);
        if (FAILED(hr))
        {
            LOG_DATA("%s(): failed to set Tag for VPD "
                     "field reading method, rc=%lx", __FUNCTION__, hr);
            pIn->Release();
            return -1;
        }

        propKeyword.vt = VT_I4;
        propKeyword.lVal = keyword;
        hr = pIn->Put(BString("Keyword").rep(), 0,
                      &propKeyword, 0);
        if (FAILED(hr))
        {
            LOG_DATA("%s(): failed to set Keyword for VPD "
                     "field reading method, rc=%lx", __FUNCTION__, hr);
            pIn->Release();
            return -1;
        }

        hr = rootWMIConn->ExecMethod(objPath.rep(), methodName.rep(),
                                     0, NULL, pIn, &pOut, NULL);
        if (FAILED(hr))
        {
            LOG_DATA("%s(): failed to call firmware version reading "
                     "method, rc=%lx", __FUNCTION__, hr);
            pIn->Release();
            return -1;
        }

        rc = wmiGetIntProp<int>(pOut, "CompletionCode", &CompletionCode);
        if (rc < 0)
        {
            pIn->Release();
            pOut->Release();
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
        if (getPortVPDField(port, VPD_TAG_ID, 0,
                            productName) != 0 ||
            getPortVPDField(port, VPD_TAG_R, VPD_KEYWORD('P', 'N'),
                            productNumber) != 0 ||
            getPortVPDField(port, VPD_TAG_R, VPD_KEYWORD('S', 'N'),
                            serialNumber) != 0)
            return -1;

        return 0;
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
            pInfo = (IP_INTERFACE_INFO *)MALLOC(outBufLen);
            if (pInfo == NULL)
            {
                LOG_DATA("Failed to allocate memory "
                         "for GetInterfaceInfo()");
                return -1;
            }
        }

        dwRetVal = GetInterfaceInfo(pInfo, &outBufLen);
        if (dwRetVal != NO_ERROR)
        {
            LOG_DATA("The second call of GetInterfaceInfo() failed, "
                     "dwRetVal=%d", dwRetVal);
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
                LOG_DATA("GetIfEntry(dwIndex=%d) failed with rc=%lu",
                         ulRetVal);
                FREE(pInfo);
                info.clear();
                return -1;
            }

            intfInfo.Name = wstr2str(pInfo->Adapter[i].Name);
            intfInfo.Descr = String("");
            for (j = 0; j < mibIfRow.dwDescrLen; j++)
                intfInfo.Descr.append(mibIfRow.bDescr[j]);
            intfInfo.Index = pInfo->Adapter[i].Index;
            intfInfo.MTU = mibIfRow.dwMtu;
            intfInfo.AdminStatus =  mibIfRow.dwAdminStatus;
            intfInfo.PhysAddr.clear();
            for (j = 0; j < mibIfRow.dwPhysAddrLen; j++)
                intfInfo.PhysAddr.append((int)mibIfRow.bPhysAddr[j]);
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
        bool                      dummy;
        bool                      clearPciInfos = false;
        int                       pci_cmp_rc;

        rc = wmiEnumInstances("EFX_Port", ports);
        if (rc < 0)
            return rc;

        rc = getInterfacesInfo(intfsInfo);
        if (rc < 0)
            return rc;

        for (i = 0; i < ports.size(); i++)
        {
            if (wmiGetBoolProp(ports[i], "DummyInstance", &dummy) != 0 ||
                wmiGetIntProp<int>(ports[i], "Id",
                                   &portDescr.port_id) != 0 ||
                wmiGetStringProp(ports[i], "McfwVersion",
                                 portDescr.mcfwVersion) != 0)
            {
                rc = -1;
                goto cleanup;
            }

            if (dummy)
                continue;

            if (getPortBootROMVersion(ports[i],
                                      portDescr.bootROMVersion) != 0 ||
                wmiGetIntArrProp<int>(ports[i], "PermanentMacAddress",
                                      portDescr.permanentMAC) != 0 ||
                wmiGetIntArrProp<int>(ports[i], "CurrentMacAddress",
                                      portDescr.currentMAC) != 0 ||
                wmiGetBoolProp(ports[i], "LinkUp",
                               &portDescr.linkUp) != 0 ||
                wmiGetIntProp<int>(ports[i], "LinkDuplex",
                                   &portDescr.linkDuplex) != 0 ||
                wmiGetBoolProp(ports[i], "FlowControlAutonegotiation",
                               &portDescr.autoneg) != 0 ||
                wmiGetIntProp<uint64>(ports[i],
                                      "LinkCurrentSpeed",
                                      &portDescr.linkSpeed) != 0 ||
                getPortVPDFields(ports[i],
                                 portDescr.productName,
                                 portDescr.productNumber,
                                 portDescr.serialNumber) != 0)

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
                LOG_DATA("Failed to find network interface information "
                         "for portId=%d", portDescr.port_id);
                rc = -1;
                goto cleanup;
            }

            portDescr.ifInfo = intfsInfo[j];

            if (wmiEnumInstances("EFX_PciInformation", pciInfos) != 0)
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
            if (rc != 0)
                break;
            if (j == pciInfos.size())
            {
                LOG_DATA("Failed to find matching EFX_PciInformation for "
                         "PortId=%d", portDescr.port_id);
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
                LOG_DATA("Failed to obtain all required EFX_PciInformation "
                         "properties for PortId=%d", portDescr.port_id);
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
                portDescrs.insert(j, portDescr);
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
            ports[i]->Release();
        ports.clear();

        return rc;
    }

    class WindowsPort : public Port {
        const NIC *owner;
    public:
        PortDescr portInfo;

        WindowsPort(const NIC *up, unsigned i, PortDescr &descr) : 
            Port(i), 
            owner(up), 
            portInfo(descr) {}
        
        virtual bool linkStatus() const
        {
            return portInfo.linkUp;
        }
        virtual Speed linkSpeed() const
        {
            return speedValue(portInfo.linkSpeed);
        }            
        virtual void linkSpeed(Speed sp) { }
            
        /// @return full-duplex state
        virtual bool fullDuplex() const
        {
            return (portInfo.linkDuplex > 0 ? true : false);
        }
        /// enable or disable full-duplex mode depending on @p fd
        virtual void fullDuplex(bool fd) { };
        /// @return true iff autonegotiation is available
        virtual bool autoneg() const
        {
            return portInfo.autoneg;
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

        virtual void initialize() {};

        // Dummy operator to make possible using of cimple::Array
        inline bool operator== (const WindowsPort &rhs)
        {
            UNUSED(rhs);
            return false;
        }
    };

    class WindowsInterface : public Interface {
        const NIC *owner;
        Port *boundPort;
    public:
        WindowsInterface(const NIC *up, unsigned i) : 
            Interface(i), 
            owner(up), 
            boundPort(NULL)
        { }
        virtual bool ifStatus() const
        {
            PortDescr *portInfo = &((WindowsPort *)boundPort)->portInfo;

            return
              (portInfo->ifInfo.AdminStatus == MIB_IF_ADMIN_STATUS_UP ? 
                                                            true : false);
        }
        virtual void enable(bool st) { }

        /// @return current MTU
        virtual uint64 mtu() const
        {
            PortDescr *portInfo = &((WindowsPort *)boundPort)->portInfo;

            return portInfo->ifInfo.MTU;
        }            
        /// change MTU to @p u
        virtual void mtu(uint64 u) { };
        /// @return system interface name (e.g. ethX for Linux)
        virtual String ifName() const;
        /// @return MAC address actually in use
        virtual MACAddress currentMAC() const
        {
            PortDescr *portInfo = &((WindowsPort *)boundPort)->portInfo;

            return MACAddress(portInfo->currentMAC[0],
                              portInfo->currentMAC[1],
                              portInfo->currentMAC[2],
                              portInfo->currentMAC[3],
                              portInfo->currentMAC[4],
                              portInfo->currentMAC[5]);
        }        
        /// Change the current MAC address to @p mac
        virtual void currentMAC(const MACAddress& mac) { };

        virtual const NIC *nic() const { return owner; }
        virtual PCIAddress pciAddress() const
        {
            return owner->pciAddress().fn(0);
        }

        virtual Port *port() { return boundPort; }
        virtual const Port *port() const { return boundPort; }

        void bindToPort(Port *p) { boundPort = p; }
            
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
        PortDescr *portInfo = &((WindowsPort *)boundPort)->portInfo;

        return portInfo->ifInfo.Name;
    }
    
    class WindowsNICFirmware : public NICFirmware {
        const NIC *owner;
        VersionInfo vers;
    public:
        WindowsNICFirmware(const NIC *o, const VersionInfo &v) :
            owner(o), vers(v) {}
        virtual const NIC *nic() const { return owner; }
        virtual VersionInfo version() const { return vers; }
        virtual bool syncInstall(const char *)
        {
            return true;
        }
        virtual void initialize() {};
    };

    class WindowsBootROM : public BootROM {
        const NIC *owner;
        VersionInfo vers;
    public:
        WindowsBootROM(const NIC *o, const VersionInfo &v) :
            owner(o), vers(v) {}
        virtual const NIC *nic() const { return owner; }
        virtual VersionInfo version() const { return vers; }
        virtual bool syncInstall(const char *) { return true; }
        virtual void initialize() {};
    };

    class WindowsDiagnostic : public Diagnostic {
        const NIC *owner;
        Result testPassed;
        static const char sampleDescr[];
        static const String diagGenName;
    public:
        WindowsDiagnostic(const NIC *o) :
            Diagnostic(sampleDescr), owner(o), testPassed(NotKnown) {}
        virtual Result syncTest() 
        {
            testPassed = Passed;
            log().logStatus("passed");
            return Passed;
        }
        virtual Result result() const { return testPassed; }
        virtual const NIC *nic() const { return owner; }
        virtual void initialize() {};
        virtual const String& genericName() const { return diagGenName; }
    };

    const char WindowsDiagnostic::sampleDescr[] = "Windows Diagnostic";
    const String WindowsDiagnostic::diagGenName = "Diagnostic";

    class WindowsNIC : public NIC {
        WindowsNICFirmware nicFw;
        WindowsBootROM rom;
        WindowsDiagnostic diag;
    public:
        Array<WindowsPort> ports;
        Array<WindowsInterface> intfs;

    protected:
        virtual void setupPorts()
        {
            int i = 0;

            for (i = 0; i < (int)ports.size(); i++)
                ports[i].initialize();
        }
        virtual void setupInterfaces()
        {
            int i = 0;

            for (i = 0; i < (int)ports.size(); i++)
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
        virtual void setupDiagnostics()
        {
            diag.initialize();
        }
    public:
        WindowsNIC(unsigned idx, NICDescr &descr) :
            NIC(idx),
            nicFw(this,
                  VersionInfo(
                      descr.ports.size() > 0 ?
                         descr.ports[0].mcfwVersion.c_str() : "0.0.0")),
            rom(this,
                VersionInfo(
                      descr.ports.size() > 0 ?
                         descr.ports[0].bootROMVersion.c_str() : "0.0.0")),
            diag(this)
        {
            int i = 0;

            for (i = 0; i < (int)descr.ports.size(); i++)
            {
                ports.append(WindowsPort(this, i, descr.ports[i]));
                intfs.append(WindowsInterface(this, i));
            }
        }
        virtual VitalProductData vitalProductData() const 
        {
          
            if (ports.size() > 0)
            {
                const PortDescr *portInfo = &ports[0].portInfo;

                return VitalProductData(portInfo->serialNumber,
                                        "",
                                        portInfo->serialNumber,
                                        portInfo->productNumber,
                                        portInfo->productName,
                                        portInfo->productNumber);
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
            return 9216;
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

            for (i = 0; i < (int)ports.size(); i++)
                if (!en.process(ports[i]))
                    return false;

            return true;
        }
        
        virtual bool forAllPorts(ConstElementEnumerator& en) const
        {
            int i = 0;

            for (i = 0; i < (int)ports.size(); i++)
                if (!en.process(ports[i]))
                    return false;

            return true;
        }

        virtual bool forAllInterfaces(ElementEnumerator& en)
        {
            int i = 0;

            for (i = 0; i < (int)ports.size(); i++)
            {
                if (!en.process(intfs[i]))
                    return false;
            }

            return true;
        }
        
        virtual bool forAllInterfaces(ConstElementEnumerator& en) const
        {
            int i = 0;

            for (i = 0; i < (int)ports.size(); i++)
            {
                if (!en.process(intfs[i]))
                    return false;
            }

            return true;
        }

        virtual bool forAllDiagnostics(ElementEnumerator& en)
        {
            return en.process(diag);
        }
        
        virtual bool forAllDiagnostics(ConstElementEnumerator& en) const
        {
            return en.process(diag);
        }

        // I failed to find PCI domain on Windows; instead there are
        // separate bus, device and function numbers for PCI bridge.
        // Anyway, we do not use PCI data for any other purpose that for
        // distinguishing ports belonging to different NICs.
        virtual PCIAddress pciAddress() const { return PCIAddress(0, 0, 0); }
    };


    class WindowsDriver : public Driver {
        const Package *owner;
        static const String drvName;
    public:
        WindowsDriver(const Package *pkg, const String& d,
                      const String& sn) :
            Driver(d, sn), owner(pkg) { }
        virtual VersionInfo version() const
        {
            /// Version cannot be obtained from class constructor -
            /// it is called when regsvr32 tries to register our DLL
            /// and trying to use WMI from this context result in
            /// hanging up or crash.

            Array<IWbemClassObject *> roots;
            int                       rc = 0;
            unsigned int              i;
            bool                      dummy;
            String                    version;
            VersionInfo               vers = VersionInfo("0.0.0");
        
            rc = wmiEnumInstances("EFX_Root", roots);
            if (rc < 0)
                return vers;

            for (i = 0; i < roots.size(); i++)
            {
                if (wmiGetBoolProp(roots[i], "DummyInstance", &dummy) != 0)
                    goto cleanup;

                if (dummy)
                    continue;

                if (wmiGetStringProp(roots[i], "DriverBuild", version) != 0)
                    goto cleanup;

                if (version.size() > 0 && version[0] == 'v')
                    version.remove(0, 1);
                vers = VersionInfo(version.c_str());
                break;
            }

cleanup:
            for (i = 0; i < roots.size(); i++)
                roots[i]->Release();
            roots.clear();  

            return vers;
        }
        virtual void initialize() {};
        virtual bool syncInstall(const char *) { return false; }
        virtual const String& genericName() const { return description(); }
        virtual const Package *package() const { return owner; }
    };


    class WindowsLibrary : public Library {
        const Package *owner;
        VersionInfo vers;
    public:
        WindowsLibrary(const Package *pkg, const String& d, const String& sn, 
                     const VersionInfo& v) :
            Library(d, sn), owner(pkg), vers(v) {}
        virtual VersionInfo version() const { return vers; }
        virtual void initialize() {};
        virtual bool syncInstall(const char *) { return false; }
        virtual const String& genericName() const { return description(); }
        virtual const Package *package() const { return owner; }
    };


    /// @brief stub-only implementation of a software package
    /// with kernel drivers
    class WindowsKernelPackage : public Package {
        WindowsDriver kernelDriver;
    protected:
        virtual void setupContents() { kernelDriver.initialize(); };
    public:
        WindowsKernelPackage() :
            Package("NET Driver RPM", "sfc"),
            kernelDriver(this, "NET Driver", "sfc") {}
        virtual PkgType type() const { return RPM; }
        virtual VersionInfo version() const { return VersionInfo("3.3"); }
        virtual bool syncInstall(const char *) { return true; }
        virtual bool forAllSoftware(ElementEnumerator& en)
        {
            return en.process(kernelDriver);
        }
        virtual bool forAllSoftware(ConstElementEnumerator& en) const 
        {
            return en.process(kernelDriver);
        }
        virtual const String& genericName() const { return description(); }
    };

    /// @brief stub-only implementation of a software package
    /// with provider library
    class WindowsManagementPackage : public Package {
        WindowsLibrary providerLibrary;
    protected:
        virtual void setupContents() { providerLibrary.initialize(); };
    public:
        WindowsManagementPackage() :
            Package("CIM Provider RPM", "sfcprovider"),
            providerLibrary(this, "CIM Provider library", "libSolarflare.so", "0.1") {}
        virtual PkgType type() const { return RPM; }
        virtual VersionInfo version() const { return VersionInfo("0.1"); }
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
        WindowsKernelPackage kernelPackage;
        WindowsManagementPackage mgmtPackage;
        WindowsSystem() {};
        ~WindowsSystem();
    protected:
        void setupNICs()
        {
            // Do nothing
        };

        void setupPackages()
        {
            kernelPackage.initialize();
            mgmtPackage.initialize();
        };
    public:
        static WindowsSystem target;
        bool is64bit() const { return true; }
        OSType osType() const { return RHEL; }
        bool forAllNICs(ConstElementEnumerator& en) const;
        bool forAllNICs(ElementEnumerator& en);
        bool forAllPackages(ConstElementEnumerator& en) const;
        bool forAllPackages(ElementEnumerator& en);
    };

    WindowsSystem::~WindowsSystem()
    {
        wmiReleaseConn();
    }

    bool WindowsSystem::forAllNICs(ConstElementEnumerator& en) const
    {
        NICDescrs   nics;
        int         i;
        bool        res;
        bool        ret = true;
        int         rc;

        if ((rc = getNICs(nics)) < 0)
        {
            LOG_DATA("Failed to obtain info about NICs");
            return false;
        }

        for (i = 0; i < (int)nics.size(); i++)
        {
            WindowsNIC  nic(i, nics[i]);

            nic.initialize();
            res = en.process(nic);
            ret = ret && res;
        }

        return ret;
    }

    bool WindowsSystem::forAllNICs(ElementEnumerator& en)
    {
        return forAllNICs((ConstElementEnumerator&) en); 
    }
    
    bool WindowsSystem::forAllPackages(ConstElementEnumerator& en) const
    {
        if (!en.process(kernelPackage))
            return false;
        return en.process(mgmtPackage);
    }
    
    bool WindowsSystem::forAllPackages(ElementEnumerator& en)
    {
        if (!en.process(kernelPackage))
            return false;
        return en.process(mgmtPackage);
    }

    WindowsSystem WindowsSystem::target;

    System& System::target = WindowsSystem::target;
}
