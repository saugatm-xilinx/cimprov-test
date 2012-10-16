#include "SF_Sysinfo.h"
#include <cimple/Buffer.h>
#include <cimple/Strings.h>

#if defined(linux)
#include <unistd.h>
#endif

namespace solarflare 
{
    using cimple::Log_Call_Frame;
    using cimple::_log_enabled_state;
    using cimple::LL_DBG;

    const unsigned VersionInfo::unknown = unsigned(-1);

    VersionInfo::VersionInfo(const char *s) :
        vmajor(0), vminor(0), revisionNo(unknown), buildNo(unknown)
    {
        char *s0 = const_cast<char *>(s);
        vmajor = strtoul(s0, &s0, 10);
        if (*s0 == '.')
        {
            vminor = strtoul(s0 + 1, &s0, 10);
            if (*s0 == '.' || *s0 == '-')
            {
                revisionNo = strtoul(s0 + 1, &s0, 10);
                if (*s0 == '.' || *s0 == '-')
                    buildNo = strtoul(s0 + 1, NULL, 10);
            }
        }
        versionStr = String(s);
    }
    

    String VersionInfo::string() const
    {
        if (versionStr.size() > 0)
            return versionStr;
        
        Buffer result;
        
        result.append_uint32(vmajor);
        result.append('.');
        result.append_uint32(vminor);
        if (revisionNo != unknown)
        {
            result.append('.');
            result.append_uint32(revisionNo);
        }
        if (buildNo != unknown)
        {
            result.append('.');
            result.append_uint32(buildNo);
        }
        return result.data();
    }

    const unsigned PCIAddress::unknown = unsigned(-1);

    const String Port::portName("Ethernet Port");
    const String Port::portDescription("NIC Ethernet Port");

    const String NICFirmware::fwName("Firmware");
    const String NICFirmware::fwDescription("NIC MC Firmware");
    const String NICFirmware::fwSysname("");

    const String BootROM::romName("BootROM");
    const String BootROM::romDescription("NIC BootROM");
    const String BootROM::romSysname("");

    const String& VitalProductData::manufacturer() const 
    {
        return (manufac.size() > 0 ? manufac : 
                System::target.manufacturer());
    }

    const String NIC::nicDescription = "Solarflare NIC";
    const String NIC::nicName = "Ethernet Adapter";

    const String System::manfId = "Solarflare Inc.";
    const String System::systemDescr = "Solarflare-enabled host";
    const String System::systemName = "System";

    class NICPortEnumerator : public NICEnumerator {
        PortEnumerator& en;
    public:
        NICPortEnumerator(PortEnumerator& e) : en(e) {}
        virtual bool process(NIC& n) 
        {
            return n.forAllPorts(en);
        }
    };

    class ConstNICPortEnumerator : public ConstNICEnumerator {
        ConstPortEnumerator& en;
    public:
        ConstNICPortEnumerator(ConstPortEnumerator& e) : en(e) {}
        virtual bool process(const NIC& n) 
        {
            return n.forAllPorts(en);
        }
    };


    class PackageContentsEnumerator : public SoftwareEnumerator {
        SoftwareEnumerator& en;
    public:
        PackageContentsEnumerator(SoftwareEnumerator& e) : en(e) {}
        virtual bool process(SWElement& se) 
        {
            return static_cast<Package&>(se).forAllSoftware(en);
        }
    };

    class ConstPackageContentsEnumerator : public ConstSoftwareEnumerator {
        ConstSoftwareEnumerator& en;
    public:
        ConstPackageContentsEnumerator(ConstSoftwareEnumerator& e) : en(e) {}
        virtual bool process(const SWElement& se) 
        {
            return static_cast<const Package&>(se).forAllSoftware(en);
        }
    };

    class NICFwEnumerator : public NICEnumerator {
        SoftwareEnumerator& en;
    public:
        NICFwEnumerator(SoftwareEnumerator& e) : en(e) {}
        virtual bool process(NIC& n) 
        {
            return n.forAllFw(en);
        }
    };

    class ConstNICFwEnumerator : public ConstNICEnumerator {
        ConstSoftwareEnumerator& en;
    public:
        ConstNICFwEnumerator(ConstSoftwareEnumerator& e) : en(e) {}
        virtual bool process(const NIC& n) 
        {
            return n.forAllFw(en);
        }
    };



    bool System::forAllPorts(ConstPortEnumerator& en) const
    {
        ConstNICPortEnumerator embed(en);
        return forAllNICs(embed);
    }

    bool System::forAllPorts(PortEnumerator& en)
    {
        NICPortEnumerator embed(en);
        return forAllNICs(embed);
    }

    bool System::forAllSoftware(ConstSoftwareEnumerator& en) const
    {
        ConstPackageContentsEnumerator embed(en);
        if (!forAllPackages(embed))
            return false;
        
        ConstNICFwEnumerator embedfw(en);
        if (!forAllNICs(embedfw))
            return false;

        return true;
    }

    bool System::forAllSoftware(SoftwareEnumerator& en)
    {
        PackageContentsEnumerator embed(en);
        if (!forAllPackages(embed))
            return false;
        
        NICFwEnumerator embedfw(en);
        if (!forAllNICs(embedfw))
            return false;
        return true;
    }

    class SamplePort : public Port {
        const NIC *owner;
        bool status;
        uint64 speed;
        bool duplex;
        bool automode;
        uint64 currentMtu;
        MACAddress current;
    public:
        SamplePort(const NIC *up, unsigned i) : 
            Port(i), 
            owner(up), 
            status(false), 
            speed(up->maxLinkSpeed()),
            duplex(true), automode(true),
            currentMtu(up->supportedMtu()),
            current(0, 1, 2, 3, 4, 5)
        { current.address[5] += i; }
        virtual bool linkStatus() const { return status; }
        virtual void enable(bool st) { status = st; }
        virtual uint64 linkSpeed() const { return speed; }            
        virtual void linkSpeed(uint64 sp) { speed = sp; }
            
        /// @return full-duplex state
        virtual bool fullDuplex() const { return duplex; }
        /// enable or disable full-duplex mode depending on @p fd
        virtual void fullDuplex(bool fd) { duplex = fd; };
        /// @return true iff autonegotiation is available
        virtual bool autoneg() const { return automode; };
        /// enable/disable autonegotiation according to @p an
        virtual void autoneg(bool an) { automode = an; }
        
        /// causes a renegotiation like 'ethtool -r'
        virtual void renegotiate() {};
        /// @return current MTU
        virtual uint64 mtu() const { return currentMtu; }            
        /// change MTU to @p u
        virtual void mtu(uint64 u) { currentMtu = u; };
        /// @return system interface name (e.g. ethX for Linux)
        virtual String ifName() const;
        /// @return Manufacturer-supplied MAC address
        virtual MACAddress permanentMAC() const;
        /// @return MAC address actually in use
        virtual MACAddress currentMAC() const { return current; }        
        /// Change the current MAC address to @p mac
        virtual void currentMAC(const MACAddress& mac) { current = mac; };

        virtual const NIC *nic() const { return owner; }
        virtual PCIAddress pciAddress() const
        {
            return owner->pciAddress().fn(elementId());
        }

        virtual void initialize() {};
    };


    class SampleNICFirmware : public NICFirmware {
        const NIC *owner;
        VersionInfo vers;
    public:
        SampleNICFirmware(const NIC *o, const VersionInfo &v) :
            owner(o), vers(v) {}
        virtual const NIC *nic() const { return owner; }
        virtual VersionInfo version() const { return vers; }
        virtual bool install(const char *, bool) { return true; }
        virtual void initialize() {};
    };

    class SampleBootROM : public BootROM {
        const NIC *owner;
        VersionInfo vers;
    public:
        SampleBootROM(const NIC *o, const VersionInfo &v) :
            owner(o), vers(v) {}
        virtual const NIC *nic() const { return owner; }
        virtual VersionInfo version() const { return vers; }
        virtual bool install(const char *, bool) { return true; }
        virtual void initialize() {};
    };

    class SampleNIC : public NIC {
        SamplePort port0;
        SamplePort port1;
        SampleNICFirmware nicFw;
        SampleBootROM rom;
    protected:
        virtual void setupPorts()
        {
            port0.initialize();
            port1.initialize();
        }
        virtual void setupFirmware()
        {
            nicFw.initialize();
            rom.initialize();
        }
    public:
        SampleNIC(unsigned idx) :
            NIC(idx),
            port0(this, 0), port1(this, 1),
            nicFw(this, VersionInfo("1.2.3")),
            rom(this, VersionInfo("2.3.4"))
        {}
        virtual VitalProductData vitalProductData() const 
        {
            return VitalProductData("12345667",
                                    "", "1111111", "2222222",
                                    "SFC00000", "333333");
        }
        Connector connector() const { return RJ45; }
        uint64 supportedMtu() const { return 9000; }
        virtual bool forAllFw(SoftwareEnumerator& en)
        {
            if(!en.process(nicFw))
                return false;
            return en.process(rom);
        }
        virtual bool forAllFw(ConstSoftwareEnumerator& en) const
        {
            if(!en.process(nicFw))
                return false;
            return en.process(rom);
        }
        virtual bool forAllPorts(PortEnumerator& en)
        {
            if(!en.process(port0))
                return false;
            return en.process(port1);
        }
        
        virtual bool forAllPorts(ConstPortEnumerator& en) const
        {
            if(!en.process(port0))
                return false;
            return en.process(port1);
        }

        virtual PCIAddress pciAddress() const { return PCIAddress(0, 1, 2); }
    };


    class SampleDriver : public Driver {
        const Package *owner;
        VersionInfo vers;
        static const String drvName;
    public:
        SampleDriver(const Package *pkg, const String& d, const String& sn, 
                     const VersionInfo& v) :
            Driver(d, sn), owner(pkg), vers(v) {}
        virtual VersionInfo version() const { return vers; }
        virtual void initialize() {};
        virtual bool install(const char *, bool) { return false; }
        virtual const String& className() const { return description(); }
        virtual const Package *package() const { return owner; }
    };


    class SampleLibrary : public Library {
        const Package *owner;
        VersionInfo vers;
    public:
        SampleLibrary(const Package *pkg, const String& d, const String& sn, 
                     const VersionInfo& v) :
            Library(d, sn), owner(pkg), vers(v) {}
        virtual VersionInfo version() const { return vers; }
        virtual void initialize() {};
        virtual bool install(const char *, bool) { return false; }
        virtual const String& className() const { return description(); }
        virtual const Package *package() const { return owner; }
    };


    /// @brief stub-only implementation of a software package
    /// with kernel drivers
    class SampleKernelPackage : public Package {
        SampleDriver kernelDriver;
    protected:
        virtual void setupContents() { kernelDriver.initialize(); };
    public:
        SampleKernelPackage() :
            Package("NET Driver RPM", "sfc"),
            kernelDriver(this, "NET Driver", "sfc", "3.3") {}
        virtual PkgType type() const { return RPM; }
        virtual VersionInfo version() const { return VersionInfo("3.3"); }
        virtual bool install(const char *, bool) { return true; }
        virtual bool forAllSoftware(SoftwareEnumerator& en)
        {
            return en.process(kernelDriver);
        }
        virtual bool forAllSoftware(ConstSoftwareEnumerator& en) const 
        {
            return en.process(kernelDriver);
        }
        virtual const String& className() const { return description(); }
    };

    /// @brief stub-only implementation of a software package
    /// with provider library
    class SampleManagementPackage : public Package {
        SampleLibrary providerLibrary;
    protected:
        virtual void setupContents() { providerLibrary.initialize(); };
    public:
        SampleManagementPackage() :
            Package("CIM Provider RPM", "sfcprovider"),
            providerLibrary(this, "CIM Provider library", "libSolarflare.so", "0.1") {}
        virtual PkgType type() const { return RPM; }
        virtual VersionInfo version() const { return VersionInfo("0.1"); }
        virtual bool install(const char *, bool) { return true; }
        virtual bool forAllSoftware(SoftwareEnumerator& en)
        {
            return en.process(providerLibrary);
        }
        virtual bool forAllSoftware(ConstSoftwareEnumerator& en) const 
        {
            return en.process(providerLibrary);
        }
        virtual const String& className() const { return description(); }
    };

    /// @brief stub-only System implementation
    /// @note all structures are initialised statically,
    /// so initialize() does nothing 
    class SampleSystem : public System {
        SampleNIC nic;
        SampleKernelPackage kernelPackage;
        SampleManagementPackage mgmtPackage;
        SampleSystem() : nic(0) {};
    protected:
        void setupNICs() { nic.initialize(); };
        void setupPackages()
        {
            kernelPackage.initialize();
            mgmtPackage.initialize();
        };
    public:
        static SampleSystem target;
        bool is64bit() const { return true; }
        OSType osType() const { return RHEL; }
        bool forAllNICs(ConstNICEnumerator& en) const;
        bool forAllNICs(NICEnumerator& en);
        bool forAllPackages(ConstSoftwareEnumerator& en) const;
        bool forAllPackages(SoftwareEnumerator& en);
    };

    bool SampleSystem::forAllNICs(ConstNICEnumerator& en) const
    {
        return en.process(nic);
    }

    bool SampleSystem::forAllNICs(NICEnumerator& en)
    {
        return en.process(nic);
    }
    
    bool SampleSystem::forAllPackages(ConstSoftwareEnumerator& en) const
    {
        if (!en.process(kernelPackage))
            return false;
        return en.process(mgmtPackage);
    }
    
    bool SampleSystem::forAllPackages(SoftwareEnumerator& en)
    {
        if (!en.process(kernelPackage))
            return false;
        return en.process(mgmtPackage);
    }

    SampleSystem SampleSystem::target;

    System& System::target = SampleSystem::target;
}
