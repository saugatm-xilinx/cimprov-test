#ifndef SOLARFLARE_SF_CORE_H
#define SOLARFLARE_SF_CORE_H 1

#include <cimple/cimple.h>
#include <cimple/Buffer.h>

#include "sf_utils.h"
#include "sf_threads.h"

// Platform-independent classes.

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

        /// @return Human readable description.
        const String& description() const { return descr; }

        /// The ordinal number of the element inside its parent.  That is
        /// relevant for 'homogenenous' elements such as NICs or
        /// ports. Software elements are all different in some sense, so
        /// zero value is expected for them.
        virtual unsigned elementId() const { return 0; }

        /// Run-time initialization for things that are unsafe to do from
        /// constructors (including calling virtual methods).  CIM provider
        /// will call these method for topmost System object and the later
        /// will create other objects and create this method for them.
        virtual void initialize() = 0;

        /// Required as IBM requirements object name should be built in
        /// accordance with fancy rules which sometimes require inlining of
        /// class name into the instance name.
        ///
        /// @return a generic name of an entity (e.g. 'Ethernet Adaptor')
        virtual const String& genericName() const = 0;

        /// @return a name possibly suffixed with an element index
        /// (like. 'Ethernet Adaptor 1')
        virtual String name() const { return genericName(); }

        virtual cimple::Instance *cimInstance(const cimple::Meta_Class& cls) const { return NULL; };
        virtual cimple::Instance *cimReference(const cimple::Meta_Class& cls) const
        {
            return cimReference(cls);
        }
        virtual bool cimIsMe(const cimple::Instance& obj) const { return false; };
    };

    /// @brief Abstract mutable enumerator for SystemObject
    class ElementEnumerator {
    public:
        virtual bool process(SystemElement& elem) = 0;
    };

    /// @brief Abstract immutable enumerator for SystemObject
    class ConstElementEnumerator {
    public:
        virtual bool process(const SystemElement& elem) = 0;
    };


    /// @brief Abstract class for software components.
    class SWElement : public SystemElement {
        String sysname;
        /// A private subclass of Thread to do asynchronous install
        class InstallThread : public Thread {
            SWElement *owner;
            String filename;
        protected:
            virtual bool threadProc();
        public:
            InstallThread(SWElement *own) : 
                owner(own) {}
            void setFilename(const char *f)
            {
                filename = f;
            }
        };
        InstallThread installer;
        
    public:
        //// Constructor
        ////
        //// @param d     Description
        //// @param sn    System name
        SWElement(const String& d, const String& sn) :
            SystemElement(d), sysname(sn), installer(this) {}

        /// Returns version of the installed component. Platform-specific
        /// subclasses shall implement the actual behaviour and retrieve the
        /// version from OS or driver.
        ///
        /// @return Filled in version or unknown version if the component is
        /// not installed (currently impossible).
        virtual VersionInfo version() const = 0;

        /// Updates a software component from @p filename.
        /// This method is only responsible for proper handling of
        /// asynchronous call; all real work is done in syncInstall()
        ///
        /// @param filename Filename with new SW version 
        /// @param sync If @p sync is false, a separate thread shall be
        ///             created and true will be returned. The corresponding
        ///             thread may be then obtained by calling
        ///             installThread().
        ///
        /// @return FALSE if the installation failed, TRUE if it succeeds or
        /// we're in async mode.
        bool install(const char *filename, bool sync = true);

        /// Actually updates a software component from @p filename.
        /// The method shall be overriden in platform-specific subclasses.
        ///
        /// @note It's not intended to be called directly; it is made public
        /// because it shall be callable from InstallThread. 
        /// But it shall do no harm if called from some other context.
        virtual bool syncInstall(const char *filename) = 0;

        /// Method returns system name of the component (e.g. object's file
        /// name, rpm name etc.). This is required so that we can relate to
        /// some OS-level object.
        const String& sysName() const { return sysname; }

        /// Kind of the software entity.
        ///
        /// It should be noted that we don't have diagnostic drivers as
        /// those are forbiden by IBM requirements.
        enum SWClass {
            SWDriver,      //< Kernel driver
            SWTool,        //< Userspace configuration tool
            SWLibrary,     //< Userspace library
            SWPackage,     //< Collection of other SW entities
            SWFirmware,    //< Some firmware or bootcode
            SWDiagnostics, //< Userspace diagnostic tool
        };

        /// @return SWClass of the instance.
        /// 
        /// @note We can not have virtual fields, so we have to use virtual
        /// method that should be overridden in each subclass and should
        /// return constant value.
        ///
        /// @note Ideally, we should use RTTI to determine whether
        /// it is safe to cast a pointer/reference to object from
        /// a base class to a given subclass. Unfortunately, RTTI may
        /// be just not present in a given implementation (e.g. -fno-rtti
        /// flag passed to gcc). Besides that, RTTI is not very handy
        /// when it comes to a small known set of subclasses (one cannot
        /// do a switch in this case and is urged to use if-cascade with
        /// dynamic_casts. Therefore, we implement a tiny replacement
        /// for full-fledged RTTI that just allows us to determine the
        /// actual subclass of a SWElement.
        virtual SWClass classify() const = 0;

        /// Returns true iff the software element is an actual host-running
        /// software (i.e. a driver, a library or a tool, but not firmware
        /// or a software package)
        /// 
        /// Effectively, if isHostSw() is true, the object may be safely
        /// cast to HostSWElement
        virtual bool isHostSw() const = 0;

        /// Returns an installation asynchronous thread
        /// If no thread is currently running, the implementation is
        /// allowed to return NULL or an inactive Thread object.
        /// The returned object is logically owned by the object, so the
        /// caller must never delete the thread, nor use it after the object
        /// is deleted
        ///
        /// @return Thread or (NULL or inactive Thread object)
        virtual Thread *installThread() { return &installer; }
    };

    /// @brief Abstract class for bus components (currently, NICs and
    /// Ports).  A hardware element always has an ordinal number inside its
    /// parent object, and a PCI bus address.
    class BusElement : public SystemElement {
        unsigned idx;
    public:
        /// Constructor
        ///
        /// @param d  Description
        /// @param i  Index in parent object
        BusElement(const String& d, unsigned i) :
            SystemElement(d), idx(i) {}

        /// @return Element index
        virtual unsigned elementId() const { return idx; }

        /// @return PCI Address of the Element
        virtual PCIAddress pciAddress() const = 0;

        /// Name of the object in accordance with IBM requirements.
        virtual String name() const;
    };

    class NIC;

    /// @brief Abstract mix-in for NIC associated elements (ports and
    /// firmware)
    class NICElement {
    public:
        virtual const NIC *nic() const = 0;
    };

    /// @brief Abstract class for firmware elements.
    class Firmware : public SWElement, public NICElement {
    public:
        /// Constructor
        ///
        /// @param d  Description
        /// @param sn System name, different meaning for subclasses
        Firmware(const String& d, const String& sn) : SWElement(d, sn) {};

        /// @return We're a firmware
        virtual SWClass classify() const { return SWFirmware; }
        /// @return We're NIC-side SW
        virtual bool isHostSw() const { return false; }
        /// @return Name of the firmware image prefixed with NIC name
        virtual String name() const;
    };

    class Package;
    /// @brief An abstract member of a software package.
    class HostSWElement : public SWElement {
    public:
        /// The containing software package (e.g. RPM)
        /// which this element belongs to.
        virtual const Package *package() const = 0;
        HostSWElement(const String& d, const String& sn) :
            SWElement(d, sn) {};
        virtual bool isHostSw() const { return true; }
    };

    /// @brief An abstract mix-in for software containing elements (NIC,
    /// Package and System)
    class SoftwareContainer {
    public:
        virtual bool forAllSoftware(ElementEnumerator& en) = 0;
        virtual bool forAllSoftware(ConstElementEnumerator& en) const = 0;
    };

    /// @brief An abstract mix-in for port containing elements (NIC, System)
    class PortContainer {
    public:
        virtual bool forAllPorts(ElementEnumerator& en) = 0;
        virtual bool forAllPorts(ConstElementEnumerator& en) const = 0;
    };

    /// @brief An abstract mix-in for port containing elements (NIC, System)
    class InterfaceContainer {
    public:
        virtual bool forAllInterfaces(ElementEnumerator& en) = 0;
        virtual bool forAllInterfaces(ConstElementEnumerator& en) const = 0;
    };

    /// @brief An abstract mix-in for diagnostic containing elements (NIC, System)
    class DiagnosticContainer {
    public:
        virtual bool forAllDiagnostics(ElementEnumerator& en) = 0;
        virtual bool forAllDiagnostics(ConstElementEnumerator& en) const = 0;
    };

} // namespace

#endif  // SOLARFLARE_SF_CORE_H
