/***************************************************************************//*! \file liprovider/sf_core.h
** <L5_PRIVATE L5_SOURCE>
** \author  OktetLabs
**  \brief  CIM Provider
**   \date  2013/10/02
**    \cop  (c) Solarflare Communications Inc.
** </L5_PRIVATE>
*//*
\**************************************************************************/

#ifndef SOLARFLARE_SF_CORE_H
#define SOLARFLARE_SF_CORE_H 1

#include <cimple/cimple.h>
#include <cimple/Buffer.h>

#include "sf_utils.h"
#include "sf_threads.h"
#include "sf_logging.h"

#define __SF_MK_VSTRING(_x, _y, _z) #_x "." #_y "." #_z
#define SF_MK_VSTRING(_x, _y, _z) __SF_MK_VSTRING(_x, _y, _z)

#define SF_LIBPROV_VERSION                      \
    SF_MK_VSTRING(PROVIDER_VERSION_MAJOR,       \
                  PROVIDER_VERSION_MINOR,       \
                  PROVIDER_REVISION)

// Platform-independent classes.

namespace solarflare 
{
    using cimple::Buffer;
    using cimple::String;
    using cimple::Datetime;
    using cimple::uint64;
    using cimple::Mutex;

    class CIMHelper;

    class SWType;

    /// Class for an exception thrown by our code
    class ProviderException {
        String file;    ///< File where exception was thrown
        unsigned line;  ///< Line where exception was thrown
        String msg;
    public:
        ProviderException(const String &fileName,
                          unsigned lineNo) : file(fileName),
                                             line(lineNo),
                                             msg("")
        {
            solarflare::Logger::errorLog.format(
                                      fileName.c_str(), lineNo,
                                      "ProviderException was thrown");
        }

        ProviderException(const String &fileName,
                          unsigned lineNo,
                          const String &message) :
                                    file(fileName), line(lineNo),
                                    msg(message)
        {
            solarflare::Logger::errorLog.format(
                                      fileName.c_str(), lineNo,
                                      "ProviderException was thrown: %s",
                                      message.c_str());
        }

        String getFile() const { return file; }
        unsigned getLine() const { return line; }
        String getMessage() const { return msg; }
    };

#define THROW_PROVIDER_EXCEPTION \
    throw ProviderException(__FILE_REL__, __LINE__)

#define THROW_PROVIDER_EXCEPTION_FMT(_fmt...) \
    do {                                                  \
        cimple::Buffer _buf;                              \
        _buf.format(_fmt);                                \
        throw ProviderException(__FILE_REL__, __LINE__,   \
                                _buf.data());             \
    } while (0)

#define ASSIGN_IGNORE_EXCEPTION(_var, _expr, _def_val) \
    do {                                                  \
        try {                                             \
            _var = (_expr);                               \
        }                                                 \
        catch (const ProviderException &exception)        \
        {                                                 \
            _var = (_def_val);                            \
        }                                                 \
    } while (0)

    /// @brief The root class for all managed objects.
    class SystemElement {
        String descr;
    public:
        SystemElement(const String& d) :
            descr(d) {}
        virtual ~SystemElement() {}

        /// @return Human readable description.
        const String& description() const { return descr; }

        /// Change human readable description.
        void setDescription(const String& d)
        {
            descr = d;
        }

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

        /// @return a helper instance to work with CIM instances of class @p cls.
        /// @retval NULL if a given SystemElement is not compatible with a given CIM class
        virtual const CIMHelper *cimDispatch(const cimple::Meta_Class& cls) const { return NULL; }
        cimple::Instance *cimReference(const cimple::Meta_Class& cls, unsigned idx = 0) const;

        virtual Thread *embeddedThread() { return NULL; }

        /// Returns generic type of this software (if this object
        /// represents software) or NULL.
        ///
        /// @return SWType class instance
        virtual SWType *getSWType() const { return NULL; }
    };

    /// @brief Abstract mutable enumerator for SystemObject
    class ElementEnumerator {
    public:
        virtual ~ElementEnumerator() {}
        virtual bool process(SystemElement& elem) = 0;
    };

    /// @brief Abstract immutable enumerator for SystemObject
    class ConstElementEnumerator {
    public:
        virtual ~ConstElementEnumerator() {}
        virtual bool process(const SystemElement& elem) = 0;
    };

    /// @brief Abstract class for software components.
    class SWElement : public SystemElement {
        String sysname;
        /// A private subclass of Thread to do asynchronous install
        class InstallThread : public Thread {
            SWElement *owner;
            String filename;
            bool force;
            String hash;
            
        protected:
            virtual Thread *dup() const;
            virtual bool threadProc();
        public:
            virtual String getThreadID() const;
            virtual void update(Thread *tempThr);
            InstallThread(SWElement *own) : owner(own) {}
            InstallThread(SWElement *own, String sid) :
                Thread(sid), owner(own) {}
            void setFilename(const char *f)
            {
                filename = f;
            }
            void setForce(bool f)
            {
                force = f;
            }
            void setHash(const char *base64_hash)
            {
                if (base64_hash != NULL)
                    hash = String(base64_hash);
            }
        };
        InstallThread installer;

    public:
        virtual const CIMHelper *cimDispatch(const cimple::Meta_Class& mc) const;        

        //// Constructor
        ////
        //// @param d     Description
        //// @param sn    System name
        SWElement(const String& d, const String& sn) :
            SystemElement(d), sysname(sn), installer(this) {}

        /// This is defined to prevent incorrect copying of
        /// a child thread class member
        SWElement(const SWElement &rhs) :
            SystemElement(rhs), sysname(rhs.sysname), installer(this) {}

        /// This is defined to prevent incorrect copying of
        /// a child thread class member
        SWElement &operator=(const SWElement &rhs)
        {
            if (this == &rhs)
                return *this;

            SystemElement::operator=(rhs);
            sysname = rhs.sysname;

            return *this;
        }

        /// Returns version of the installed component. Platform-specific
        /// subclasses shall implement the actual behaviour and retrieve the
        /// version from OS or driver.
        ///
        /// @return Filled in version or unknown version if the component is
        /// not installed (currently impossible).
        virtual VersionInfo version() const = 0;

        /// Return code of install() and syncInstall()
        enum InstallRC {
            Install_OK = 1,           ///< Installation successeded
            Install_NA,               ///< Firmware image is not applicable
            Install_Error,            ///< Error occured during installation
        };

        /// Updates a software component from @p filename.
        /// This method is only responsible for proper handling of
        /// asynchronous call; all real work is done in syncInstall()
        ///
        /// @param filename     Filename with new SW version 
        /// @param sync         If @p sync is false, a separate thread
        ///                     shall be created and true will be
        ///                     returned. The corresponding thread may
        ///                     be then obtained by calling
        ///                     installThread().
        /// @param force        Do update even if it is actually a
        ///                     downgrade to lower version (or reinstalling
        ///                     the same version)
        /// @param base64_hash  SHA-1 hash of firmware image,
        ///                     encoded in Base64 string
        ///
        /// @return Install_OK if the installation successed or we are in
        ///         async mode, Install_NA if firmware image is not
        ///         applicable, Install_Error in case of error
        InstallRC install(const char *filename, bool sync = true,
                          bool force = false,
                          const char *base64_hash = NULL);

        /// Actually updates a software component from @p filename.
        /// The method shall be overriden in platform-specific subclasses.
        ///
        /// @note It's not intended to be called directly; it is made public
        /// because it shall be callable from InstallThread. 
        /// But it shall do no harm if called from some other context.
        virtual InstallRC syncInstall(const char *filename,
                                      bool force,
                                      const char *base64_hash) = 0;

        /// Method returns system name of the component (e.g. object's file
        /// name, rpm name etc.). This is required so that we can relate to
        /// some OS-level object.
        virtual String sysName() const { return sysname; }

        /// Kind of the software entity.
        ///
        /// It should be noted that we don't have diagnostic drivers as
        /// those are forbiden by IBM requirements.
        enum SWClass {
            SWDriver = 1,  //< Kernel driver
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
        Thread *installThread() { return installer.findUpdate(); }
        virtual Thread *embeddedThread() { return installThread(); }

        /// Returns generic type of this software.
        ///
        /// @return SWType class instance
        virtual SWType *getSWType() const;
    };

    /// @brief Class representing generic software type
    class SWType : public SWElement {
        String swGenericName;
        SWClass swClass;
    public:
        virtual const CIMHelper *cimDispatch(
                                  const cimple::Meta_Class& mc) const;
        
        SWType(const String& d, const String& sn,
               const String& genName, SWClass swClassId) :
                  SWElement(d, sn), swGenericName(genName),
                  swClass(swClassId) {}

        virtual VersionInfo version() const
        {
            return VersionInfo();
        }

        virtual InstallRC syncInstall(const char *filename,
                                      bool force,
                                      const char *base64_hash)
        {
            return Install_Error;
        }

        virtual SWClass classify() const
        {
            return swClass;
        }

        virtual bool isHostSw() const
        {
            return false;
        }

        virtual void initialize() {}

        virtual const String& genericName() const
        {
            return swGenericName;
        }

        bool operator== (const SWType &rhs)
        {
            return (this->description() == rhs.description() &&
                    this->sysName() == rhs.sysName() &&
                    this->genericName() == rhs.genericName() &&
                    this->classify() == rhs.classify());
        }
    };

    /// @brief Auxuliary class to represent software identity associated
    ///        with a diagnostic instance
    class DiagSWElement : public SWElement {
        VersionInfo vers;
        static const String diagSwGenName;

      public:

        DiagSWElement() : SWElement("", "") {}

        DiagSWElement(const String& d, const String& sn,
                      const VersionInfo version) :
                             SWElement(d, sn), vers(version) {}

        virtual String name() const
        {
            return sysName();
        }

        virtual VersionInfo version() const
        {
            return vers;
        }

        virtual InstallRC syncInstall(const char *filename,
                                      bool force,
                                      const char *base64_hash)
        {
            return Install_Error;
        }

        virtual SWClass classify() const
        {
            return SWDiagnostics;
        }

        virtual void initialize() {}

        virtual bool isHostSw() const
        {
            return true;
        }

        virtual const String& genericName() const
        {
            return diagSwGenName;
        }
    };

    /// @brief Mix-in class for ordered elements (having index value).
    class OrderedElement {
        unsigned idx;
    public:
        OrderedElement(unsigned i) : idx(i) {}
        virtual ~OrderedElement() {}
        /// @return Element index
        virtual unsigned elementId() const { return idx; }
        virtual String name(const String& prefix) const;
    };

    /// @brief Abstract mix-in class for bus components
    /// (currently, NICs, Ports and Interfaces).
    /// Each bus component has a PCI bus address.
    class BusElement {
    public:
        virtual ~BusElement() {}

        /// @return PCI Address of the Element
        virtual PCIAddress pciAddress() const = 0;
    };

    class NIC;

    /// @brief Abstract mix-in for NIC associated elements (ports,
    /// interfaces and firmware)
    class NICElementMixIn {
    public:
        virtual ~NICElementMixIn() {}
        virtual const NIC *nic() const = 0;
    };

    /// @brief Abstract class for NIC associated elements (ports,
    /// interfaces and firmware)
    class NICElement : public SystemElement,
                       public OrderedElement,
                       public NICElementMixIn {
    public:
        /// Constructor
        ///
        /// @param d  Description
        /// @param i  Index in parent object
        NICElement(const String& d, unsigned i) : SystemElement(d),
                                                  OrderedElement(i) {}
        virtual ~NICElement() {}
        virtual String name() const;
    };

    /// @brief Abstract class for firmware elements.
    class Firmware : public SWElement, public NICElementMixIn {
    public:
        virtual const CIMHelper *cimDispatch(const cimple::Meta_Class& mc) const;

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

        /// @return IBM-specific value for IdentityInfoValue value
        virtual String sysName() const;
    };

    class Package;
    /// @brief An abstract member of a software package.
    class HostSWElement : public SWElement {
    public:
        virtual const CIMHelper *cimDispatch(const cimple::Meta_Class& mc) const;

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
        virtual ~SoftwareContainer() {}
        virtual bool forAllSoftware(ElementEnumerator& en) = 0;
        virtual bool forAllSoftware(ConstElementEnumerator& en) const = 0;
    };

    /// @brief An abstract mix-in for port containing elements (NIC, System)
    class PortContainer {
    public:
        virtual ~PortContainer() {}
        virtual bool forAllPorts(ElementEnumerator& en) = 0;
        virtual bool forAllPorts(ConstElementEnumerator& en) const = 0;
    };

    /// @brief An abstract mix-in for port containing elements (NIC, System)
    class InterfaceContainer {
    public:
        virtual ~InterfaceContainer() {}
        virtual bool forAllInterfaces(ElementEnumerator& en) = 0;
        virtual bool forAllInterfaces(ConstElementEnumerator& en) const = 0;
    };

    /// @brief An abstract mix-in for diagnostic containing elements (NIC, System)
    class DiagnosticContainer {
    public:
        virtual ~DiagnosticContainer() {}
        virtual bool forAllDiagnostics(ElementEnumerator& en) = 0;
        virtual bool forAllDiagnostics(ConstElementEnumerator& en) const = 0;
    };

    /// Types of firmware that can be updated by CIM provider
    typedef enum {
        FIRMWARE_UNKNOWN = 0, ///< Unknown firmware
        FIRMWARE_BOOTROM,     ///< BootROM
        FIRMWARE_MCFW         ///< MC firmware
    } UpdatedFirmwareType;
} // namespace

#endif  // SOLARFLARE_SF_CORE_H
