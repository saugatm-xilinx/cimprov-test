#ifndef SOLARFLARE_SF_SYSINFO_H
#define SOLARFLARE_SF_SYSINFO_H 1

#include <cimple/cimple.h>
#include <cimple/Buffer.h>

/// @brief Namespace for system abstraction classes
namespace solarflare 
{
    using cimple::Buffer;
    using cimple::String;
    using cimple::Datetime;
    using cimple::uint64;
    using cimple::Mutex;

    /// @brief The root class for all managed objects
    class SystemElement {
        String descr;
     public:
        SystemElement(const String& d) :
            descr(d) {}
        virtual ~SystemElement() {}
            
        const String& description() const { return descr; }
        virtual unsigned elementId() const { return 0; }
        /// Run-time initialization for things that are unsafe
        /// to do from constructors (including calling virtual methods).
        /// CIM provider will call these method for topmost System object
        /// and the later will create other objects and create this method
        /// for them.
        virtual void initialize() = 0;
        /// @return a generic name of an entity (e.g. 'Ethernet Adaptor')
        virtual const String& className() const = 0;
    };

    /// @brief Detailed version info for SW elements
    class VersionInfo {
        unsigned vmajor; //< Major version
        unsigned vminor; //< Minor version
        unsigned revisionNo; //< Revision number
        unsigned buildNo; //< Build number
        String versionStr; //< Version string
        Datetime dateReleased; //< Release date
    public:
        /// A value for missing version component
        static const unsigned unknown;
        VersionInfo(unsigned mj, unsigned mn, 
                    unsigned rev = unknown, 
                    unsigned bno = unknown,
                    const String& vstr = "", const Datetime& rd = Datetime()) :
            vmajor(mj), vminor(mn), revisionNo(rev), buildNo(bno),
            versionStr(vstr), 
            dateReleased(rd) {}
        VersionInfo() :
            vmajor(unknown), vminor(unknown), revisionNo(unknown), 
            buildNo(unknown) {}
        /// Construct version data by parsing a version string.
        /// It must have the form 'major.minor[.revision[.build]] rest'.
        /// Missing fields (incl. release date) are set to unknown.
        /// The versionStr field itself is set to @p s
        VersionInfo(const char *s);
        bool operator == (const VersionInfo& v) const;
        bool operator != (const VersionInfo& v) const
        {
            return !(*this == v);
        }
        unsigned major() const { return vmajor; }
        unsigned minor() const { return vminor; }
        unsigned revision() const { return revisionNo; }
        unsigned build() const{ return buildNo; }
        /// @return String rep of the version info
        /// If versionStr is non-empty, it is simply returned, otherwise
        /// the returned string is 'major.minor[.revision[.build]]'
        String string() const;
        Datetime releaseDate() const {  return dateReleased; }
        /// Test whether any field has a 'known' value
        bool isUnknown() const;
    };

    inline bool VersionInfo::operator == (const VersionInfo& v) const
    {
        return (vmajor == v.vmajor &&
                vminor == v.vminor &&
                revisionNo == v.revisionNo &&
                buildNo == v.buildNo &&
                versionStr == v.versionStr &&
                dateReleased == v.dateReleased);
    }

    inline bool VersionInfo::isUnknown() const
    {
        return (vmajor == unknown &&
                vminor == unknown &&
                revisionNo == unknown &&
                buildNo == unknown &&
                versionStr.size() == 0 &&
                dateReleased == Datetime());
    }

    /// @brief Thread abstraction
    /// Unlike CIMPLE's own class, this class shall be
    /// subclassed, as the thread encapsulates its behaviour
    /// via threadProc() method.
    /// The class also supports querying thread state and 
    /// stopping the thread
    class Thread : public cimple::Thread {
    public:
        enum State {
            NotRun,
            Running,
            Succeeded,
            Failed,
            Aborted
        };
    private:
        Mutex stateLock;
        State state;
        /// An adaptor between cimple::Thread interface and threadProc()
        static void *doThread(void *self);
    protected:
        /// The actual thread routine
        /// @return true for successful termination (state := Succeeded) 
        ///         false for failure (state := failed)
        virtual bool threadProc() = 0;
    public:
        Thread() : state(NotRun) {}
        State currentState() const;
        /// Makes the thread running
        void start();
        /// Attempts to stop the thread.
        /// It is not guaranteed that this method actually has any effect;
        /// caller must poll currentState() for state change.
        /// As there is no cross-platform universal way to stop threads,
        /// this needs co-operation from threadProc(), so the method shall
        /// be overridden in subclasses
        virtual void stop() {};
    };

    /// @brief Abstract class for software components
    class SWElement : public SystemElement {
        String sysname;
    public:
        SWElement(const String& d, const String& sn) :
            SystemElement(d), sysname(sn) {}
        /// @return version of the installed component
        /// Platform-specific subclasses shall implement the actual behaviour
        virtual VersionInfo version() const = 0;
        /// Updates a software component from @p filename.
        /// Platform-specific subclasses shall implement the actual behaviour
        /// If @p sync is false, a separate thread shall be created and true
        /// be returned. The corresponding thread may be then obtained by 
        /// calling installThread()
        virtual bool install(const char *filename, bool sync = true) = 0;
        const String& sysName() const { return sysname; }
        enum SWClass {
            SWDriver,
            SWTool,
            SWLibrary,
            SWPackage,
            SWFirmware,
            SWDiagnostics
        };
        virtual SWClass classify() const = 0;
        /// @return true iff the software element is an actual host-running
        /// software (i.e. a driver, a library or a tool, but not firmware
        /// or a software package)
        /// Effectively, if isHostSw() is true, the object may be safely
        /// cast to HostSWElement
        virtual bool isHostSw() const = 0;
        /// @return an installation asynchronous thread
        /// If no thread is currently running, the implementation is
        /// allowed to return NULL or an inactive Thread object.
        /// The returned object is logically owned by the object, so the
        /// caller must never delete the thread, nor use it after the object
        /// is deleted
        virtual Thread *installThread() { return NULL; }
    };

    /// @brief PCI address
    /// If deviceId is set to a known value, the address
    /// is a conventional PCI address; otherwise it is assumed
    /// to be an ARI-style address.
    /// fnId is set to unknown for slot addresses
    class PCIAddress {
        unsigned pciDomain;
        unsigned pciBus;
        unsigned deviceId;
        unsigned fnId;
    public:
        static const unsigned unknown;
        PCIAddress(unsigned dom, unsigned b, 
                   unsigned dev = unknown, unsigned fn = unknown) :
            pciDomain(dom), pciBus(b), deviceId(dev), fnId(fn) {}
        unsigned domain() const { return pciDomain; }
        unsigned bus() const { return pciBus; }
        unsigned device() const { return deviceId; }
        unsigned fn() const { return fnId; }
        bool isARI() const { return deviceId == unknown && fnId != unknown; }
        bool isSlot() const { return fnId == unknown; }
        /// Return a new  PCI address with a function id set to @p f
        /// Useful to construct device PCI address from slot address
        PCIAddress fn(unsigned f) const
        {
            return PCIAddress(pciDomain, pciBus, deviceId, f);
        }
        
    };

    /// @brief Abstract class for hardware components (currently, NICs and Ports)
    /// A hardware element always has an ordinal number inside its parent
    /// object, and a PCI bus address
    class HWElement : public SystemElement {
        unsigned idx;
    public:
        HWElement(const String& d, unsigned i) :
            SystemElement(d), idx(i) {}
        virtual unsigned elementId() const { return idx; }
        virtual PCIAddress pciAddress() const = 0;
    };

    struct MACAddress {
        unsigned char address[6];
        /// We need this because C++ (prior to 2011 edition)
        /// does not allow compound initializers in many contexts
        MACAddress(unsigned a0, unsigned a1, unsigned a2,
                   unsigned a3, unsigned a4, unsigned a5);
    };

    MACAddress::MACAddress(unsigned a0, unsigned a1, unsigned a2,
                           unsigned a3, unsigned a4, unsigned a5)
    {
        address[0] = a0;
        address[1] = a1;
        address[2] = a2;
        address[3] = a3;
        address[4] = a4;
        address[5] = a5;
    }

    class NIC;

    /// @brief Abstract mix-in for NIC associated elements (ports and firmware)
    class NICElement {
    public:
        virtual const NIC *nic() const = 0;
    };

    /// @brief Abstract class for ports
    /// Implementors shall subclass it for platform-specific port representation
    class Port : public HWElement, public NICElement {
        static const String portName;
        static const String portDescription;
    public:
        Port(unsigned i) : HWElement(portDescription, i) {}
        /// @return link status
        virtual bool linkStatus() const = 0;
        /// @return changing the interface state up/down
        virtual void enable(bool status) = 0;
        /// @return current link speed
        virtual uint64 linkSpeed() const = 0;
        /// @return change link speed to @p speed
        virtual void linkSpeed(uint64 speed) = 0;
        /// @return full-duplex state
        virtual bool fullDuplex() const = 0;
        /// enable or disable full-duplex mode depending on @p fd
        virtual void fullDuplex(bool fd) = 0;
        /// @return true iff autonegotiation is available
        virtual bool autoneg() const = 0;
        /// enable/disable autonegotiation according to @p an
        virtual void autoneg(bool an) = 0;
        /// causes a renegotiation like 'ethtool -r'
        virtual void renegotiate() = 0;
        /// @return current MTU
        virtual uint64 mtu() const = 0;
        /// change MTU to @p u
        virtual void mtu(uint64 u) = 0;
        /// @return system interface name (e.g. ethX for Linux)
        virtual String ifName() const = 0;
        /// @return Manufacturer-supplied MAC address
        virtual MACAddress permanentMAC() const = 0;
        /// @return MAC address actually in use
        virtual MACAddress currentMAC() const = 0;
        /// Change the current MAC address to @p mac
        virtual void currentMAC(const MACAddress& mac) = 0;
        virtual const String& className() const { return portName; }
    };

    /// @brief Abstract class for firmware elements
    class Firmware : public SWElement, public NICElement {
    public:
        Firmware(const String& d, const String& sn) : SWElement(d, sn) {};
        virtual SWClass classify() const { return SWFirmware; }
        virtual bool isHostSw() const { return false; }
    };

    /// @brief Abstract class for MC firmware
    /// Implementors shall subclass it for platform-specific behaviour
    class NICFirmware : public Firmware {
        static const String fwName;
        static const String fwDescription;
        static const String fwSysname;
    public:
        NICFirmware() : Firmware(fwDescription, fwSysname) {};
        virtual const String& className() const { return fwName; }
    };

    /// @brief Abstract class for BootROM
    /// Implementors shall subclass it for platform-specific behaviour
    class BootROM : public Firmware {
        static const String romName;
        static const String romDescription;
        static const String romSysname;
    public:
        BootROM() : Firmware(romDescription, romSysname) {};
        virtual const String& className() const { return romName; }
    };

    /// @brief NIC's vital product data (VPD) representation
    /// The set of fields is defined according to SF-108427-TC-2
    /// (IBM's requirements)
    class VitalProductData {
        String uuid;
        String manufac;
        String serialNo;
        String partNo;
        String modelId;
        String fruNo;
    public:
        VitalProductData(const String& id, const String& manf, 
                         const String& sno, const String& pno,
                         const String& m, const String& fru) :
            uuid(id), manufac(manf), serialNo(sno), partNo(pno),
            modelId(m), fruNo(fru) {}
        VitalProductData() {}
        const String& id() const { return uuid; }
        /// @return manufacturer field, or calls
        /// System::target.manufacturer() if the former is empty
        const String& manufacturer() const;
        const String& serial() const { return serialNo; }
        const String& part() const { return partNo; }
        const String& model() const { return modelId; }
        const String& fru() const { return fruNo; }
    };

    /// @brief abstract mutating enumerator for ports
    class PortEnumerator {
    public:
        virtual bool process(Port& p) = 0;
    };

    /// @brief abstract constant enumerator for ports
    class ConstPortEnumerator {
    public:
        virtual bool process(const Port& p) = 0;
    };

    class SoftwareEnumerator {
    public:
        virtual bool process(SWElement& sw) = 0;
    };

    class ConstSoftwareEnumerator {
    public:
        virtual bool process(const SWElement& sw) = 0;
    };

    /// @brief An abstract mix-in for software containing elements (NIC, Package and System)
    class SoftwareContainer {
    public:
        virtual bool forAllSoftware(SoftwareEnumerator& en) = 0;
        virtual bool forAllSoftware(ConstSoftwareEnumerator& en) const = 0;
    };


    /// @brief An abstract mix-in for port containing elements (NIC, System)
    class PortContainer {
    public:
        virtual bool forAllPorts(PortEnumerator& en) = 0;
        virtual bool forAllPorts(ConstPortEnumerator& en) const = 0;
    };

    /// @brief An abstract class for NIC elements
    /// Implementors shall subclass it for platform-specific behaviour
    class NIC : public HWElement, public SoftwareContainer, public PortContainer {
        static const String nicDescription;
        static const String nicName;
    protected:
        /// Create all necessary internal structures for physical ports
        /// (typically, that would imply creating instance of Port subclass
        /// and calling initialize() on it
        virtual void setupPorts() = 0;
        /// Create all necessary internal structures for present firmware
        virtual void setupFirmware() = 0;
    public:
        NIC(unsigned i) : HWElement(nicDescription, i) {};
        virtual VitalProductData vitalProductData() const = 0;
        enum Connector {
            RJ45,
            SFPPlus,
            Mezzanine
        };
        /// @return physical connector type
        virtual Connector connector() const = 0;
        /// @return maximum link speed (defaults to 10G)
        virtual uint64 maxLinkSpeed() const { return uint64(10) * 1024 * 1024 * 1024; }
        /// @return largest possible MTU for the NIC
        virtual uint64 supportedMtu() const = 0;
        virtual const String& className() const { return nicName; }
        virtual void initialize()
        {
            setupPorts();
            setupFirmware();
        }
        /// Apply @p en to all firmware of the NIC
        virtual bool forAllFw(SoftwareEnumerator& en) = 0;
        /// Apply @p en to all firmware of the NIC (non-destructive)
        virtual bool forAllFw(ConstSoftwareEnumerator& en) const = 0;
        virtual bool forAllSoftware(SoftwareEnumerator& en) { return forAllFw(en); }
        virtual bool forAllSoftware(ConstSoftwareEnumerator& en) const { return forAllFw(en); }
    };


    class Package;

    /// @brief An abstract member of a software package
    class HostSWElement : public SWElement {
    public:
        virtual const Package *package() const = 0;
        HostSWElement(const String& d, const String& sn) :
            SWElement(d, sn) {};
        virtual bool isHostSw() const { return true; }
    };

    /// @brief An abstract driver class
    class Driver : public HostSWElement {
    public:
        Driver(const String& d, const String& sn) :
            HostSWElement(d, sn) {};
        virtual SWClass classify() const { return SWDriver; }
    };

    /// @brief An abstract userspace library class
    class Library : public HostSWElement {
    public:
        Library(const String& d, const String& sn) :
            HostSWElement(d, sn) {};
        virtual SWClass classify() const { return SWLibrary; }
    };

    /// @brief An abstract userspace tool class
    class Tool : public HostSWElement {
    public:
        Tool(const String& d, const String& sn) :
            HostSWElement(d, sn) {};
        virtual SWClass classify() const { return SWTool; }
    };

    
    /// @brief An abstract class for software packages (RPM, MSI, ...)
    /// Implentors shall subclass it for platform-specific behaviour
    class Package : public SWElement, public SoftwareContainer {
    protected:
        /// Creates internal structures representing package members
        /// (usually as instances of SWElememt subclasses)
        virtual void setupContents() = 0;
    public:
        Package(const String& descr, const String& pkg) :
            SWElement(descr, pkg) {}
        virtual SWClass classify() const { return SWPackage; }
        virtual void initialize() { setupContents(); }
        enum PkgType {
            RPM,
            Deb,
            Tarball,
            MSI
        };
        virtual PkgType type() const = 0;
        virtual bool isHostSw() const { return false; }
    };

    class NICEnumerator {
    public:
        virtual bool process(NIC& p) = 0;
    };

    class ConstNICEnumerator {
    public:
        virtual bool process(const NIC& p) = 0;
    };

    /// @brief An abstract topmost class
    /// Implementors must subclass it to define a specific platform
    /// All others abstract classes above will usually need to be subclassed too
    class System : public SystemElement, public SoftwareContainer, public PortContainer {
        System(const System&);
        const System& operator = (const System&);
        static const String manfId;
        static const String systemDescr;
        static const String systemName;
        bool initialized;
    protected:
        System() : 
            SystemElement(systemDescr), initialized(false) {}
        /// Detect all Solarflare NICs in the system and make corresponding
        /// internal structures
        virtual void setupNICs() = 0;
        /// Detect all Solarflare software packages in the system and make corresponding
        /// internal structures
        virtual void setupPackages() = 0;
    public:
        /// The root object. It is defined as a reference, so a hidden
        /// concrete instance of a platform-specific subclass may be created
        static System& target;
        const String& manufacturer() const { return manfId; };
        virtual bool is64bit() const = 0;
        enum OSType {
            WindowsServer2003,
            WindowsServer2008,
            WindowsServer2008R2,
            RHEL,
            SLES,
            Debian,
            CentOS,
            OracleEL,
            GenericLinux,
            VMWareESXi
        };
        virtual OSType osType() const = 0;
        virtual void initialize() 
        {
            if (!initialized)
            {
                initialized = true;
                setupNICs();
                setupPackages();
            }
        }

        virtual bool forAllNICs(ConstNICEnumerator& en) const = 0;
        virtual bool forAllNICs(NICEnumerator& en) = 0;
        virtual bool forAllPackages(ConstSoftwareEnumerator& en) const = 0;
        virtual bool forAllPackages(SoftwareEnumerator& en) = 0;

        virtual bool forAllPorts(ConstPortEnumerator& en) const;
        virtual bool forAllPorts(PortEnumerator& en);
        virtual bool forAllSoftware(ConstSoftwareEnumerator& en) const;
        virtual bool forAllSoftware(SoftwareEnumerator& en);
        
        virtual const String& className() const { return systemName; }
    };

}


#endif
