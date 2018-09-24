/***************************************************************************//*! \file liprovider/sf_platform.h
** <L5_PRIVATE L5_SOURCE>
** \author  OktetLabs
**  \brief  CIM Provider
**   \date  2013/10/02
**    \cop  (c) Solarflare Communications Inc.
** </L5_PRIVATE>
*//*
\**************************************************************************/

#ifndef SOLARFLARE_SF_PLATFORM_H
#define SOLARFLARE_SF_PLATFORM_H 1

#include <cimple/cimple.h>
#include <cimple/Buffer.h>
#include <cimple/Property.h>

#include "sf_core.h"
#include "sf_logging.h"
#include "sf_locks.h"

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
    using cimple::Property;
    using cimple::Buffer;
    using cimple::String;
    using cimple::Datetime;
    using cimple::uint32;
    using cimple::uint64;
    using cimple::sint64;
    using cimple::Mutex;
    using cimple::Array;
    using cimple::Array_String;
    using cimple::Array_uint32;

    ///
    /// Lock protecting NICs from simultaneous access.
    /// Ideally this should be per-NIC, but unfortunately we do not
    /// have global NIC objects, each thread has its own.
    /// Most methods do not need any protection, so they acquire
    /// this lock in a shared mode. Methods updating firmware need
    /// to acquire it in an exclusive mode.
    ///
    /// Note: if a thread wants to acquire this lock in an exclusive
    ///       mode, it should not try to acquire it in a shared mode
    ///       before that, or deadlock may happen.
    extern SharedLock nicsLock;

    class NIC;

    /// @brief Abstract class for diagnostics. Implementors shall subclass it for
    /// platform-specific port representation.
    class Diagnostic : public SystemElement, public NICElementMixIn,
                       public SoftwareContainer {
        //// Private subclass of Thread to run diagnostics asynchronously
        class DiagnosticThread : public Thread {
            Diagnostic *owner;
        protected:
            virtual bool threadProc();
            virtual Thread *dup() const;
            virtual void terminate()
            {
                owner->stop();
            };
        public:
            Logger resultLog;
            DiagnosticThread(Diagnostic *own) :
                owner(own),
                resultLog(LogInfo, maxRecordedEvents, "Result Log") {}
            DiagnosticThread(Diagnostic *own, String sid) :
                Thread(sid), owner(own),
                resultLog(LogInfo, maxRecordedEvents, "Result Log") {}
            virtual unsigned percentage() const;
            virtual void update(Thread *tempThread);
            virtual String getThreadID() const;

            friend class Diagnostic;
        };
        /// Diagnostic thread object
        DiagnosticThread diagThread;

        /// Number of failure events stored
        static const unsigned maxRecordedEvents;

        /// Associated software identity
        mutable DiagSWElement soft;

    public:
        /// Constructor
        ///
        /// @param d    Description
        Diagnostic(const String& d) :
            SystemElement(d), diagThread(this) {}

        /// Copy constructor
        Diagnostic(const Diagnostic& src) :
            SystemElement(src), 
            diagThread(src.diagThread)
        {
            diagThread.owner = this;
        }

        /// Runs the diagnostic either synchronously or not
        void run(bool sync = true);

        /// Three-state test result
        enum Result {
            Passed,   //< test passed
            Failed,   //< test failed
            NotKnown  //< test result not known (i.e. test not yet run)
        };


        /// The actual test routine (made public solely because
        /// of DiagnosticThread).
        /// It must be overridden in all actual diagnostic subclasses
        virtual Result syncTest() = 0;
        /// Return the percent of done work
        virtual unsigned percentage() const { return 0; }
        /// Attempt to stop hardware testing
        virtual void stop() {};

        /// @return Result of the latest diagnostic run
        virtual Result result() const = 0;
        /// Thread object to control over asynchronous tests
        Thread *asyncThread() { return diagThread.findUpdate(); }
        /// @return an associated software element or NULL
        virtual const SWElement *diagnosticTool() const;

        /// @return a name prefixed with NIC name
        virtual String name() const;

        // Following are test classifications

        /// @return true if the test alters device state in an arbitrary manner
        virtual bool isDestructive() const { return false; }
        /// @return true iff only one instance of test may be run on a device
        virtual bool isExclusive() const { return isDestructive(); }

        /// Diagnostic varieties
        enum TestKind {
            FunctionalTest, //< functional
            StressTest,     //< stress
            HealthCheckTest, //< health check
            MediaAccessTest //< media access test
        };
        /// @return the kind of the test
        virtual TestKind testKind() const { return FunctionalTest; }

        /// @return associated result logger
        Logger& log()
        {
            return static_cast<DiagnosticThread *>(asyncThread())->resultLog;
        }
        /// @return associated success logger (immutable)
        const Logger& log() const
        { 
            DiagnosticThread *thr = static_cast<DiagnosticThread *>(diagThread.find());
            if (thr == NULL)
                return diagThread.resultLog;
            else
                return thr->resultLog;
        }

        virtual Thread *embeddedThread() { return asyncThread(); }
        virtual const CIMHelper *cimDispatch(const cimple::Meta_Class& mc) const;

        virtual bool forAllSoftware(ElementEnumerator& en) {
            // diagnosticTool() is called to get software identity
            // info - it is not obtained in constructor
            if (diagnosticTool() != NULL)
                return en.process(soft);

            return true;
        }

        virtual bool forAllSoftware(ConstElementEnumerator& en) const {
            // diagnosticTool() is called to get software identity
            // info - it is not obtained in constructor
            if (diagnosticTool() != NULL)
                return en.process(soft);

            return true;
        }
    };

    /// @brief Abstract class for ports. Implementors shall subclass it for
    /// platform-specific port representation.
    class Port : public BusElement, public NICElement {
        /// Class-wide name of the port to be passed into constructor
        static const String portName;
        /// Class-wide description of the port to be passed into constructor
        static const char portDescription[];

    public:
        /// Constructor
        ///
        /// @param i  Index of the port (0, 1)
        Port(unsigned i) : NICElement(portDescription, i) {}

        /// @return link status
        virtual bool linkStatus() const = 0;

        /// link speed possible values
        enum Speed {
            SpeedUnknown,
            Speed10M,
            Speed100M,
            Speed1G,
            Speed10G,
            Speed25G,
            Speed40G,
            Speed50G,
            Speed100G
        };
        static uint64 speedBPS(Speed sp);
        static Speed speedValue(uint64 sp);

        /// @return current link speed
        virtual Speed linkSpeed() const = 0;

        /// @return change link speed to @p speed
        virtual void linkSpeed(Speed speed) = 0;

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

        /// @return Manufacturer-supplied MAC address.
        /// fixme: it's not clear if it should be here or in Interface
        virtual MACAddress permanentMAC() const = 0;

        /// We're a port.
        virtual const String& genericName() const { return portName; }

        virtual const CIMHelper *cimDispatch(const cimple::Meta_Class& mc) const;

	///
	/// Get privileges of a given physical or virtual function.
	///
	/// @param pf                   Physical function number
	/// @param vf                   Virtual function number
	/// @param privilegeNames [out] Array of privilege names
	/// @param privileges     [out] Array of privilege numbers in the
	///                             same order (each number is index of
	///                             corresponding bit in privilege
	///                             mask)
	///
	/// @return 0 on success, -1 on failure
	virtual int getPrivileges(
                        const Property<uint32> &pf,
                        const Property<uint32> &vf,
                        Property<Array_String> &privilegeNames,
                        Property<Array_uint32> &privileges) const
        {
            return -1;
        }

	///
	/// Modify privileges of a physical or virtual function.
	///
	/// @param pf                   Physical function number
	/// @param vf                   Virtual function number
	/// @param addedMask            Added privileges
	/// @param removedMask          Removed privileges
	///
	/// @note addedMask and removedMask have the folloding format:
	///       PRIVILEGE := privilege_name | ":"privilege_bit_index
	///       MASK := PRIVILEGE | MASK"|"PRIVILEGE
	///       Example: ONLOAD|:3|INSECURE
	///
	/// @return 0 on success, -1 on failure
	virtual int modifyPrivileges(
                        const Property<uint32> &pf,
                        const Property<uint32> &vf,
                        const Property<String> &addedMask,
                        const Property<String> &removedMask)
        {
            return -1;
        }

        ///
        /// Get interrupt moderation parameters.
        ///
        /// @param paramNames           Parameter names
        /// @param paramValues    [out] Parameter values
        ///
        /// @return 0 on success, -1 on failure
        virtual int getIntrModeration(const Array_String &paramNames,
                                      Array_uint32 &paramValues) const
        {
            return -1;
        }

        ///
        /// Set interrupt moderation parameters.
        ///
        /// @param paramNames           Parameter names
        /// @param paramValues          Parameter values
        ///
        /// @return 0 on success, -1 on failure
        virtual int setIntrModeration(const Array_String &paramNames,
                                      const Array_uint32 &paramValues)
        {
            return -1;
        }
    };

            
    inline uint64 Port::speedBPS(Speed sp)
    {
        switch (sp)
        {
            case Speed10M:
                return uint64(10) * 1000 * 1000; //10M
            case Speed100M:
                return uint64(100) * 1000 * 1000; //100M
            case Speed1G:
                return uint64(1000) * 1000 * 1000; // 1g
            case Speed10G:
                return uint64(10000) * 1000 * 1000; // 10g
            case Speed25G:
                return uint64(25000) * 1000 * 1000; // 25g
            case Speed40G:
                return uint64(40000) * 1000 * 1000; // 40g
            case Speed50G:
                return uint64(50000) * 1000 * 1000; // 50g
            case Speed100G:
                return uint64(100000) * 1000 * 1000; // 100g
            default:
                return 0;
        }
    }
        
    inline Port::Speed Port::speedValue(uint64 sp)
    {
        switch (sp)
        {
            case uint64(10) * 1000 * 1000:
            case uint64(10) * 1024 * 1024:
                return Speed10M;
            case uint64(100) * 1000 * 1000:
            case uint64(100) * 1024 * 1024:
                return Speed100M;
            case uint64(1000) * 1000 * 1000:
            case uint64(1000) * 1024 * 1024:
                return Speed1G;
            case uint64(10000) * 1000 * 1000:
            case uint64(10000) * 1024 * 1024:
                return Speed10G;
            case uint64(25000) * 1000 * 1000:
            case uint64(25000) * 1024 * 1024:
                return Speed25G;
            case uint64(40000) * 1000 * 1000:
            case uint64(40000) * 1024 * 1024:
                return Speed40G;
            case uint64(50000) * 1000 * 1000:
            case uint64(50000) * 1024 * 1024:
                return Speed50G;
            case uint64(100000) * 1000 * 1000:
            case uint64(100000) * 1024 * 1024:
                return Speed100G;
            default:
                return SpeedUnknown;
        }
    }


    class Interface : public BusElement, public NICElement {
        // Class-wide name (unrelated to OS ifname) and description.
        static const String ifGenName;
        static const char ifGenDescription[];
    public:
        /// Constructor
        ///
        /// @param i Index of the interface among all SF interfaces on
        ///          particular NIC (not system-wide).
        ///          fixme: or on all NICs?
        Interface(unsigned i) : NICElement(ifGenDescription, i) {}

        /// Function port.
        virtual Port *port() = 0;
        /// Function port in case we don't plan to modify it.
        virtual const Port *port() const = 0;
        
        /// @return interface status.
        virtual bool ifStatus() const = 0;

        /// @return changing the interface state up/down.
        virtual void enable(bool status) = 0;

        /// @return system interface name (e.g. ethX for Linux).
        virtual String ifName() const = 0;

        /// @return current MTU
        virtual uint64 mtu() const = 0;

        /// Change MTU to @p u
        virtual void mtu(uint64 u) = 0;

        /// @return MAC address actually in use
        virtual MACAddress currentMAC() const = 0;

        /// Change the current MAC address to @p mac.
        virtual void currentMAC(const MACAddress& mac) = 0;

        virtual const String& genericName() const { return ifGenName; }

        virtual const CIMHelper *cimDispatch(const cimple::Meta_Class& mc) const;
    };


    /// @brief Abstract class for MC firmware. Implementors shall subclass
    /// it for platform-specific behaviour.
    class NICFirmware : public Firmware {
        // Class-wide name and description for all firmware instances; to be
        // passed into superclass.
        static const String fwName;
        static const char fwDescription[];
        // fixme: fwSysname meaning is not fully clear, for now it's the
        // same for all class instances.
        static const char fwSysname[];
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
        static const char romDescription[];
        // fixme: same as FW
        static const char romSysname[];
    public:
        /// Constructor
        BootROM() : Firmware(romDescription, romSysname) {};
        /// @return Same class name for all instances
        virtual const String& genericName() const { return romName; }
    };


    /// @brief Abstract class for UEFIROM. Implementors shall subclass it
    /// for platform-specific behaviour
    class UEFIROM : public Firmware {
        // Class-wide name and description for all UEFIROM instances; to be
        // passed into superclass.
        static const String uefiromName;
        static const char uefiromDescription[];
        // fixme: same as FW
        static const char uefiromSysname[];
    public:
        /// Constructor
        UEFIROM() : Firmware(uefiromDescription, uefiromSysname) {};
        /// @return Same class name for all instances
        virtual const String& genericName() const { return uefiromName; }
    };


    class Driver;

    ///
    /// VPD field description
    ///
    class VPDField
    {
    public:
        String name;      ///< VPD field name
        Buffer data;      ///< VPD field data

        // Dummy operator to make possible using of cimple::Array
        bool operator== (const VPDField &rhs)
        {
            UNUSED(rhs);
            return false;
        }
    };

    /// @brief An abstract class for NIC elements
    /// Implementors shall subclass it for platform-specific behaviour
    class NIC : public SystemElement,
                public BusElement,
                public OrderedElement,
                public SoftwareContainer,
                public PortContainer,
                public InterfaceContainer,
                public DiagnosticContainer {
        // Same name and description for all class instances.
        static const char nicDescription[];
        static const String nicName;
    protected:
        /// Create all necessary internal structures for physical ports
        /// (typically, that would imply creating instance of Port subclass
        /// and calling initialize() on it
        virtual void setupPorts() = 0;

        /// Same as setupPorts but for interfaces.
        virtual void setupInterfaces() = 0;

        /// Create all necessary internal structures for present firmware
        virtual void setupFirmware() = 0;

        /// Create all necessary internal structures for all diagnostic tests
        virtual void setupDiagnostics() = 0;

    public:
        /// Constructor
        ///
        /// @param i Index that uniqely identifies this particular instance
        ///          of SF NIC among other SF NICs in the system. Starts
        ///          from zero.
        NIC(unsigned i) : SystemElement(nicDescription),
                          OrderedElement(i) {};

        ///
        /// Get all VPD fields
        ///
        /// @param staticVPD           Whether to get static or dynamic VPD
        /// @param parsedFields  [out] Parsed VPD fields
        ///
        /// @return 0 on success, -1 on failure
        virtual int getFullVPD(bool staticVPD,
                               Array<VPDField> &parsedFields) const
        {
            return -1;
        }

        /// @return NIC VPD
        virtual VitalProductData vitalProductData() const = 0;

        /// @return NIC Tag
        virtual String tag() const
        {
            return vitalProductData().id();
        }

        /// @return NIC product number from VPD
        virtual String productNumber() const
        {
            return vitalProductData().part();
        }

        /// Network connector type.
        enum Connector {
            RJ45,
            SFPPlus,
            Mezzanine
        };

        /// @return Physical connector type.
        virtual Connector connector() const = 0;

        /// @return Associated device driver or NULL
        virtual Driver *driver() const { return NULL; }

        /// @return Associated device driver or NULL
        virtual VersionInfo driverVersion() const;

        /// @return maximum link speed (defaults to 10G)
        /// fixme: enum
        virtual Port::Speed maxLinkSpeed() const {
            return Port::Speed10G;
        }

        /// @return largest possible MTU for the NIC
        virtual uint64 supportedMTU() const = 0;
        virtual const String& genericName() const { return nicName; }
        virtual void initialize()
        {
            setupPorts();
            setupInterfaces();
            setupFirmware();
            setupDiagnostics();
        }
        /// Apply @p en to all firmware of the NIC
        virtual bool forAllFw(ElementEnumerator& en) = 0;
        /// Apply @p en to all firmware of the NIC (non-destructive)
        virtual bool forAllFw(ConstElementEnumerator& en) const = 0;

        // Inherited methods
        virtual bool forAllSoftware(ElementEnumerator& en) {
            return forAllFw(en);
        }
        virtual bool forAllSoftware(ConstElementEnumerator& en) const {
            return forAllFw(en);
        }

        virtual const CIMHelper *cimDispatch(const cimple::Meta_Class& mc) const;

        virtual String name() const
        {
            return OrderedElement::name(genericName());
        }

	///
	/// Get PF/VF corresponding to a given PCI address.
	///
	/// @param searchPCIAddr       PCI address.
	/// @param found               Will be set to true if
	///                            PF/VF with provided PCI address
	///                            was found.
	/// @param pf_out              Where to save PF.
	/// @param vf_out              Where to save VF.
	/// @param vf_null             Will be set to true if PF was
	///                            found.
	///
	/// @return 0 on success, -1 on failure.
	virtual int getPFVFByPCIAddr(const PCIAddress &searchPCIAddr,
                                     bool &found, uint32 &pf_out,
                                     uint32 &vf_out,
                                     bool &vf_null) const
        {
            found = false;
            return 0;
        }

	///
	/// Get name of an interface having ADMIN privilege.
	///
	/// @param ifName   Where to save name of the interface.
	///
	/// @return 0 on success, -1 on failure.
	virtual int getAdminInterface(String &ifName) const
	{
	    return -1;
	}
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

        virtual const CIMHelper *cimDispatch(const cimple::Meta_Class& mc) const;
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

        virtual const CIMHelper *cimDispatch(const cimple::Meta_Class& mc) const;

        /// Returns generic type of this software.
        ///
        /// @return SWType class instance
        virtual SWType *getSWType() const;
    };

    /// @brief An abstract class for software packages (RPM, MSI, ...)
    /// cointaining our CIM provider. Implentors shall subclass it
    /// for platform-specific behaviour
    class ManagementPackage : public Package {
    public:
        ManagementPackage(const String& descr, const String& pkg) :
            Package(descr, pkg) {}
        virtual const CIMHelper *cimDispatch(const cimple::Meta_Class& mc) const;
    };

    ///
    /// Class representing generic software package type.
    ///
    class SWPkgType : public SWType {
      Package::PkgType pkgType;
    public:
        SWPkgType(const String& d, const String& sn,
                  const String& genName, SWClass swClassId,
                  Package::PkgType type) : SWType(d, sn,
                                                  genName, swClassId),
                                           pkgType(type)
        {};

        virtual Package::PkgType type() const
        {
            return pkgType;
        }
    };

    /// Entry of RequestedState values storage
    class ReqStateStorageEntry
    {
    public:
        unsigned requestedState; ///< RequestedState value
        String name;             ///< Instance name

        bool operator== (const ReqStateStorageEntry &rhs)
        {
            return (requestedState == rhs.requestedState &&
                    name == rhs.name);
        }

        ///
        /// Save RequestedState value for a given instance name.
        ///
        /// @param storage        Where to save
        /// @param name           Instance name
        /// @param RequestedState RequestedState parameter value to be
        ///                       saved
        ///
        static void saveReqState(Array<ReqStateStorageEntry> &storage,
                                 const String &name,
                                 unsigned reqSt);

        ///
        /// Get previously saved RequestedState value for a given
        /// instance name.
        ///
        /// @param storage        Where to get 
        /// @param name           Instance name
        /// @param value    [out] RequestState value
        /// 
        /// @return 0 on success, -1 on failure
        ///
        static int getReqState(
                            const Array<ReqStateStorageEntry> &storage,
                            const String &name,
                            unsigned &value);
    };

    /// @brief An abstract topmost class. Implementors must subclass it to
    /// define a specific platform All others abstract classes above will
    /// usually need to be subclassed too
    class System : public SystemElement,
                   public SoftwareContainer,
                   public PortContainer,
                   public InterfaceContainer,
                   public DiagnosticContainer {
        /// Singleton
        System(const System&);
        const System& operator = (const System&);

        /// Manufacturer ID
        static const String manfId;

        /// Short form of Manufacturer ID
        static const String nsPrefix;

        // Class-wide name and description
        static const char systemDescr[];
        static const String systemName;

        // Stores last value of RequestedState parameter used
        // when calling RequestStateChange() method for SF_EthernetPort,
        // to be able to set the same value to RequestedState property
        // of SF_EthernetPort instance after successful invocation
        // of this method (required by DSP 1035).
        static Array<ReqStateStorageEntry> portReqStateStorage;
        static Mutex portReqStateStorageLock;
        // Doest the same for logging class instances
        static Array<ReqStateStorageEntry> logReqStateStorage;
        static Mutex logReqStateStorageLock;

        /// True if initialization was performed
        bool initialized;
    protected:
        System() : 
            SystemElement(systemDescr), initialized(false) {}

        /// Detect all Solarflare NICs in the system and make corresponding
        /// internal structures (including FW instances creation).
        virtual void setupNICs() = 0;

        /// Detect all Solarflare software packages in the system and make
        /// corresponding internal structures.
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
#if 0
                // It is not safe to log from ctor due
                // to undefined order of static variables initialization
                // in CIMPLE logging system
                Logger::getEventLog().log("startup");
#endif
                initialized = true;
                setupPackages();
                setupNICs();
            }
        }

        /// Apply en to all NICs in the system (non-destructively)
        virtual bool forAllNICs(ConstElementEnumerator& en) const = 0;
        /// Apply en to all NICs in the system
        virtual bool forAllNICs(ElementEnumerator& en) = 0;
        /// Apply en to all softwre packages in the system
        /// (non-destructively)
        virtual bool forAllPackages(ConstElementEnumerator& en) const = 0;
        /// Apply en to all softwre packages in the system 
        virtual bool forAllPackages(ElementEnumerator& en) = 0;

        virtual bool forAllDrivers(ConstElementEnumerator& en) const
        {
            return true;
        };
        virtual bool forAllDrivers(ElementEnumerator& en)
        {
            return true;
        };
        virtual bool forAllPorts(ConstElementEnumerator& en) const;
        virtual bool forAllPorts(ElementEnumerator& en);
        virtual bool forAllInterfaces(ConstElementEnumerator& en) const;
        virtual bool forAllInterfaces(ElementEnumerator& en);
        virtual bool forAllDiagnostics(ConstElementEnumerator& en) const;
        virtual bool forAllDiagnostics(ElementEnumerator& en);
        /// Enumerate all software except diagnostics
        virtual bool forAllNDiagSoftware(ConstElementEnumerator& en) const;
        /// Enumerate all software objects except software type objects
        /// (const enumerator)
        virtual bool forAllSoftwareNTypes(ConstElementEnumerator& en) const;
        /// Enumerate all software type objects
        virtual bool forAllSoftwareTypes(ConstElementEnumerator& en) const;
        /// Enumerate all software, including software type objects
        virtual bool forAllSoftware(ConstElementEnumerator& en) const;
        /// Enumerate all software except diagnostics (not const
        /// enumerator)
        virtual bool forAllNDiagSoftware(ElementEnumerator& en);
        /// Enumerate all software (not const enumerator, so
        /// software type objects are not enumerated)
        virtual bool forAllSoftware(ElementEnumerator& en);

        virtual bool forAllObjects(ConstElementEnumerator& en) const;
        virtual bool forAllObjects(ElementEnumerator& en);
        
        virtual const String& genericName() const { return systemName; }

        virtual const CIMHelper *cimDispatch(const cimple::Meta_Class& mc) const;

        /// Save variable so that its value can be restored across reboots
        virtual int saveVariable(const String &id, const String &val) const
        {
            return -1;
        }

        /// Get value of previously saved variable
        virtual int loadVariable(const String &id, String &val) const
        {
            return -1;
        }

        static void savePortReqState(const String &portName,
                                     unsigned requestedState);
        static unsigned getPortReqState(const String &portName);
        static void saveLogReqState(const String &logName,
                                    unsigned requestedState);
        static unsigned getLogReqState(const String &logName);

        /// Get information required to know whether  a firmware
        /// image would be accepted for update of a given firmware
        /// type on a given NIC.
        ///
        /// @param nic                     NIC object
        /// @param type                    Firmware type
        /// @param img_type          [out] Firmware image type
        /// @param img_subtype       [out] Firmware image subtype
        /// @param img_name          [out] Firmware image default file name
        /// @param current_version   [out] Current version of installed
        ///                                firmware
        ///
        /// @return 0 on success, -1 on error
        virtual int getRequiredFwImageName(const NIC &nic,
                                           UpdatedFirmwareType type,
                                           unsigned int &img_type,
                                           unsigned int &img_subtype,
                                           String &img_name,
                                           VersionInfo &current_version)
        {
            return -1;
        }

        /// Check whether a given file contains applicable firmware
        /// image, and if it does, obtain firmware image version
        ///
        /// @param nic              NIC object
        /// @param type             Firmware type
        /// @param filename         File name
        /// @param applicable [out] Whether firmware image is applicable
        /// @param imgVersion [out] Firwmare image version
        ///
        /// @return 0 on success, -1 on error
        virtual int getLocalFwImageVersion(const NIC &nic,
                                           UpdatedFirmwareType type,
                                           const char *filename,
                                           bool &applicable,
                                           VersionInfo &imgVersion)
        {
            return -1;
        }

        virtual String createTmpFile()
        {
            return String(""); 
        }

        virtual int tmpFileBase64Append(const String &fileName,
                                        const String &base64Str)
        {
            return -1;
        }

        virtual int removeTmpFile(String fileName)
        {
            return -1;
        }

        // Get string representation of array of sfu_devices
        // structures returned by sfu_devide_enum()
        //
        // @return String representation of array encoded in Base64
        //         on success, empty string on failure
        virtual String getSFUDevices()
        {
            return String(""); 
        }

        // Call nv_exists().
        //
        // @param dev_name             Device name
        // @param type                  Partition type
        // @param subtype               Partition subtype
        // @param try_other_devs        Try other ports if on current
        //                              device partition cannot be accessed
        // @param correct_dev     [out] If try_other_devs is TRUE, here
        //                              name of device to be accessed will
        //                              be stored
        //
        // @return true if NV partition exists, false otherwise
        virtual bool NVExists(const String &dev_name,
                              unsigned int type, unsigned int subtype,
                              bool try_other_devs,
                              String &correct_dev)
        {
            return false;
        }

        // Call nv_open().
        //
        // @param dev_name      Device name
        // @param type          Partition type
        // @param subtype       Partition subtype
        //
        // @return NV context id (>= 0) on success, -1 on failure
        virtual int NVOpen(const String &dev_name,
                           unsigned int type, unsigned int subtype)
        {
            return -1;
        }

        // Call nv_close().
        //
        // @param nv_ctx        NV context id
        //
        // @return 0 on success, -1 on failure
        virtual int NVClose(unsigned int nv_ctx)
        {
            return -1;
        }

        // Return NV partition size
        //
        // @param nv_ctx        NV context id
        //
        // @return 0 on success, -1 on failure
        virtual size_t NVPartSize(unsigned int nv_ctx)
        {
            return 0;
        }

        // Read data from NV partition
        //
        // @param nv_ctx        NV context id
        // @param lenth         Number of bytes to be read
        // @param offset        Starting offset
        // @param data    [out] Read data
        //
        // @return 0 on success, -1 on failure
        virtual int NVRead(unsigned int nv_ctx,
                           uint64 length,
                           sint64 offset,
                           String &data)
        {
            return -1;
        }

        // Read all the data from NV partition
        //
        // @param nv_ctx        NV context id
        // @param data    [out] Read data
        //
        // @return 0 on success, -1 on failure
        virtual int NVReadAll(unsigned int nv_ctx,
                              String &data)
        {
            return -1;
        }

        // Write data to NV partition
        //
        // @param nv_ctx        NV context id
        // @param data          Data to be written
        // @param full_erase    If true, erase whole partition.
        //
        // @return 0 on success, -1 on failure
        virtual int NVWriteAll(unsigned int nv_ctx,
                               const String &data,
                               bool full_erase)
        {
            return -1;
        }

        // Read data from NV partition using MCDI directly
        //
        // @param dev_name        Device name
        // @param part_type       Partition type
        // @param len             Length of data to be read
        // @param offset          At which offset to read
        // @param data      [out] Where to save obtained data
        //
        // @return 0 on success, -1 on failure
        virtual int MCDIRead(const String &dev_name,
                             uint32 part_type,
                             uint32 len,
                             uint32 offset,
                             String &data)
        {
            return -1;
        }

        // Write data to NV partition using MCDI directly
        //
        // @param dev_name      Device name
        // @param part_type     Partition type
        // @param offset        At which offset to write
        // @param data          Data to be written
        //
        // @return 0 on success, -1 on failure
        virtual int MCDIWrite(const String &dev_name,
                              uint32 part_type,
                              uint32 offset,
                              const String &data)
        {
            return -1;
        }

        // Get driver load parameters
        //
        // @param loadParams  [out] Driver load parameters string
        //
        // @return 0 on success, -1 on failure
        virtual int getDriverLoadParameters(String &loadParams)
        {
            return -1;
        }

        // Set driver load parameters
        //
        // @param loadParams      Driver load parameters string
        //
        // @return 0 on success, -1 on failure
        virtual int setDriverLoadParameters(const String &loadParams)
        {
            return -1;
        }

        // Execute MCDI v1 command
        //
        // @param dev_name    [in]      Device name
        // @param cmd         [in, out] cmd field of efx_mcdi_request
        // @param len         [in, out] len field of efx_mcdi_request
        // @param rc          [in, out] rc field of efx_mcdi_request
        // @param payload     [in, out] payload field of efx_mcdi_request
        // @param ioctl_rc    [out]     value returned by ioctl() call
        // @param ioctl_errno [out]     errno after ioctl() call
        //
        // @return 0 if ioctl() was called, -1 if some error made calling
        // ioctl() with the given parameters impossible
        virtual int MCDIV1Command(const String &dev_name,
                                  unsigned int &cmd,
                                  unsigned int &len,
                                  unsigned int &rc,
                                  String &payload,
                                  int &ioctl_rc,
                                  unsigned int &ioctl_errno)
        {
            return -1;
        }

        // Execute MCDI v2 command
        //
        // @param dev_name    [in]      Device name
        // @param cmd         [in, out] cmd field of efx_mcdi_request2
        // @param inlen       [in, out] inlen field of efx_mcdi_request2
        // @param outlen      [in, out] outlen field of efx_mcdi_request2
        // @param flags       [in, out] flags field of efx_mcdi_request2
        // @param payload     [in, out] payload field of efx_mcdi_request2
        // @param host_errno  [in, out] host_errno field of efx_mcdi_request2
        // @param ioctl_rc    [out]     value returned by ioctl() call
        // @param ioctl_errno [out]     errno after ioctl() call
        //
        // @return 0 if ioctl() was called, -1 if some error made calling
        // ioctl() with the given parameters impossible
        virtual int MCDIV2Command(const String &dev_name,
                                  unsigned int &cmd,
                                  unsigned int &inlen,
                                  unsigned int &outlen,
                                  unsigned int &flags,
                                  String &payload,
                                  unsigned int &host_errno,
                                  int &ioctl_rc,
                                  unsigned int &ioctl_errno)
        {
            return -1;
        }

	///
	/// Get privileges of a NIC function (physical or virtual).
	///
	/// @note If both @p pf and @p vf are not set, then privileges are
	///       obtained for @p dev_name itself.
	///
	/// @param dev_name             Device name on which to make
	///                             request.
	/// @param pf                   Physical function number.
	/// @param vf                   Virtual function number.
	/// @param privilegeNames [out] Array of privilege names.
	/// @param privileges     [out] Array of privilege numbers in the
	///                             same order (each number is index of
	///                             corresponding bit in privilege
	///                             mask).
	///
	/// @return 0 on success, -1 on failure.
	virtual int getFuncPrivileges(
                                  const String &dev_name,
                                  const Property<uint32> &pf,
                                  const Property<uint32> &vf,
                                  Property<Array_String> &privilegeNames,
                                  Property<Array_uint32> &privileges) const
        {
            return -1;
        }
    };

} // namespace

#endif  // SOLARFLARE_SF_PLATFORM_H
