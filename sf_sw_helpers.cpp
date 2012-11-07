#include "sf_provider.h"
#include "SF_SoftwareIdentity.h"
#include "SF_SoftwareInstallationService.h"
#include "SF_SoftwareInstallationServiceCapabilities.h"
#include "SF_BundleComponent.h"
#include "SF_ConcreteJob.h"
#include "CIM_OperatingSystem.h"
#include "SF_ElementConformsToProfile.h"

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
    using cimple::SF_SoftwareInstallationServiceCapabilities;
    using cimple::CIM_OperatingSystem;
    using cimple::SF_ConcreteJob;
    using cimple::SF_BundleComponent;
    using cimple::CIM_ManagedElement;
    using cimple::SF_ElementConformsToProfile;

    class SoftwareIdentityHelper : public CIMHelper {
    public:
        virtual Instance *reference(const SystemElement& obj, unsigned idx) const;
        virtual Instance *instance(const SystemElement&, unsigned idx) const;
        virtual bool match(const SystemElement& obj, const Instance& inst, unsigned idx) const;
    };

    class SoftwareInstallationServiceHelper : public CIMHelper {
    public:
        virtual Instance *reference(const SystemElement& obj, unsigned idx) const;
        virtual Instance *instance(const SystemElement&, unsigned idx) const;
        virtual bool match(const SystemElement& obj, const Instance& inst, unsigned idx) const;
    };

    class SoftwareInstallationServiceCapsHelper : public CIMHelper {
    protected:
        virtual const char *targetType() const = 0;
    public:
        virtual Instance *reference(const SystemElement& obj, unsigned idx) const;
        virtual Instance *instance(const SystemElement&, unsigned idx) const;
    };

    class FwSoftwareInstallationServiceCapsHelper : public SoftwareInstallationServiceCapsHelper {
    protected:
        virtual const char *targetType() const
        {
            return "NIC";
        }
    public:
        virtual Instance *instance(const SystemElement&, unsigned idx) const;
    };

    class BundleSoftwareInstallationServiceCapsHelper : public SoftwareInstallationServiceCapsHelper {
    protected:
        virtual const char *targetType() const
        {
            return "Host";
        }
    public:
        virtual Instance *instance(const SystemElement&, unsigned idx) const;
    };

    class InstallationJobHelper : public ConcreteJobAbstractHelper {
    protected:
        virtual const char *threadSuffix() const 
        {
            return "installThread";
        }
    };

    class SWConformsToProfileHelper : public CIMHelper {
    public:
        virtual Instance *instance(const SystemElement& se, unsigned idx) const;
    };

    class InstallableConformsToProfileHelper : public SWConformsToProfileHelper {
    public:
        virtual unsigned nObjects(const SystemElement& se) const;
        virtual Instance *instance(const SystemElement& se, unsigned idx) const;
    };

    const CIMHelper* SWElement::cimDispatch(const Meta_Class& cls) const
    {
        static const SoftwareIdentityHelper softwareIdentity;
        static const SWConformsToProfileHelper conformToProfile;
        if (&cls == &SF_SoftwareIdentity::static_meta_class)
            return &softwareIdentity;
        if (&cls == &SF_ElementConformsToProfile::static_meta_class)
            return &conformToProfile;
        return NULL;
    }


    Instance *
    SoftwareIdentityHelper::reference(const solarflare::SystemElement& se, unsigned) const
    {
        SF_SoftwareIdentity *identity = SF_SoftwareIdentity::create(true);
        identity->InstanceID.set(instanceID(se.name()));
        return identity;
    }

    Instance *
    SoftwareIdentityHelper::instance(const SystemElement& se, unsigned idx) const
    {
        const SWElement& sw = static_cast<const SWElement&>(se);
        
        SF_SoftwareIdentity *identity = static_cast<SF_SoftwareIdentity *>(reference(sw, idx));
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
    SoftwareIdentityHelper::match(const solarflare::SystemElement& se, const Instance& inst, unsigned) const
    {
        if (!is_a<cimple::CIM_SoftwareIdentity>(&inst))
            return false;
        const cimple::Property<String>& id = static_cast<const cimple::CIM_SoftwareIdentity&>(inst).InstanceID;
        return !id.null && instanceID(se.name()) == id.value;
    }


    class HostSoftwareIdentityHelper : public SoftwareIdentityHelper {
        static void addTargetOS(SF_SoftwareIdentity *id);
    public:
        virtual Instance *instance(const SystemElement&, unsigned idx) const;
    };

    class BundleComponentHelper : public CIMHelper {
    public:
        virtual Instance *instance(const SystemElement&, unsigned idx) const;
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
    HostSoftwareIdentityHelper::instance(const SystemElement& se, unsigned idx) const
    {
        SF_SoftwareIdentity *id = static_cast<SF_SoftwareIdentity *>(SoftwareIdentityHelper::instance(se, idx));
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
    BundleComponentHelper::instance(const solarflare::SystemElement& se, unsigned) const
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
        virtual Instance *instance(const SystemElement& se, unsigned idx) const;
    };


    const CIMHelper* Package::cimDispatch(const Meta_Class& cls) const
    {
        static const BundleSoftwareIdentityHelper bundleSoftwareIdentity;
        static const SoftwareInstallationServiceHelper bundleInstallation;
        static const BundleSoftwareInstallationServiceCapsHelper bundleInstallationCaps;
        static const InstallationJobHelper bundleInstallationJob;
        static const InstallableConformsToProfileHelper conformToProfile;

        if (&cls == &SF_SoftwareInstallationService::static_meta_class)
            return &bundleInstallation;
        if (&cls == &SF_SoftwareInstallationServiceCapabilities::static_meta_class)
            return &bundleInstallationCaps;
        if (&cls == &SF_SoftwareIdentity::static_meta_class)
            return &bundleSoftwareIdentity;
        if (&cls == &SF_ConcreteJob::static_meta_class && 
            const_cast<Package *>(this)->installThread() != NULL)
            return &bundleInstallationJob;
        if (&cls == &SF_ElementConformsToProfile::static_meta_class)
            return &conformToProfile;
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
    BundleSoftwareIdentityHelper::instance(const SystemElement& se, unsigned idx) const
    {
        SF_SoftwareIdentity *id = static_cast<SF_SoftwareIdentity *>(HostSoftwareIdentityHelper::instance(se, idx));
        addPackageType(id, static_cast<const Package&>(se).type());
        return id;
    }

    const CIMHelper* Firmware::cimDispatch(const Meta_Class& cls) const
    {
        static const SoftwareInstallationServiceHelper firmwareInstallation;
        static const FwSoftwareInstallationServiceCapsHelper firmwareInstallationCaps;
        static const InstallationJobHelper installationJob;
        static const InstallableConformsToProfileHelper conformToProfile;

        if (&cls == &SF_SoftwareInstallationService::static_meta_class)
            return &firmwareInstallation;
        if (&cls == &SF_SoftwareInstallationServiceCapabilities::static_meta_class)
            return &firmwareInstallationCaps;
        if (&cls == &SF_ConcreteJob::static_meta_class)
            return &installationJob;
        if (&cls == &SF_ElementConformsToProfile::static_meta_class)
            return &conformToProfile;
        else
            return SWElement::cimDispatch(cls);
    }


    Instance *SoftwareInstallationServiceHelper::reference(const SystemElement& se, unsigned idx) const
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
    SoftwareInstallationServiceHelper::instance(const SystemElement& se, unsigned idx) const
    {
        const SWElement& sw = static_cast<const SWElement&>(se);
    
        SF_SoftwareInstallationService *newSvc = static_cast<SF_SoftwareInstallationService *>(reference(sw, idx));

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
                                             const Instance& inst, unsigned) const
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

    
    Instance *
    SoftwareInstallationServiceCapsHelper::reference(const SystemElement& se, unsigned) const
    {
        SF_SoftwareInstallationServiceCapabilities *newSvc = SF_SoftwareInstallationServiceCapabilities::create(true);
    
        newSvc->InstanceID.set(instanceID(se.name()));
        newSvc->InstanceID.value.append(" Installation Capabilities");
        return newSvc;
    }

    
    Instance *
    SoftwareInstallationServiceCapsHelper::instance(const SystemElement& se, unsigned idx) const
    {
        const SWElement& sw = static_cast<const solarflare::SWElement&>(se);
        SF_SoftwareInstallationServiceCapabilities *newSvc = 
        static_cast<SF_SoftwareInstallationServiceCapabilities *>(reference(sw, idx));

        newSvc->SupportedAsynchronousActions.null = false;
        newSvc->SupportedAsynchronousActions.value.append(SF_SoftwareInstallationServiceCapabilities::_SupportedAsynchronousActions::enum_Install_from_URI);
        newSvc->SupportedSynchronousActions.null = false;
        newSvc->SupportedSynchronousActions.value.append(SF_SoftwareInstallationServiceCapabilities::_SupportedSynchronousActions::enum_Install_from_URI);
        newSvc->SupportedTargetTypes.null = false;
        newSvc->SupportedTargetTypes.value.append(targetType());

        static const cimple::uint16 supported[] = 
        {
            SF_SoftwareInstallationServiceCapabilities::_SupportedInstallOptions::enum_Update,
            SF_SoftwareInstallationServiceCapabilities::_SupportedInstallOptions::enum_Repair,
            SF_SoftwareInstallationServiceCapabilities::_SupportedInstallOptions::enum_Force_installation,
            SF_SoftwareInstallationServiceCapabilities::_SupportedInstallOptions::enum_Defer_target_system_reset,
            SF_SoftwareInstallationServiceCapabilities::_SupportedInstallOptions::enum_Log,
            SF_SoftwareInstallationServiceCapabilities::_SupportedInstallOptions::enum_SilentMode,
            SF_SoftwareInstallationServiceCapabilities::_SupportedInstallOptions::enum_AdministrativeMode
        };
        newSvc->SupportedInstallOptions.null = false;
        newSvc->SupportedInstallOptions.value.append(supported, sizeof(supported) / sizeof(*supported));
        
        newSvc->CanAddToCollection.set(false);
        
        newSvc->SupportedURISchemes.null = false;
        newSvc->SupportedURISchemes.value.append(SF_SoftwareInstallationServiceCapabilities::_SupportedURISchemes::enum_file);
        

        return newSvc;
    }

    Instance *
    FwSoftwareInstallationServiceCapsHelper::instance(const SystemElement& se, unsigned idx) const
    {
        SF_SoftwareInstallationServiceCapabilities *newSvc = 
        static_cast<SF_SoftwareInstallationServiceCapabilities *>(SoftwareInstallationServiceCapsHelper::instance(se, idx));
        newSvc->SupportedExtendedResourceTypes.null = false;
        newSvc->SupportedExtendedResourceTypes.value.append(SF_SoftwareInstallationServiceCapabilities::_SupportedExtendedResourceTypes::enum_Other);
        newSvc->OtherSupportedExtendedResourceTypeDescriptions.null = false;
        newSvc->OtherSupportedExtendedResourceTypeDescriptions.value.append("Firmware image");
        return newSvc;
    }
    
    Instance *
    BundleSoftwareInstallationServiceCapsHelper::instance(const SystemElement& se, unsigned idx) const
    {
        SF_SoftwareInstallationServiceCapabilities *newSvc = 
        static_cast<SF_SoftwareInstallationServiceCapabilities *>(SoftwareInstallationServiceCapsHelper::instance(se, idx));
        const solarflare::Package& pkg = static_cast<const solarflare::Package&>(se);
        unsigned pkgType = 0;

        switch (pkg.type())
        {
            case solarflare::Package::Deb:
                pkgType = SF_SoftwareInstallationServiceCapabilities::_SupportedExtendedResourceTypes::enum_Debian_linux_Package;
                break;
            case solarflare::Package::RPM:
                pkgType = SF_SoftwareInstallationServiceCapabilities::_SupportedExtendedResourceTypes::enum_Linux_RPM;
                break;
            case solarflare::Package::MSI:
                pkgType = SF_SoftwareInstallationServiceCapabilities::_SupportedExtendedResourceTypes::enum_Windows_MSI;
                break;
            case solarflare::Package::VSphereBundle:
                pkgType = SF_SoftwareInstallationServiceCapabilities::_SupportedExtendedResourceTypes::enum_VMware_vSphere_Installation_Bundle;
                break;
            case solarflare::Package::Tarball:
                pkgType = SF_SoftwareInstallationServiceCapabilities::_SupportedExtendedResourceTypes::enum_Other;
                newSvc->OtherSupportedExtendedResourceTypeDescriptions.null = false;
                newSvc->OtherSupportedExtendedResourceTypeDescriptions.value.append("TAR archive");
                break;
            default:
                pkgType = SF_SoftwareInstallationServiceCapabilities::_SupportedExtendedResourceTypes::enum_Unknown;
                break;
        }
        if (pkgType != 0)
        {
            newSvc->SupportedExtendedResourceTypes.null = false;
            newSvc->SupportedExtendedResourceTypes.value.append(pkgType);
        }
        return newSvc;
    }


    Instance *
    SWConformsToProfileHelper::instance(const SystemElement& se, unsigned) const
    {
        return DMTFProfileInfo::SoftwareInventoryProfile.               \
        conformingElement(se.cimReference(SF_SoftwareIdentity::static_meta_class));
    }

    unsigned 
    InstallableConformsToProfileHelper::nObjects(const SystemElement& se) const
    {
        // This is nothing magic here: an object that is installable conforms
        // to all the profile of ordinary software (a single one as of now)
        // + Software Update profile + Job Control profile if it has an
        // associated thread
        return SWConformsToProfileHelper::nObjects(se) + 1 + 
        (se.cimDispatch(cimple::SF_ConcreteJob::static_meta_class) != NULL);
    }

    Instance *
    InstallableConformsToProfileHelper::instance(const SystemElement& se, unsigned idx) const
    {
        switch (idx)
        {
            case 0:
                return SWConformsToProfileHelper::instance(se, 0);
            case 1:                
                return DMTFProfileInfo::SoftwareUpdateProfile.          \
                conformingElement(se.cimReference(SF_SoftwareInstallationService::static_meta_class));
            case 2:
                return DMTFProfileInfo::JobControlProfile.              \
                conformingElement(se.cimReference(SF_ConcreteJob::static_meta_class));
            default:
                return NULL;
        }
    }

} // namespace
