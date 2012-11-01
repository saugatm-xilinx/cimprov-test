#include "sf_provider.h"
#include "SF_SoftwareIdentity.h"
#include "CIM_OperatingSystem.h"

namespace solarflare 
{
    using cimple::Instance;
    using cimple::Meta_Class;
    using cimple::CIM_ComputerSystem;
    using cimple::Ref;
    using cimple::cast;
    using cimple::is_a;
    using cimple::SF_SoftwareIdentity;
    using cimple::CIM_OperatingSystem;

    class SoftwareIdentityHelper : public CIMHelper {
    public:
        virtual Instance *reference(const SystemElement& obj) const;
        virtual Instance *instance(const SystemElement&) const;
        virtual bool match(const SystemElement& obj, const Instance& inst) const;
    };

    const CIMHelper* SWElement::cimDispatch(const Meta_Class& cls) const
    {
        static const SoftwareIdentityHelper softwareIdentity;
        if (&cls == &SF_SoftwareIdentity::static_meta_class)
            return &softwareIdentity;
        return NULL;
    }


    Instance *
    SoftwareIdentityHelper::reference(const solarflare::SystemElement& se) const
    {
        SF_SoftwareIdentity *identity = SF_SoftwareIdentity::create(true);
        identity->InstanceID.set(instanceID(se.name()));
        return identity;
    }

    Instance *
    SoftwareIdentityHelper::instance(const SystemElement& se) const
    {
        const SWElement& sw = static_cast<const SWElement&>(se);
        
        SF_SoftwareIdentity *identity = static_cast<SF_SoftwareIdentity *>(reference(sw));
        static const unsigned classmap[] = {
            SF_SoftwareIdentity::_Classifications::enum_Unknown,
            SF_SoftwareIdentity::_Classifications::enum_Unknown,
            SF_SoftwareIdentity::_Classifications::enum_Unknown,
            SF_SoftwareIdentity::_Classifications::enum_Driver,
            SF_SoftwareIdentity::_Classifications::enum_Middleware,
            SF_SoftwareIdentity::_Classifications::enum_Configuration_Software,
            SF_SoftwareIdentity::_Classifications::enum_Software_Bundle,
            SF_SoftwareIdentity::_Classifications::enum_Firmware
        };
        
        identity->IsEntity.set(true);
        identity->ElementName.set(sw.sysName());
        identity->Name.set(se.name());
        identity->MajorVersion.set(sw.version().major());
        identity->MinorVersion.set(sw.version().minor());
        if (sw.version().revision() != VersionInfo::unknown)
            identity->RevisionNumber.set(sw.version().revision());
        if (sw.version().build() != VersionInfo::unknown)
        {
            identity->LargeBuildNumber.set(sw.version().build());
            identity->IsLargeBuildNumber.set(true);
        }
        identity->VersionString.set(sw.version().string());
        identity->ReleaseDate.set(sw.version().releaseDate());
        identity->Description.set(sw.description());

        identity->Manufacturer.set(System::target.manufacturer());

        identity->IdentityInfoType.null = false;
        identity->IdentityInfoType.value.append("SoftwareID");
        identity->IdentityInfoType.value.append("SoftwareStatus");
        
        identity->IdentityInfoValue.null = false;
        identity->IdentityInfoValue.value.append(sw.name());
        identity->IdentityInfoValue.value.append("Default");    
        
        identity->Classifications.null = false;
        identity->Classifications.value.append(classmap[sw.classify()]);
    
        return identity;
    }

    bool
    SoftwareIdentityHelper::match(const solarflare::SystemElement& se, const Instance& inst) const
    {
        if (!is_a<cimple::CIM_SoftwareIdentity>(&inst))
            return false;
        const cimple::Property<String>& id = static_cast<const cimple::CIM_SoftwareIdentity&>(inst).InstanceID;
        return !id.null && instanceID(se.name()) == id.value;
    }


    class HostSoftwareIdentityHelper : public SoftwareIdentityHelper {
        static void addTargetOS(SF_SoftwareIdentity *id);
    public:
        virtual Instance *instance(const SystemElement&) const;
    };

    const CIMHelper* HostSWElement::cimDispatch(const Meta_Class& cls) const
    {
        static const HostSoftwareIdentityHelper hostSoftwareIdentity;
        if (&cls == &SF_SoftwareIdentity::static_meta_class)
            return &hostSoftwareIdentity;
        else
            return SWElement::cimDispatch(cls);
    }

    void HostSoftwareIdentityHelper::addTargetOS(SF_SoftwareIdentity *identity)
    {
        solarflare::System::OSType type = System::target.osType();
        bool is64 = System::target.is64bit();
        unsigned cimType[][2] = {
            {
                CIM_OperatingSystem::_OSType::enum_Microsoft_Windows_Server_2003, 
                CIM_OperatingSystem::_OSType::enum_Microsoft_Windows_Server_2003_64_Bit
            },
            {
                CIM_OperatingSystem::_OSType::enum_Microsoft_Windows_Server_2008, 
                CIM_OperatingSystem::_OSType::enum_Microsoft_Windows_Server_2008_64_Bit
            },
            {
                CIM_OperatingSystem::_OSType::enum_Microsoft_Windows_Server_2008_R2, 
                CIM_OperatingSystem::_OSType::enum_Microsoft_Windows_Server_2008_R2
            },
            {
                CIM_OperatingSystem::_OSType::enum_RedHat_Enterprise_Linux,
                CIM_OperatingSystem::_OSType::enum_RedHat_Enterprise_Linux_64_Bit
            },
            {
                CIM_OperatingSystem::_OSType::enum_SLES,
                CIM_OperatingSystem::_OSType::enum_SLES_64_Bit
            },
            {
                CIM_OperatingSystem::_OSType::enum_Debian,
                CIM_OperatingSystem::_OSType::enum_Debian_64_Bit
            },
            {
                CIM_OperatingSystem::_OSType::enum_CentOS_32_bit,
                CIM_OperatingSystem::_OSType::enum_CentOS_64_bit
            },
            {
                CIM_OperatingSystem::_OSType::enum_Oracle_Enterprise_Linux_32_bit,
                CIM_OperatingSystem::_OSType::enum_Oracle_Enterprise_Linux_64_bit
            },
            {
                CIM_OperatingSystem::_OSType::enum_Linux_2_6_x,
                CIM_OperatingSystem::_OSType::enum_Linux_2_6_x_64_Bit
            },
            {
                CIM_OperatingSystem::_OSType::enum_VMware_ESXi,
                CIM_OperatingSystem::_OSType::enum_VMware_ESXi,
            }
        };
        identity->TargetOSTypes.null = false;
        identity->TargetOSTypes.value.append(cimType[type][is64]);
    }
    
    Instance *
    HostSoftwareIdentityHelper::instance(const SystemElement& se) const
    {
        SF_SoftwareIdentity *id = static_cast<SF_SoftwareIdentity *>(SoftwareIdentityHelper::instance(se));
        addTargetOS(id);
        return id;
    }

    class BundleSoftwareIdentityHelper : public HostSoftwareIdentityHelper {
        static void addPackageType(SF_SoftwareIdentity *identity, Package::PkgType type);
    public:
        virtual Instance *instance(const SystemElement& se) const;
    };


    const CIMHelper* Package::cimDispatch(const Meta_Class& cls) const
    {
        static const BundleSoftwareIdentityHelper bundleSoftwareIdentity;
        if (&cls == &SF_SoftwareIdentity::static_meta_class)
            return &bundleSoftwareIdentity;
        else
            return SWElement::cimDispatch(cls);
    }

    void BundleSoftwareIdentityHelper::addPackageType(SF_SoftwareIdentity *identity,
                                                      Package::PkgType type)
    {
        unsigned pkgType = 0;
        
        switch (type)
        {
            case Package::Deb:
                pkgType = SF_SoftwareIdentity::_ExtendedResourceType::enum_Debian_linux_Package;
                break;
            case Package::RPM:
                pkgType = SF_SoftwareIdentity::_ExtendedResourceType::enum_Linux_RPM;
                break;
            case Package::MSI:
                pkgType = SF_SoftwareIdentity::_ExtendedResourceType::enum_Windows_MSI;
                break;
            case Package::VSphereBundle:
                pkgType = SF_SoftwareIdentity::_ExtendedResourceType::enum_VMware_vSphere_Installation_Bundle;
                break;
            case Package::Tarball:
                pkgType = SF_SoftwareIdentity::_ExtendedResourceType::enum_Other;
                identity->OtherExtendedResourceTypeDescription.set("TAR archive");
                break;
            default:
                pkgType = SF_SoftwareIdentity::_ExtendedResourceType::enum_Unknown;
                break;
        }
        if (pkgType != 0)
        {
            identity->ExtendedResourceType.null = false;
            identity->ExtendedResourceType.value = pkgType;
        }
    }

    Instance *
    BundleSoftwareIdentityHelper::instance(const SystemElement& se) const
    {
        SF_SoftwareIdentity *id = static_cast<SF_SoftwareIdentity *>(HostSoftwareIdentityHelper::instance(se));
        addPackageType(id, static_cast<const Package&>(se).type());
        return id;
    }



} // namespace
