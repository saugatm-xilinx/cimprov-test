#include "sf_platform.h"
#include <cimple/Buffer.h>
#include <cimple/Strings.h>

#if defined(linux)
#include <unistd.h>
#endif

namespace solarflare 
{
    class SamplePort : public Port {
        const NIC *owner;
        Speed speed;
        bool duplex;
        bool automode;
    public:
        SamplePort(const NIC *up, unsigned i) : 
            Port(i), 
            owner(up), 
            speed(up->maxLinkSpeed()),
            duplex(true), automode(true) {}
        
        virtual bool linkStatus() const { return true; }
        virtual Speed linkSpeed() const { return speed; }            
        virtual void linkSpeed(Speed sp) { speed = sp; }
            
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

        /// @return Manufacturer-supplied MAC address
        virtual MACAddress permanentMAC() const { return MACAddress(0, 1, 2, 3, 4, 5); };

        virtual const NIC *nic() const { return owner; }
        virtual PCIAddress pciAddress() const
        {
            return owner->pciAddress().fn(elementId());
        }

        virtual void initialize() {};
    };

    class SampleInterface : public Interface {
        const NIC *owner;
        bool status;
        uint64 currentMTU;
        MACAddress current;
        Port *boundPort;
    public:
        SampleInterface(const NIC *up, unsigned i) : 
            Interface(i), 
            owner(up), 
            status(false), 
            currentMTU(up->supportedMTU()),
            current(0, 1, 2, 3, 4, 5),
            boundPort(NULL)
        { current.address[5] += i; }
        virtual bool ifStatus() const { return status; }
        virtual void enable(bool st) { status = st; }

        /// @return current MTU
        virtual uint64 mtu() const { return currentMTU; }            
        /// change MTU to @p u
        virtual void mtu(uint64 u) { currentMTU = u; };
        /// @return system interface name (e.g. ethX for Linux)
        virtual String ifName() const;
        /// @return MAC address actually in use
        virtual MACAddress currentMAC() const { return current; }        
        /// Change the current MAC address to @p mac
        virtual void currentMAC(const MACAddress& mac) { current = mac; };

        virtual const NIC *nic() const { return owner; }
        virtual PCIAddress pciAddress() const
        {
            return owner->pciAddress().fn(elementId());
        }

        virtual Port *port() { return boundPort; }
        virtual const Port *port() const { return boundPort; }

        void bindToPort(Port *p) { boundPort = p; }
            
        virtual void initialize() {};
    };


    String SampleInterface::ifName() const
    {
        char buf[] = "eth1";
        buf[sizeof(buf) - 2] += elementId();
        return buf;
    }

    
    class SampleNICFirmware : public NICFirmware {
        const NIC *owner;
        VersionInfo vers;
    public:
        SampleNICFirmware(const NIC *o, const VersionInfo &v) :
            owner(o), vers(v) {}
        virtual const NIC *nic() const { return owner; }
        virtual VersionInfo version() const { return vers; }
        virtual bool syncInstall(const char *)
        {
            return true;
        }
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
        virtual bool syncInstall(const char *) { return true; }
        virtual void initialize() {};
    };

    class SampleDiagnostic : public Diagnostic {
        const NIC *owner;
        Result testPassed;
        static const char sampleDescr[];
        static const String diagGenName;
    public:
        SampleDiagnostic(const NIC *o) :
            Diagnostic(sampleDescr), owner(o), testPassed(NotKnown) {}
        virtual Result syncTest() 
        {
            testPassed = Passed;
            return Passed;
        }
        virtual Result result() const { return testPassed; }
        virtual const NIC *nic() const { return owner; }
        virtual void initialize() {};
        virtual const String& genericName() const { return diagGenName; }
    };

    const char SampleDiagnostic::sampleDescr[] = "Sample Diagnostic";
    const String SampleDiagnostic::diagGenName = "Diagnostic";

    class SampleNIC : public NIC {
        SamplePort port0;
        SamplePort port1;
        SampleInterface intf0;
        SampleInterface intf1;
        SampleNICFirmware nicFw;
        SampleBootROM rom;
        SampleDiagnostic diag;
    protected:
        virtual void setupPorts()
        {
            port0.initialize();
            port1.initialize();
        }
        virtual void setupInterfaces()
        {
            intf0.initialize();
            intf1.initialize();
            intf0.bindToPort(&port0);
            intf1.bindToPort(&port1);
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
        SampleNIC(unsigned idx) :
            NIC(idx),
            port0(this, 0), port1(this, 1),
            intf0(this, 0), intf1(this, 1),
            nicFw(this, VersionInfo("1.2.3")),
            rom(this, VersionInfo("2.3.4")),
            diag(this)
        {}
        virtual VitalProductData vitalProductData() const 
        {
            return VitalProductData("12345667",
                                    "", "1111111", "2222222",
                                    "SFC00000", "333333");
        }
        Connector connector() const { return RJ45; }
        uint64 supportedMTU() const { return 9000; }
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

        virtual bool forAllInterfaces(InterfaceEnumerator& en)
        {
            if(!en.process(intf0))
                return false;
            return en.process(intf1);
        }
        
        virtual bool forAllInterfaces(ConstInterfaceEnumerator& en) const
        {
            if(!en.process(intf0))
                return false;
            return en.process(intf1);
        }
        virtual bool forAllDiagnostics(DiagnosticEnumerator& en)
        {
            return en.process(diag);
        }
        
        virtual bool forAllDiagnostics(ConstDiagnosticEnumerator& en) const
        {
            return en.process(diag);
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
        virtual bool syncInstall(const char *) { return false; }
        virtual const String& genericName() const { return description(); }
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
        virtual bool syncInstall(const char *) { return false; }
        virtual const String& genericName() const { return description(); }
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
        virtual bool syncInstall(const char *) { return true; }
        virtual bool forAllSoftware(SoftwareEnumerator& en)
        {
            return en.process(kernelDriver);
        }
        virtual bool forAllSoftware(ConstSoftwareEnumerator& en) const 
        {
            return en.process(kernelDriver);
        }
        virtual const String& genericName() const { return description(); }
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
        virtual bool syncInstall(const char *) { return true; }
        virtual bool forAllSoftware(SoftwareEnumerator& en)
        {
            return en.process(providerLibrary);
        }
        virtual bool forAllSoftware(ConstSoftwareEnumerator& en) const 
        {
            return en.process(providerLibrary);
        }
        virtual const String& genericName() const { return description(); }
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
