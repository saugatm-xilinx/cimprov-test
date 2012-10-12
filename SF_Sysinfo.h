#ifndef SOLARFLARE_SF_SYSINFO_H
#define SOLARFLARE_SF_SYSINFO_H 1

#include <cimple/cimple.h>
#include <cimple/Buffer.h>

namespace solarflare 
{
    using cimple::Buffer;
    using cimple::String;
    using cimple::Datetime;
    using cimple::uint64;
    using cimple::Mutex;
    
    class SystemElement {
        const SystemElement * parent;
        unsigned idx;
        unsigned nChildren;
        SystemElement *children;
        SystemElement *next;
        String descr;
        void makePath(Buffer& buf) const;
    protected:
        void push(SystemElement *child);
        virtual const String& baseId() const = 0;
        virtual bool singleton() const { return false; }
        virtual void pushed() {};
        void rootElement() { idx = 0; }
     public:
        SystemElement(const String& d) :
            parent(NULL), idx(unsigned(-1)), 
            nChildren(0), children(NULL), next(NULL), descr(d) {}
        void reset();
        virtual ~SystemElement() { reset(); }
            
        const String& description() const { return descr; }
        unsigned elementId() const { return idx; }
        virtual String name() const;
        String path() const;

        class Enum {
        public:
            virtual bool process(SystemElement& e) = 0;
        };
        class ConstEnum {
        public:
            virtual bool process(const SystemElement& e) = 0;
        };
        void enumerate(Enum& en);
        void enumerate(ConstEnum& en) const;

        enum Class {
            ClassSystem,
            ClassNIC,
            ClassPort,
            ClassDriver,
            ClassLibrary,
            ClassTool,
            ClassBundle,
            ClassFirmware
        };
        virtual Class classify() const = 0;
        bool isSoftware() const;
        const SystemElement *container() const { return parent; }
        SystemElement *nextComponent() { return next; }
        const SystemElement *nextComponent() const { return next; } 
        SystemElement *firstComponent() { return children; }
        const SystemElement *firstComponent() const { return children; }
        SystemElement *component(unsigned i);
        const SystemElement *component(unsigned i) const;
    };

    inline bool SystemElement::isSoftware() const
    {
        switch (classify())
        {
            case ClassSystem:
            case ClassNIC:
            case ClassPort:
                return false;
            default:
                return true;
        }
    }

    struct VersionInfo {
        unsigned vmajor;
        unsigned vminor;
        unsigned revision;
        unsigned buildNo;
        String versionStr;
        Datetime releaseDate;
        VersionInfo(unsigned mj, unsigned mn, unsigned rev, unsigned bno,
                    const String& vstr, const Datetime& release) :
            vmajor(mj), vminor(mn), revision(rev), buildNo(bno),
            versionStr(vstr), 
            releaseDate(release) {}
        VersionInfo() :
            vmajor(0), vminor(0), revision(unsigned(-1)), 
            buildNo(unsigned(-1)) {}
        VersionInfo(const char *s);
        String buildVersionStr() const;
        static VersionInfo versionFromCmd(const char *cmd);
        bool isInstalled() const;
    };

    inline bool VersionInfo::isInstalled() const
    {
        return (vmajor != 0 || vminor != 0 ||
                (revision != unsigned(-1) && revision != 0) ||
                (buildNo != unsigned(-1) && buildNo != 0) ||
                versionStr.size() > 0 ||
                !(releaseDate == Datetime()));
    }

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
        static void *doThread(void *self);
    protected:
        virtual bool threadProc() = 0;
    public:
        Thread() : state(NotRun) {}
        State currentState() const;
        void start();
        virtual void stop();
    };

    class SWElement : public SystemElement {
        VersionInfo versionInfo;
        String sysname;
    protected:
        virtual VersionInfo detectVersionInfo() const = 0;
        void updateVersionInfo(const VersionInfo& nv) {  versionInfo = nv; }
        virtual bool singleton() const { return true; }
        virtual void pushed() { syncVersion(); }
    public:
        SWElement(const String& d, const String& sn) :
            SystemElement(d), sysname(sn) {}
        const VersionInfo& version() const { return versionInfo; }
        virtual bool install(const char *filename) = 0;
        void syncVersion() { updateVersionInfo(detectVersionInfo()); };
        const String& sysName() const { return sysname; }
    };

    struct MACAddress {
        unsigned char address[6];
    };

    class Port : public SystemElement {
        static const String portDescription;
        static const String portBaseId;
    protected:
        virtual const String& baseId() const { return portBaseId; }
    public:
        Port() : SystemElement(portDescription) {};
        virtual Class classify() const { return ClassPort; }
        bool linkStatus() const;
        void enable(bool status);
        uint64 linkSpeed() const;
        uint64 maxLinkSpeed() const;
        void linkSpeed(uint64 speed);
        String ifName() const;
        MACAddress permanentMAC() const;
        MACAddress currentMAC() const;
        void currentMAC(const MACAddress& mac);
        
    };

    class Firmware : public SWElement {
    public:
        Firmware(const String& d) : SWElement(d, "") {};
        virtual Class classify() const { return ClassFirmware; }
    };

    class NIC;
    class NICFirmware : public Firmware {
        static const String fwBaseId;
        static const String fwDescr;
    protected:
        virtual VersionInfo detectVersionInfo() const;
        virtual const String& baseId() const { return fwBaseId; }
    public:
        NICFirmware() : Firmware(fwDescr) {};
        virtual bool install(const char *file);
    };

    class BootROM : public Firmware {
        static const String romBaseId;
        static const String romDescr;
    protected:
        virtual VersionInfo detectVersionInfo() const;
        virtual const String& baseId() const { return romBaseId; }
    public:
        BootROM() : Firmware(romDescr) {};
        virtual bool install(const char *file);
    };

    struct VitalProductData {
        String uuid;
        String manufacturer;
        String serialNo;
        String partNo;
        String model;
        String fruNumber;
        VitalProductData(const String& id, const String& manf, 
                         const String& sno, const String& pno,
                         const String& m, const String& fru) :
            uuid(id), manufacturer(manf), serialNo(sno), partNo(pno),
            model(m), fruNumber(fru) {}
        VitalProductData() {}
    };

    class NIC : public SystemElement {
        static const String nicDescription;
        static const String nicBaseId;
        VitalProductData vpd;
        void updateVPD();
    protected:
        virtual const String& baseId() const { return nicBaseId; }
        virtual void pushed();
    public:
        NIC() : SystemElement(nicDescription) {};
        virtual Class classify() const { return ClassNIC; }
        const VitalProductData& vitalProductData() const { return vpd; }
        VersionInfo fwVersion() const;
        VersionInfo romVersion() const;
        enum Connector {
            RJ45,
            SFPPlus,
            Mezzanine
        };
        Connector connector() const;
        String manufacturer() const;
        virtual String name() const;
    };

    class Bundle;
    
    class Driver : public SWElement {
    protected:
        virtual VersionInfo detectVersionInfo() const;
    public:
        Driver(const String& d, const String& sn) :
            SWElement(d, sn) {};
        virtual bool install(const char *name);
        virtual Class classify() const { return ClassDriver; }
    };

    class NetDriver : public Driver {
        static const String drvDescription;
        static const String drvBaseId;
        static const String drvName;
    protected:
        virtual const String& baseId() const { return drvBaseId; }
    public:
        NetDriver() : Driver(drvDescription, drvName) {};
    };

    class OnloadDriver : public Driver {
        static const String drvDescription;
        static const String drvBaseId;
        static const String drvName;
    protected:
        virtual const String& baseId() const { return drvBaseId; }
    public:
        OnloadDriver() : Driver(drvDescription, drvName) {};
    };
    
    class Tool : public SWElement {
    protected:
        virtual VersionInfo detectVersionInfo() const;
    public:
        Tool(const String& descr, const String& sn) :
            SWElement(descr, sn) {}
        virtual bool install(const char *name);
        virtual Class classify() const { return ClassTool; }
    };

    class UpdateTool : public Tool {
        static const String toolDescription;
        static const String toolBaseId;
        static const String toolName;
    protected:
        virtual const String& baseId() const { return toolBaseId; }
    public:
        UpdateTool() : Tool(toolDescription, toolName) {};
    };

    class Library : public SWElement {
    protected:
        virtual VersionInfo detectVersionInfo() const;
    public:
        Library(const String& descr, const String& sn) :
            SWElement(descr, sn) {}
        virtual bool install(const char *name);
        virtual Class classify() const { return ClassLibrary; }
    };

    class OnloadLibrary : public Library {
        static const String libDescription;
        static const String libBaseId;
        static const String libName;
    protected:
        virtual const String& baseId() const { return libBaseId; }
    public:
        OnloadLibrary() : Library(libDescription, libName) {}
    };

    class CIMProviderLibrary : public Library {
        static const String libDescription;
        static const String libBaseId;
        static const String libName;
    protected:
        virtual const String& baseId() const { return libBaseId; }
    public:
        CIMProviderLibrary() : Library(libDescription, libName) {}
    };

    class Bundle : public SWElement {
        class BundleUpdater : public Enum {
        public:
            virtual bool process(SystemElement& se);
        };
        virtual VersionInfo detectVersionInfo() const;
    public:
        Bundle(const String& descr, const String& pkg) :
            SWElement(descr, pkg) {}
        virtual bool install(const char *name);
        virtual Class classify() const { return ClassBundle; }
    };

    class KernelBundle : public Bundle {
        static const String kbBaseId;
        static const String kbDescription;
        static const String& kbName();
    protected:
        virtual const String& baseId() const { return kbBaseId; }
        virtual void pushed();
    public:
        KernelBundle() : Bundle(kbDescription, kbName()) {}
    };

    class UserspaceBundle : public Bundle {
        static const String ubBaseId;
        static const String ubDescription;
        static const String& ubName();
    protected:
        virtual const String& baseId() const { return ubBaseId; }
        virtual void pushed();
    public:
        UserspaceBundle() : Bundle(ubDescription, ubName()) {}
    };

    class ManagementBundle : public Bundle {
        static const String mgmtBaseId;
        static const String mgmtDescription;
        static const String& mgmtName();
    protected:
        virtual const String& baseId() const { return mgmtBaseId; }
        virtual void pushed();
    public:
        ManagementBundle() : Bundle(mgmtDescription, mgmtName()) {};
    };

    class System : public SystemElement {
        System(const System&);
        const System& operator = (const System&);
        static const String manfId;
        static const String idPfx;
        static const String systemBaseId;
        static const String systemDescr;
        System() : 
            SystemElement(systemDescr) 
        { 
            rootElement();
            rescan(); 
        }
    protected:
        virtual bool singleton() const { return true; }
        virtual const String& baseId() const { return systemBaseId; }
    public:
        static System target;
        const String& manufacturer() const { return manfId; };
        const String& idPrefix() const { return idPfx; };
        bool is64bit() const;
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
        OSType osType() const;
        virtual Class classify() const { return ClassSystem; }
        void rescan();
    };

}


#endif
