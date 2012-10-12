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

    String SystemElement::name() const
    {
        if (singleton())
            return baseId();
        Buffer result;
        result.appends(baseId().c_str());
        result.append(' ');
        result.append_uint16(elementId());
        return result.data();
    }

    void SystemElement::makePath(Buffer& buf) const
    {
        if (parent)
        {
            parent->makePath(buf);
            buf.append('.');
        }
        buf.append_uint16(elementId());
    }

    String SystemElement::path() const
    {
        Buffer buf;
        makePath(buf);
        return buf.data();
    }

    void SystemElement::reset()
    {
        SystemElement *iter = children;
        SystemElement *n;
        
        while (iter != NULL)
        {
            n = iter->next;
            delete iter;
            iter = n;
        }
        children = NULL;
        nChildren = 0;
    }

    void SystemElement::enumerate(Enum& en)
    {
        CIMPLE_DBG(("enumerating %s", name().c_str()));

        if (!en.process(*this))
            return;
        
        CIMPLE_DBG(("descending %s", name().c_str()));

        for (SystemElement *comp = children; comp != NULL; comp = comp->next)
        {
            comp->enumerate(en);
        }
    }

    void SystemElement::enumerate(ConstEnum& en) const
    {
        CIMPLE_DBG(("enumerating %s", name().c_str()));

        if (!en.process(*this))
            return;

        CIMPLE_DBG(("descending %s", name().c_str()));

        for (const SystemElement *comp = children; comp != NULL; comp = comp->next)
        {
            comp->enumerate(en);
        }
    }

    SystemElement *SystemElement::component(unsigned i)
    {
        for (SystemElement *comp = children; comp != NULL; comp = comp->next)
        {
            if (comp->idx == i)
                return comp;
        }
        return NULL;
    }

    const SystemElement *SystemElement::component(unsigned i) const
    {
        for (const SystemElement *comp = children; comp != NULL; comp = comp->next)
        {
            if (comp->idx == i)
                return comp;
        }
        return NULL;
    }

    void SystemElement::push(SystemElement *comp)
    {
        comp->parent = this;
        comp->next = children;
        comp->idx = nChildren;
        nChildren++;
        children = comp;
        comp->pushed();
    }

    VersionInfo::VersionInfo(const char *s) :
        vmajor(0), vminor(0), revision(unsigned(-1)), buildNo(unsigned(-1))
    {
        char *s0 = const_cast<char *>(s);
        vmajor = strtoul(s0, &s0, 10);
        if (*s0 == '.')
        {
            vminor = strtoul(s0 + 1, &s0, 10);
            if (*s0 == '.' || *s0 == '-')
            {
                revision = strtoul(s0 + 1, &s0, 10);
                if (*s0 == '.' || *s0 == '-')
                    buildNo = strtoul(s0 + 1, NULL, 10);
            }
        }
        versionStr = String(s);
    }
    

    String VersionInfo::buildVersionStr() const
    {
        if (versionStr.size() > 0)
            return versionStr;
        
        Buffer result;
        
        result.append_uint32(vmajor);
        result.append('.');
        result.append_uint32(vminor);
        if (revision != unsigned(-1))
        {
            result.append('.');
            result.append_uint32(revision);
        }
        if (buildNo != unsigned(-1))
        {
            result.append('.');
            result.append_uint32(buildNo);
        }
        return result.data();
    }


    VersionInfo VersionInfo::versionFromCmd(const char *cmd)
    {
        FILE *p = popen(cmd, "r");
        if (p != NULL)
        {
            char buf[128] = "";
            fgets(buf, sizeof(buf), p);
            pclose(p);
            return VersionInfo(buf);
        }
        return VersionInfo();
    }


    const String Port::portBaseId("NIC Port");
    const String Port::portDescription("NIC Ethernet Port");

    bool Port::linkStatus() const
    {
        return false;
    }
    
    void Port::enable(bool status)
    {
    }
    
    uint64 Port::linkSpeed() const
    {
        return uint64(10) * 1024 * 1024 * 1024;
    }
    
    uint64 Port::maxLinkSpeed() const
    {
        return uint64(10) * 1024 * 1024 * 1024;
    }
    
    void Port::linkSpeed(uint64 speed)
    {
    }
    
    String Port::ifName() const
    {
        Buffer buf;
        buf.appends("eth");
        buf.append_uint64(8 + elementId());
        return buf.data();
    }
    
    MACAddress Port::permanentMAC() const
    {
        MACAddress mac = {{0x00, 0x0f, 0x53, 0x01, 0x39, 0x7c}};
        mac.address[5] += elementId();
        return mac;
    }
    
    MACAddress Port::currentMAC() const
    {
        return permanentMAC();
    }
    
    void Port::currentMAC(const MACAddress& mac)
    {
    }

    const String NICFirmware::fwBaseId("NIC Firmware");
    const String NICFirmware::fwDescr("NIC MC Firmware");

    bool NICFirmware::install(const char *)
    {
        return false;
    }

    VersionInfo NICFirmware::detectVersionInfo() const
    {
        return static_cast<const NIC *>(container())->fwVersion();
    }

    const String BootROM::romBaseId("NIC BootROM");
    const String BootROM::romDescr("NIC BootROM image");

    bool BootROM::install(const char *)
    {
        return false;
    }

    VersionInfo BootROM::detectVersionInfo() const
    {
        return static_cast<const NIC *>(container())->romVersion();
    }

    const String NIC::nicBaseId("NIC");
    const String NIC::nicDescription("Solarflare NIC instance");

    void NIC::pushed()
    {
        CIMPLE_DBG(("NIC record created"));
        updateVPD();
        push(new Port);
        push(new Port);
        push(new BootROM);
        push(new NICFirmware);
    }

    VersionInfo NIC::romVersion() const
    {
        return VersionInfo(1, 2, 3, 5, "", Datetime::now());
    }

    VersionInfo NIC::fwVersion() const
    {
        return VersionInfo(1, 2, 3, 4, "", Datetime::now());
    }

    void NIC::updateVPD()
    {
        vpd = VitalProductData("129821821821812", "Solarflare",
                               "1122221122", "23232332", "SFC9000A", "212112121");
    }

    NIC::Connector NIC::connector() const
    {
        return RJ45;
    }

    String NIC::manufacturer() const
    {
        String m = vitalProductData().manufacturer;
        if (m.size() > 0)
            return m;
        return System::target.manufacturer();
    }

    String NIC::name() const
    {
        String result = manufacturer();
        result.append(" Ethernet Adapter ");
        result.append(vitalProductData().serialNo);

        return result;
    }

#if defined(linux)
    VersionInfo Driver::detectVersionInfo() const
    {
        Buffer cmd;

        cmd.appends("/sbin/modinfo ");
        cmd.appends(sysName().c_str());
        cmd.appends(" 2>/dev/null | grep ^version: | cut -f2 -d:");
        return VersionInfo::versionFromCmd(cmd.data());
    }
#endif

    bool Driver::install(const char *)
    {
        return false;
    }

    const String NetDriver::drvDescription = "Solarflare NIC Net driver";
    const String NetDriver::drvBaseId = "Net Driver";
#if defined(linux)
    const String NetDriver::drvName = "sfc";
#endif

    const String OnloadDriver::drvDescription = "Solarflare OpenOnload driver";
    const String OnloadDriver::drvBaseId = "Onload Driver";
#if defined(linux)
    const String OnloadDriver::drvName = "onload";
#endif

#if defined(linux)
    VersionInfo Tool::detectVersionInfo() const
    {
        Buffer cmd;

        cmd.appends(sysName().c_str());
        cmd.appends(" --version 2>&1");
        return VersionInfo::versionFromCmd(cmd.data());
    }
#endif

    bool Tool::install(const char *)
    {
        return false;
    }

    const String UpdateTool::toolDescription = "Solarflare Firmware Update Tool";
    const String UpdateTool::toolBaseId = "Firmware Update Tool";
#if defined(linux)
    const String UpdateTool::toolName = "/usr/sbin/sfupdate";
#endif


#if defined(linux)
    VersionInfo Library::detectVersionInfo() const
    {
        char link[256] = "";
        readlink(sysName().c_str(), link, sizeof(link));
        const char *so = strstr(link, ".so.");
        if (so == NULL)
            return VersionInfo();
        return VersionInfo(so + 4);
    }
#endif

    bool Library::install(const char *)
    {
        return false;
    }

    const String OnloadLibrary::libDescription = "Solarflare OpenOnload Transport Library";
    const String OnloadLibrary::libBaseId = "Onload Transport Library";
    const String OnloadLibrary::libName = "/usr/lib/libtransport.so";

    const String CIMProviderLibrary::libDescription = "Solarflare CIM Provider";
    const String CIMProviderLibrary::libBaseId = "CIM Provider";
    const String CIMProviderLibrary::libName = "/opt/solarlare/cim/libSolarflare.so";

    VersionInfo Bundle::detectVersionInfo() const
    {
        Buffer cmd;
        switch (System::target.osType())
        {
            case System::RHEL:
            case System::SLES:
            case System::CentOS:
            case System::OracleEL:
                cmd.appends("rpm -q ");
                cmd.appends(sysName().c_str());
                cmd.appends(" 2>/dev/null | awk -vFS=- '{ print $(NF - 1) }'");
                return VersionInfo::versionFromCmd(cmd.data());
            case System::Debian:
                cmd.appends("dpkg -s ");
                cmd.appends(sysName().c_str());
                cmd.appends(" 2>/dev/null | grep ^Version: | cut -d: -f2");
                return VersionInfo::versionFromCmd(cmd.data());
            default:
                return VersionInfo();
        }
    }

    bool Bundle::BundleUpdater::process(SystemElement& se)
    {
        if (!se.isSoftware())
            return false;
        static_cast<SWElement *>(&se)->syncVersion();
        return true;
    }
    

    bool Bundle::install(const char *name)
    {
        Buffer cmd;
        switch (System::target.osType())
        {
            case System::RHEL:
            case System::SLES:
            case System::CentOS:
            case System::OracleEL:
                cmd.appends("rpm -i ");
                cmd.appends(name);
                break;
            case System::Debian:
                cmd.appends("dpkg -i ");
                cmd.appends(name);
                break;
            default:
                return false;
        }
        if (system(cmd.data()) != 0)
            return false;
        BundleUpdater updater;
        enumerate(updater);
        return true;
    }

    const String KernelBundle::kbBaseId = "Kernel OS Package";
    const String KernelBundle::kbDescription = "Solarflare Kernel drivers";
    const String& KernelBundle::kbName()
    {
        static const String pkg = "sfc";
        return pkg;
    }

    void KernelBundle::pushed()
    {
        Bundle::pushed();
        push(new NetDriver);
        push(new OnloadDriver);
    }

    const String UserspaceBundle::ubBaseId = "Userspace OS Package";
    const String UserspaceBundle::ubDescription = "Solarflare userspace libraries & utilities";
    const String& UserspaceBundle::ubName()
    {
        static const String pkg = "onload";
        return pkg;
    }

    void UserspaceBundle::pushed()
    {
        Bundle::pushed();
        push(new OnloadLibrary);
        push(new UpdateTool);
    }

    const String ManagementBundle::mgmtBaseId = "Management Package";
    const String ManagementBundle::mgmtDescription = "Solarflare CIM provider package";
    const String& ManagementBundle::mgmtName()
    {
        static const String pkg = "sfc_cim";
        return pkg;
    }

    void ManagementBundle::pushed()
    {
        Bundle::pushed();
        push(new CIMProviderLibrary);
    }

    const String System::manfId = "Solarflare Inc.";
    const String System::idPfx = "Solarflare";
    const String System::systemBaseId = "Solarflare System";
    const String System::systemDescr = "Solarflare-enabled host";

    void System::rescan()
    {
        reset();
        CIMPLE_DBG(("scanning the system"));
        push(new NIC);
        push(new KernelBundle);
        push(new UserspaceBundle);
        push(new ManagementBundle);
    }

    System System::target;

    bool System::is64bit() const
    {
#ifdef i386
        return false;
#else
        return true;
#endif
    }
    
    System::OSType System::osType() const
    {
#if defined(linux)
        if (access("/etc/debian_version", F_OK) == 0)
            return Debian;
        else if (access("/etc/redhat-release", F_OK) == 0)
            return RHEL;
        else
            return GenericLinux;
#elif defined(WIN32)
        return WindowsServer2003;
#else
        return VMWareESXi;
#endif
    }
}
