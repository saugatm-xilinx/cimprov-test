#include "sf_provider.h"
#include "SF_SoftwareIdentity.h"
#include "SF_SoftwareInstallationService.h"
#include "SF_SoftwareInstallationServiceCapabilities.h"
#include "SF_BundleComponent.h"
#include "SF_ConcreteJob.h"
#include "CIM_OperatingSystem.h"
#include "SF_ElementConformsToProfile.h"
#include "SF_ElementCapabilities.h"
#include "SF_ElementSoftwareIdentity.h"
#include "SF_PortController.h"
#include "SF_HostedService.h"
#include "SF_InstalledSoftwareIdentity.h"
#include "SF_ServiceAffectsSystem.h"
#include "SF_ServiceAffectsSoftware.h"
#include "SF_ServiceAffectsCard.h"
#include "SF_NICCard.h"

namespace solarflare 
{
    using cimple::Instance;
    using cimple::Meta_Class;
    using cimple::CIM_ComputerSystem;
    using cimple::Ref;
    using cimple::cast;
    using cimple::is_a;
    using cimple::CIM_SoftwareIdentity;
    using cimple::SF_SoftwareIdentity;
    using cimple::SF_SoftwareInstallationService;
    using cimple::SF_SoftwareInstallationServiceCapabilities;
    using cimple::CIM_OperatingSystem;
    using cimple::SF_ConcreteJob;
    using cimple::SF_BundleComponent;
    using cimple::CIM_ManagedElement;
    using cimple::SF_ElementConformsToProfile;
    using cimple::SF_ElementCapabilities;
    using cimple::SF_ElementSoftwareIdentity;
    using cimple::SF_PortController;
    using cimple::SF_HostedService;
    using cimple::SF_InstalledSoftwareIdentity;
    using cimple::SF_ServiceAffectsSystem;
    using cimple::SF_ServiceAffectsSoftware;
    using cimple::SF_ServiceAffectsCard;
    using cimple::SF_NICCard;

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

    class FirmwareElementSoftwareIdentityHelper : public CIMHelper {
    public:
        virtual Instance *instance(const SystemElement& se, unsigned idx) const;
    };


    class InstallationHostedServiceHelper : public CIMHelper {
    public:
        virtual Instance *instance(const SystemElement &se, unsigned) const;
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

    class InstalledSoftwareIdentityHelper : public CIMHelper {
    public:
        virtual Instance *instance(const SystemElement& se, unsigned idx) const;
    };

    class FirmwareServiceAffectsCardHelper : public CIMHelper {
    public:
        virtual unsigned nObjects(const SystemElement&) const { return 2; }
        virtual Instance *instance(const SystemElement& se, unsigned idx) const;
    };

    class FirmwareServiceAffectsSoftwareHelper : public CIMHelper {
    public:
        virtual unsigned nObjects(const SystemElement&) const { return 2; }
        virtual Instance *instance(const SystemElement& se, unsigned idx) const;
    };

    class PackageServiceAffectsSystemHelper : public CIMHelper {
    public:
        virtual unsigned nObjects(const SystemElement&) const { return 2; }
        virtual Instance *instance(const SystemElement& se, unsigned idx) const;
    };

    class PackageServiceAffectsSoftwareHelper : public CIMHelper {
    public:
        virtual unsigned nObjects(const SystemElement&) const { return 2; }
        virtual Instance *instance(const SystemElement& se, unsigned idx) const;
    };

    class SoftwareServiceAffectsElementHelper : public CIMHelper {
    public:
        virtual Instance *instance(const SystemElement& se, unsigned idx) const;
    };


    const CIMHelper* SWElement::cimDispatch(const Meta_Class& cls) const
    {
        static const SoftwareIdentityHelper softwareIdentity;
        static const SWConformsToProfileHelper conformToProfile;
        static const InstalledSoftwareIdentityHelper installedSoftware;

        if (&cls == &SF_SoftwareIdentity::static_meta_class)
            return &softwareIdentity;
        if (&cls == &SF_InstalledSoftwareIdentity::static_meta_class)
            return &installedSoftware;
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
#if CIM_SCHEMA_VERSION_MINOR > 0
        identity->ElementName.set(sw.sysName());
#endif
        identity->Name.set(se.name());
        identity->MajorVersion.set(sw.version().major());
        identity->MinorVersion.set(sw.version().minor());
        if (sw.version().revision() != VersionInfo::unknown)
            identity->RevisionNumber.set(sw.version().revision());
        if (sw.version().build() != VersionInfo::unknown)
        {
#if CIM_SCHEMA_VERSION_MINOR == 26
            identity->LargeBuildNumber.set(sw.version().build());
            identity->IsLargeBuildNumber.set(true);
#else
            identity->BuildNumber.set(sw.version().build());
#endif
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
        static const SoftwareServiceAffectsElementHelper serviceAffectsSoftware;

        if (&cls == &SF_SoftwareIdentity::static_meta_class)
            return &hostSoftwareIdentity;
        else if (&cls == &SF_BundleComponent::static_meta_class)
            return &bundleComponent;
        else if (&cls == &SF_ServiceAffectsSoftware::static_meta_class)
            return &serviceAffectsSoftware;
        else
            return SWElement::cimDispatch(cls);
    }

    void HostSoftwareIdentityHelper::addTargetOS(SF_SoftwareIdentity *identity)
    {
        solarflare::System::OSType type = System::target.osType();
        bool is64 = System::target.is64bit();
        unsigned cimType[][2] = {
            {
                CIM_SoftwareIdentity::_TargetOSTypes::enum_Microsoft_Windows_Server_2003, 
                CIM_SoftwareIdentity::_TargetOSTypes::enum_Microsoft_Windows_Server_2003_64_Bit
            },
#if CIM_SCHEMA_VERSION_MINOR > 0
            {
#if CIM_SCHEMA_VERSION_MINOR == 26
                CIM_SoftwareIdentity::_TargetOSTypes::enum_Microsoft_Windows_Server_2008, 
                CIM_SoftwareIdentity::_TargetOSTypes::enum_Microsoft_Windows_Server_2008_64_Bit
#else
                CIM_OperatingSystem::_OSType::enum_Microsoft_Windows_Server_2008, 
                CIM_OperatingSystem::_OSType::enum_Microsoft_Windows_Server_2008_64_Bit
#endif
            },
            {
#if CIM_SCHEMA_VERSION_MINOR == 26
                CIM_SoftwareIdentity::_TargetOSTypes::enum_Microsoft_Windows_Server_2008_R2, 
                CIM_SoftwareIdentity::_TargetOSTypes::enum_Microsoft_Windows_Server_2008_R2
#else
                CIM_OperatingSystem::_OSType::enum_Microsoft_Windows_Server_2008_64_Bit, 
                CIM_OperatingSystem::_OSType::enum_Microsoft_Windows_Server_2008_64_Bit
#endif
            },
#else
            {
                CIM_SoftwareIdentity::_TargetOSTypes::enum_Microsoft_Windows_Server_2003, 
                CIM_SoftwareIdentity::_TargetOSTypes::enum_Microsoft_Windows_Server_2003_64_Bit
            },
            {
                CIM_SoftwareIdentity::_TargetOSTypes::enum_Microsoft_Windows_Server_2003, 
                CIM_SoftwareIdentity::_TargetOSTypes::enum_Microsoft_Windows_Server_2003_64_Bit
            },
#endif
#if CIM_SCHEMA_VERSION_MINOR == 26
            {
                CIM_SoftwareIdentity::_TargetOSTypes::enum_RedHat_Enterprise_Linux,
                CIM_SoftwareIdentity::_TargetOSTypes::enum_RedHat_Enterprise_Linux_64_Bit
            },
            {
                CIM_SoftwareIdentity::_TargetOSTypes::enum_SLES,
                CIM_SoftwareIdentity::_TargetOSTypes::enum_SLES_64_Bit
            },
            {
                CIM_SoftwareIdentity::_TargetOSTypes::enum_Debian,
                CIM_SoftwareIdentity::_TargetOSTypes::enum_Debian_64_Bit
            },
            {
                CIM_SoftwareIdentity::_TargetOSTypes::enum_CentOS_32_bit,
                CIM_SoftwareIdentity::_TargetOSTypes::enum_CentOS_64_bit
            },
            {
                CIM_SoftwareIdentity::_TargetOSTypes::enum_Oracle_Enterprise_Linux_32_bit,
                CIM_SoftwareIdentity::_TargetOSTypes::enum_Oracle_Enterprise_Linux_64_bit
            },
            {
                CIM_SoftwareIdentity::_TargetOSTypes::enum_Linux_2_6_x,
                CIM_SoftwareIdentity::_TargetOSTypes::enum_Linux_2_6_x_64_Bit
            },
#else
#define GENERIC_LINUX {CIM_SoftwareIdentity::_TargetOSTypes::enum_LINUX, CIM_SoftwareIdentity::_TargetOSTypes::enum_LINUX}
            GENERIC_LINUX,
            GENERIC_LINUX,
            GENERIC_LINUX,
            GENERIC_LINUX,
            GENERIC_LINUX,
            GENERIC_LINUX,
#undef GENERIC_LINUX
#endif
            {
#if CIM_SCHEMA_VERSION_MINOR == 26
                CIM_SoftwareIdentity::_TargetOSTypes::enum_VMware_ESXi,
                CIM_SoftwareIdentity::_TargetOSTypes::enum_VMware_ESXi,
#else
                CIM_SoftwareIdentity::_TargetOSTypes::enum_Dedicated,
                CIM_SoftwareIdentity::_TargetOSTypes::enum_Dedicated,
#endif
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
        
        if (he.package() == NULL)
            return NULL;
        
        SF_BundleComponent *link = SF_BundleComponent::create(true);
        link->GroupComponent = 
        cast<CIM_SoftwareIdentity *>(he.package()->cimReference(SF_SoftwareIdentity::static_meta_class));
        link->PartComponent =
        cast<CIM_SoftwareIdentity *>(he.cimReference(SF_SoftwareIdentity::static_meta_class));
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
        static const InstallationHostedServiceHelper hostedService;
        static const InstallableConformsToProfileHelper conformToProfile;
        static const PackageServiceAffectsSystemHelper serviceAffectsSystem;
        static const PackageServiceAffectsSoftwareHelper serviceAffectsSoftware;
        static const ElementCapabilitiesHelper capsLink(SF_SoftwareInstallationService::static_meta_class,
                                                        SF_SoftwareInstallationServiceCapabilities::static_meta_class);

        if (&cls == &SF_SoftwareInstallationService::static_meta_class)
            return &bundleInstallation;
        if (&cls == &SF_SoftwareInstallationServiceCapabilities::static_meta_class)
            return &bundleInstallationCaps;
        if (&cls == &SF_ElementCapabilities::static_meta_class)
            return &capsLink;
        if (&cls == &SF_SoftwareIdentity::static_meta_class)
            return &bundleSoftwareIdentity;
        if (&cls == &SF_HostedService::static_meta_class)
            return &hostedService;
        if (&cls == &SF_ServiceAffectsSystem::static_meta_class)
            return &serviceAffectsSystem;
        if (&cls == &SF_ServiceAffectsSoftware::static_meta_class)
            return &serviceAffectsSoftware;
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
#if CIM_SCHEMA_VERSION_MINOR == 26
                pkgType = SF_SoftwareIdentity::_ExtendedResourceType::enum_VMware_vSphere_Installation_Bundle;
#else
                pkgType = SF_SoftwareIdentity::_ExtendedResourceType::enum_Other;
                identity->OtherExtendedResourceTypeDescription.set("vSphere Installation Bundle");
#endif
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
        static const ElementCapabilitiesHelper capsLink(SF_SoftwareInstallationService::static_meta_class,
                                                        SF_SoftwareInstallationServiceCapabilities::static_meta_class);
        static const InstallationJobHelper installationJob;
        static const InstallableConformsToProfileHelper conformToProfile;
        static const InstallationHostedServiceHelper hostedService;
        static const FirmwareElementSoftwareIdentityHelper elementSoftwareIdentity;
        static const FirmwareServiceAffectsCardHelper serviceAffectsCard;
        static const FirmwareServiceAffectsSoftwareHelper serviceAffectsSoftware;

        if (&cls == &SF_SoftwareInstallationService::static_meta_class)
            return &firmwareInstallation;
        if (&cls == &SF_SoftwareInstallationServiceCapabilities::static_meta_class)
            return &firmwareInstallationCaps;
        if (&cls == &SF_ConcreteJob::static_meta_class)
            return &installationJob;
        if (&cls == &SF_ElementSoftwareIdentity::static_meta_class)
            return &elementSoftwareIdentity;
        if (&cls == &SF_HostedService::static_meta_class)
            return &hostedService;
        if (&cls == &SF_ServiceAffectsCard::static_meta_class)
            return &serviceAffectsCard;
        if (&cls == &SF_ServiceAffectsSoftware::static_meta_class)
            return &serviceAffectsSoftware;
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
#if CIM_SCHEMA_VERSION_MINOR > 0
        newSvc->ElementName.set(sw.name());
#endif
#if CIM_SCHEMA_VERSION_MINOR == 26
        newSvc->InstanceID.set(instanceID(sw.name()));
        newSvc->InstanceID.value.append(" Installer");
#endif
#if CIM_SCHEMA_VERSION_MINOR > 0
        newSvc->OperationalStatus.null = false;
        newSvc->OperationalStatus.value.append(SF_SoftwareInstallationService::_OperationalStatus::enum_OK);
        newSvc->OperatingStatus.null = false;
        newSvc->OperatingStatus.value = SF_SoftwareInstallationService::_OperatingStatus::enum_Servicing;
        newSvc->PrimaryStatus.null = false;
        newSvc->PrimaryStatus.value = SF_SoftwareInstallationService::_PrimaryStatus::enum_OK;
        newSvc->EnabledState.null = false;
        newSvc->EnabledState.value = SF_SoftwareInstallationService::_EnabledState::enum_Enabled;
#endif

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
                pkgType = SF_SoftwareInstallationServiceCapabilities::_SupportedExtendedResourceTypes::enum_Other;
                newSvc->OtherSupportedExtendedResourceTypeDescriptions.null = false;
                newSvc->OtherSupportedExtendedResourceTypeDescriptions.value.append("VMWare vSphere bundle");
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
    FirmwareElementSoftwareIdentityHelper::instance(const SystemElement& se, unsigned) const
    {
        const Firmware& fw = static_cast<const Firmware&>(se);
        SF_ElementSoftwareIdentity *item = SF_ElementSoftwareIdentity::create(true);
        
        item->Antecedent = cast<cimple::CIM_SoftwareIdentity *>(fw.cimReference(SF_SoftwareIdentity::static_meta_class));
        item->Dependent = cast<cimple::CIM_PortController *>(fw.nic()->cimReference(SF_PortController::static_meta_class));
        item->ElementSoftwareStatus.null = false;
        item->ElementSoftwareStatus.value.append(SF_ElementSoftwareIdentity::_ElementSoftwareStatus::enum_Current);
        item->ElementSoftwareStatus.value.append(SF_ElementSoftwareIdentity::_ElementSoftwareStatus::enum_Next);
        item->ElementSoftwareStatus.value.append(SF_ElementSoftwareIdentity::_ElementSoftwareStatus::enum_Default);
        item->ElementSoftwareStatus.value.append(SF_ElementSoftwareIdentity::_ElementSoftwareStatus::enum_Installed);
        return item;
    }


    Instance * 
    InstallationHostedServiceHelper::instance(const solarflare::SystemElement& se, unsigned) const
    {
        SF_HostedService *link = SF_HostedService::create(true);
        link->Antecedent = solarflare::CIMHelper::systemRef();
        link->Dependent = cast<cimple::CIM_Service *>(se.cimReference(SF_SoftwareInstallationService::static_meta_class));
#if NEED_ASSOC_IN_ROOT_CIMV2
        link->Dependent->__name_space = CIMHelper::solarflareNS;
#endif 
        return link;
    }

    Instance *
    InstalledSoftwareIdentityHelper::instance(const SystemElement& se, unsigned) const
    {
        const SWElement& sw = static_cast<const solarflare::SWElement&>(se);
    
        if (sw.version().isUnknown())
            return NULL;
        
        SF_InstalledSoftwareIdentity *link = SF_InstalledSoftwareIdentity::create(true);
        
        link->System = cast<cimple::CIM_System *>(CIMHelper::findSystem()->clone());
        link->InstalledSoftware = cast<cimple::CIM_SoftwareIdentity *>(sw.cimReference(SF_SoftwareIdentity::static_meta_class));
    
#if NEED_ASSOC_IN_ROOT_CIMV2
        link->InstalledSoftware->__name_space = CIMHelper::solarflareNS;
#endif 
        return link;
    }

    Instance *
    FirmwareServiceAffectsCardHelper::instance(const SystemElement& se, unsigned idx) const
    {
        const Firmware& fw = static_cast<const Firmware&>(se);
    
        SF_ServiceAffectsCard *link = NULL;

        if (idx != 0)
            return NULL;

        link = SF_ServiceAffectsCard::create(true);
        link->AffectedElement = cast<cimple::CIM_Card *>(fw.nic()->cimReference(SF_NICCard::static_meta_class));
        link->AffectingElement = cast<cimple::CIM_Service *>(se.cimReference(SF_SoftwareInstallationService::static_meta_class));

        return link;
    }

    Instance *
    FirmwareServiceAffectsSoftwareHelper::instance(const SystemElement& se, unsigned idx) const
    {
        const Firmware& fw = static_cast<const Firmware&>(se);

        SF_ServiceAffectsSoftware *link = NULL;

        if (idx != 1)
            return NULL;

        link = SF_ServiceAffectsSoftware::create(true);
        link->AffectedElement = cast<cimple::CIM_SoftwareIdentity *>(se.cimReference(SF_SoftwareIdentity::static_meta_class));
        link->AffectingElement = cast<cimple::CIM_Service *>(se.cimReference(SF_SoftwareInstallationService::static_meta_class));

        return link;
    }

    Instance *
    PackageServiceAffectsSystemHelper::instance(const SystemElement& se, unsigned idx) const
    {
        SF_ServiceAffectsSystem *link = NULL;

        if (idx != 0)
            return NULL;

        link = SF_ServiceAffectsSystem::create(true);
        link->AffectedElement = cast<cimple::CIM_ComputerSystem *>(findSystem()->clone());
        link->AffectingElement = cast<cimple::CIM_Service *>(se.cimReference(SF_SoftwareInstallationService::static_meta_class));

#if NEED_ASSOC_IN_ROOT_CIMV2
        link->AffectingElement->__name_space = CIMHelper::solarflareNS;
#endif
        return link;
    }

    Instance *
    PackageServiceAffectsSoftwareHelper::instance(const SystemElement& se, unsigned idx) const
    {
        SF_ServiceAffectsSoftware *link = NULL;

        if (idx != 1)
            return NULL;

        link = SF_ServiceAffectsSoftware::create(true);
        link->AffectedElement = cast<cimple::CIM_SoftwareIdentity *>(se.cimReference(SF_SoftwareIdentity::static_meta_class));
        link->AffectingElement = cast<cimple::CIM_Service *>(se.cimReference(SF_SoftwareInstallationService::static_meta_class));
        return link;
    }

    Instance *
    SoftwareServiceAffectsElementHelper::instance(const SystemElement& se, unsigned idx) const
    {
        const HostSWElement& hsw = static_cast<const HostSWElement&>(se);
        if (hsw.package() == NULL)
            return NULL;

        SF_ServiceAffectsSoftware *link = SF_ServiceAffectsSoftware::create(true);

        link->AffectedElement = cast<cimple::CIM_SoftwareIdentity *>(se.cimReference(SF_SoftwareIdentity::static_meta_class));
        link->AffectingElement = cast<cimple::CIM_Service *>(hsw.package()->cimReference(SF_SoftwareInstallationService::static_meta_class));
        return link;
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
#if !TARGET_CIM_SERVER_esxi
        return SWConformsToProfileHelper::nObjects(se) + 1 + 
        (se.cimDispatch(cimple::SF_ConcreteJob::static_meta_class) != NULL);
#else
        // On ESXi Job Control profile support is not declared due to
        // incorrect CIM PAT Provides Tags Validation test.
        return SWConformsToProfileHelper::nObjects(se) + 1;
#endif
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
