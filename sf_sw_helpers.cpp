#include "sf_provider.h"
#include "SF_SoftwareIdentity.h"
#include "SF_SoftwareInstallationService.h"
#include "SF_BundleComponent.h"
#include "SF_ConcreteJob.h"
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
    using cimple::SF_SoftwareInstallationService;
    using cimple::CIM_OperatingSystem;
    using cimple::SF_ConcreteJob;
    using cimple::SF_BundleComponent;
    using cimple::CIM_ManagedElement;

    class SoftwareIdentityHelper : public CIMHelper {
    public:
        virtual Instance *reference(const SystemElement& obj) const;
        virtual Instance *instance(const SystemElement&) const;
        virtual bool match(const SystemElement& obj, const Instance& inst) const;
    };

    class SoftwareInstallationServiceHelper : public CIMHelper {
    public:
        virtual Instance *reference(const SystemElement& obj) const;
        virtual Instance *instance(const SystemElement&) const;
        virtual bool match(const SystemElement& obj, const Instance& inst) const;
    };

    class InstallationJobHelper : public ConcreteJobAbstractHelper {
    protected:
        virtual const char *threadSuffix() const 
        {
            return "installThread";
        }
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

    class BundleComponentHelper : public CIMHelper {
    public:
        virtual Instance *instance(const SystemElement&) const;
    };

    const CIMHelper* HostSWElement::cimDispatch(const Meta_Class& cls) const
    {
        static const HostSoftwareIdentityHelper hostSoftwareIdentity;
        static const BundleComponentHelper bundleComponent;
        if (&cls == &SF_SoftwareIdentity::static_meta_class)
            return &hostSoftwareIdentity;
        else if (&cls == &SF_BundleComponent::static_meta_class)
            return &bundleComponent;
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

    class FindIndex : public ConstElementEnumerator {
        unsigned idx;
        const SystemElement *target;
    public:
        FindIndex(const SystemElement *what) : idx(0), target(what) {};
        unsigned found() const { return idx; }
        virtual bool process(const SystemElement& se);
    };

    bool FindIndex::process(const SystemElement& se)
    {
        if (&se == target)
            return false;
        idx++;
        return true;
    }

    Instance *
    BundleComponentHelper::instance(const solarflare::SystemElement& se) const
    {
        const solarflare::HostSWElement& he = static_cast<const solarflare::HostSWElement&>(se);
        
        SF_BundleComponent *link = SF_BundleComponent::create(true);
        link->GroupComponent = 
        cast<CIM_ManagedElement *>(he.package()->cimReference(SF_SoftwareIdentity::static_meta_class));
        link->PartComponent =
        cast<CIM_ManagedElement *>(he.cimReference(SF_SoftwareIdentity::static_meta_class));
        FindIndex finder(&se);
        he.package()->forAllSoftware(finder);
        link->AssignedSequence.set(finder.found());
        
        return link;
    }

    class BundleSoftwareIdentityHelper : public HostSoftwareIdentityHelper {
        static void addPackageType(SF_SoftwareIdentity *identity, Package::PkgType type);
    public:
        virtual Instance *instance(const SystemElement& se) const;
    };


    const CIMHelper* Package::cimDispatch(const Meta_Class& cls) const
    {
        static const BundleSoftwareIdentityHelper bundleSoftwareIdentity;
        static const SoftwareInstallationServiceHelper bundleInstallation;
        static const InstallationJobHelper bundleInstallationJob;

        if (&cls == &SF_SoftwareInstallationService::static_meta_class)
            return &bundleInstallation;
        if (&cls == &SF_SoftwareIdentity::static_meta_class)
            return &bundleSoftwareIdentity;
        if (&cls == &SF_ConcreteJob::static_meta_class && 
            const_cast<Package *>(this)->installThread() != NULL)
            return &bundleInstallationJob;
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

    const CIMHelper* Firmware::cimDispatch(const Meta_Class& cls) const
    {
        static const SoftwareInstallationServiceHelper firmwareInstallation;
        static const InstallationJobHelper installationJob;

        if (&cls == &SF_SoftwareInstallationService::static_meta_class)
            return &firmwareInstallation;
        if (&cls == &SF_ConcreteJob::static_meta_class)
            return &installationJob;
        else
            return SWElement::cimDispatch(cls);
    }


    Instance *SoftwareInstallationServiceHelper::reference(const SystemElement& se) const
    {
        const CIM_ComputerSystem *system = findSystem();
        SF_SoftwareInstallationService *newSvc = SF_SoftwareInstallationService::create(true);
        
        newSvc->CreationClassName.set("SF_SoftwareInstallationService");
        newSvc->Name.set(se.name());
        newSvc->SystemCreationClassName.set(system->CreationClassName.value);
        newSvc->SystemName.set(system->Name.value);
        return newSvc;
    }


    Instance *
    SoftwareInstallationServiceHelper::instance(const SystemElement& se) const
    {
        const SWElement& sw = static_cast<const SWElement&>(se);
    
        SF_SoftwareInstallationService *newSvc = static_cast<SF_SoftwareInstallationService *>(reference(sw));

        newSvc->Description.set(sw.description());
        newSvc->ElementName.set(sw.name());
        newSvc->InstanceID.set(instanceID(sw.name()));
        newSvc->InstanceID.value.append(" Installer");
        newSvc->OperationalStatus.null = false;
        newSvc->OperationalStatus.value.append(SF_SoftwareInstallationService::_OperationalStatus::enum_OK);
        newSvc->OperatingStatus.null = false;
        newSvc->OperatingStatus.value = SF_SoftwareInstallationService::_OperatingStatus::enum_Servicing;
        newSvc->PrimaryStatus.null = false;
        newSvc->PrimaryStatus.value = SF_SoftwareInstallationService::_PrimaryStatus::enum_OK;
        newSvc->EnabledState.null = false;
        newSvc->EnabledState.value = SF_SoftwareInstallationService::_EnabledState::enum_Enabled;

        return newSvc;
    }

    bool
    SoftwareInstallationServiceHelper::match(const SystemElement& se,
                                             const Instance& inst) const
    {
        const cimple::CIM_SoftwareInstallationService *svc = cast<const cimple::CIM_SoftwareInstallationService *>(&inst);
        
        if (svc == NULL)
            return false;


        if (svc->CreationClassName.null || svc->Name.null || 
            svc->CreationClassName.value != "SF_SoftwareInstallationService" ||
            svc->SystemCreationClassName.null || svc->SystemName.null)
            return false;
        
        if (!isOurSystem(svc->SystemCreationClassName.value, svc->SystemName.value))
            return false;

        return svc->Name.value == se.name();
    }
} // namespace
