#ifndef SOLARFLARE_SF_PLATFORM_H
#define SOLARFLARE_SF_PLATFORM_H 1

#include <cimple/cimple.h>
#include <cimple/Buffer.h>

// Classes that should be subclassed for specific platform

/// @brief Namespace for OS & Driver abstraction classes.
/// 
/// OS&Driver abstraction classes are used to create an abstraction 
/// layer between CIM-level logic of the Solarflare provider and low level
/// infrastructure.
///
/// See SF-108787 document for details.
///
namespace solarflare 
{
    using cimple::Buffer;
    using cimple::String;
    using cimple::Datetime;
    using cimple::uint64;
    using cimple::Mutex;


    class NIC;

    /// @brief Abstract class for ports. Implementors shall subclass it for
    /// platform-specific port representation.
    class Port : public HWElement, public NICElement {
        /// Class-wide name of the port to be passed into constructor
        static const String portName;
        /// Class-wide description of the port to be passed into constructor
        static const String portDescription;
    public:
        /// Constructor
        ///
        /// @param i  Index of the port (0, 1)
        Port(unsigned i) : HWElement(portDescription, i) {}

        /// @return link status
        virtual bool linkStatus() const = 0;

        /// @return interface status; fixme
        virtual bool ifStatus() const = 0;

        /// @return changing the interface state up/down; fixme
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

        /// Enable/disable autonegotiation according to @p an.
        virtual void autoneg(bool an) = 0;

        /// Causes a renegotiation like 'ethtool -r'.
        virtual void renegotiate() = 0;

        /// @return current MTU
        virtual uint64 mtu() const = 0;

        /// Change MTU to @p u
        virtual void mtu(uint64 u) = 0;

        /// @return system interface name (e.g. ethX for Linux); fixme
        virtual String ifName() const = 0;

        /// @return Manufacturer-supplied MAC address
        virtual MACAddress permanentMAC() const = 0;

        /// @return MAC address actually in use
        virtual MACAddress currentMAC() const = 0;

        /// Change the current MAC address to @p mac.
        virtual void currentMAC(const MACAddress& mac) = 0;

        /// We're a port.
        virtual const String& genericName() const { return portName; }
    };

    /// @brief Abstract class for MC firmware. Implementors shall subclass
    /// it for platform-specific behaviour.
    class NICFirmware : public Firmware {
        // Class-wide name and description for all firmware instances; to be
        // passed into superclass.
        static const String fwName;
        static const String fwDescription;
        // fixme: fwSysname meaning is not fully clear, for now it's the
        // same for all class instances.
        static const String fwSysname;
    public:
        /// Constructor
        NICFirmware() : Firmware(fwDescription, fwSysname) {};
        /// @return Same class name for all instances.
        virtual const String& genericName() const { return fwName; }
    };

    /// @brief Abstract class for BootROM. Implementors shall subclass it
    /// for platform-specific behaviour
    class BootROM : public Firmware {
        // Class-wide name and description for all BootROM instances; to be
        // passed into superclass.
        static const String romName;
        static const String romDescription;
        // fixme: same as FW
        static const String romSysname;
    public:
        /// Constructor
        BootROM() : Firmware(romDescription, romSysname) {};
        /// @return Same class name for all instances
        virtual const String& genericName() const { return romName; }
    };

    /// @brief An abstract class for NIC elements
    /// Implementors shall subclass it for platform-specific behaviour
    class NIC : public HWElement,
                public SoftwareContainer,
                public PortContainer {
        // Same name and description for all class instances.
        static const String nicDescription;
        static const String nicName;
    protected:
        /// Create all necessary internal structures for physical ports
        /// (typically, that would imply creating instance of Port subclass
        /// and calling initialize() on it
        virtual void setupPorts() = 0;

        /// Create all necessary internal structures for present firmware
        virtual void setupFirmware() = 0;

        /// fixme: setup interfaces
    public:
        /// Constructor
        ///
        /// @param i Index that uniqely identifies this particular instance
        ///          of SF NIC among other SF NICs in the system. Starts
        ///          from zero.
        NIC(unsigned i) : HWElement(nicDescription, i) {};

        /// @return NIC VPD
        virtual VitalProductData vitalProductData() const = 0;

        /// Network connector type.
        enum Connector {
            RJ45,
            SFPPlus,
            Mezzanine
        };

        /// @return Physical connector type.
        virtual Connector connector() const = 0;

        /// @return maximum link speed (defaults to 10G)
        /// fixme: enum
        virtual uint64 maxLinkSpeed() const { return uint64(10) * 1024 * 1024 * 1024; }

        /// @return largest possible MTU for the NIC
        virtual uint64 supportedMTU() const = 0;
        virtual const String& genericName() const { return nicName; }
        virtual void initialize()
        {
            setupPorts();
            setupFirmware();
        }
        /// Apply @p en to all firmware of the NIC
        virtual bool forAllFw(SoftwareEnumerator& en) = 0;
        /// Apply @p en to all firmware of the NIC (non-destructive)
        virtual bool forAllFw(ConstSoftwareEnumerator& en) const = 0;

        // Inherited methods
        virtual bool forAllSoftware(SoftwareEnumerator& en) { return forAllFw(en); }
        virtual bool forAllSoftware(ConstSoftwareEnumerator& en) const { return forAllFw(en); }
    };

    /// @brief An abstract driver class.
    class Driver : public HostSWElement {
    public:
        /// Constructor
        ///
        /// @param d  Description
        /// @param sn Kernel module name
        Driver(const String& d, const String& sn) :
            HostSWElement(d, sn) {};
        virtual SWClass classify() const { return SWDriver; }
    };

    /// @brief An abstract userspace library class.
    class Library : public HostSWElement {
    public:
        /// Constructor
        ///
        /// @param d  Description
        /// @param sn Shared library path
        Library(const String& d, const String& sn) :
            HostSWElement(d, sn) {};
        virtual SWClass classify() const { return SWLibrary; }
    };

    /// @brief An abstract userspace tool class.
    class Tool : public HostSWElement {
    public:
        /// Constructor
        ///
        /// @param d  Description
        /// @param sn Path of the binary
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
        /// Constructor
        ///
        /// @param descr Package description
        /// @param pkg   Package name
        Package(const String& descr, const String& pkg) :
            SWElement(descr, pkg) {}
        virtual SWClass classify() const { return SWPackage; }
        virtual void       initialize() { setupContents(); }
        /// Packaging type
        enum PkgType {
            RPM,
            Deb,
            Tarball,
            MSI,
            VSphereBundle,
        };
        /// @return Type of the package.
        virtual PkgType type() const = 0;

        /// Package is not host SW as it is a meta thing.
        virtual bool isHostSw() const { return false; }
    };

    /// @brief An abstract topmost class
    /// Implementors must subclass it to define a specific platform
    /// All others abstract classes above will usually need to be subclassed too
    class System : public SystemElement, public SoftwareContainer, public PortContainer {
        /// Singleton
        System(const System&);
        const System& operator = (const System&);

        /// Manufacturer ID
        static const String manfId;

        /// Short form of Manufacturer ID
        static const String nsPrefix;

        // Class-wide name and description
        static const String systemDescr;
        static const String systemName;

        /// True if initialization was performed
        bool initialized;
    protected:
        System() : 
            SystemElement(systemDescr), initialized(false) {}

        /// Detect all Solarflare NICs in the system and make corresponding
        /// internal structures (including FW instances creation).
        virtual void setupNICs() = 0;

        /// Detect all Solarflare software packages in the system and make corresponding
        /// internal structures.
        virtual void setupPackages() = 0;
    public:
        /// The root object. It is defined as a reference, so a hidden
        /// concrete instance of a platform-specific subclass may be created
        static System& target;

        /// @return Global vendor id.
        const String& manufacturer() const { return manfId; };

        /// @return A shortened vendor name to use as a prefix for
        /// namespaces etc. Required by IBM.
        const String& prefix() const { return nsPrefix; }

        /// @return True iff the host system is 64-bit. Affects
        /// representation of some of the CIM instances fields.
        virtual bool is64bit() const = 0;

        /// Installed OS type
        ///
        /// fixme: do we actually need this?
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

        /// @return host OS type
        virtual OSType osType() const = 0;
        // fixme: mutex!
        virtual void initialize() 
        {
            if (!initialized)
            {
                initialized = true;
                setupNICs();
                setupPackages();
            }
        }

        /// Apply en to all NICs in the system (non-destructively)
        virtual bool forAllNICs(ConstNICEnumerator& en) const = 0;
        /// Apply en to all NICs in the system
        virtual bool forAllNICs(NICEnumerator& en) = 0;
        /// Apply en to all softwre packages in the system (non-destructively)
        virtual bool forAllPackages(ConstSoftwareEnumerator& en) const = 0;
        /// Apply en to all softwre packages in the system 
        virtual bool forAllPackages(SoftwareEnumerator& en) = 0;

        virtual bool forAllPorts(ConstPortEnumerator& en) const;
        virtual bool forAllPorts(PortEnumerator& en);
        virtual bool forAllSoftware(ConstSoftwareEnumerator& en) const;
        virtual bool forAllSoftware(SoftwareEnumerator& en);
        
        virtual const String& genericName() const { return systemName; }
    };

} // namespace

#endif  // SOLARFLARE_SF_PLATFORM_H