// Generated by genprov 2.0.24
#include "SF_SoftwareInstallationServiceCapabilities_Provider.h"
#include "SF_SoftwareIdentity_Provider.h"

CIMPLE_NAMESPACE_BEGIN

SF_SoftwareInstallationServiceCapabilities *
SF_SoftwareInstallationServiceCapabilities_Provider::makeReference(const solarflare::SWElement& sw)
{
    SF_SoftwareInstallationServiceCapabilities *newSvc = SF_SoftwareInstallationServiceCapabilities::create(true);
    
    newSvc->InstanceID.set(solarflare::System::target.prefix());
    newSvc->InstanceID.value.append(":");
    newSvc->InstanceID.value.append(sw.name());
    newSvc->InstanceID.value.append(" Installation Capabilities");
    return newSvc;
}


bool SF_SoftwareInstallationServiceCapabilities_Provider::SWEnum::process(const solarflare::SystemElement& se)
{
    const solarflare::SWElement& sw = static_cast<const solarflare::SWElement&>(se);
    bool firmware = false;
    switch (sw.classify())
    {
        case solarflare::SWElement::SWPackage:
            break;
        case solarflare::SWElement::SWFirmware:
            firmware = true;
            break;
        default:
            /* just skip */
            return true;
    }
    SF_SoftwareInstallationServiceCapabilities *newSvc = makeReference(sw);

    newSvc->SupportedAsynchronousActions.null = false;
    newSvc->SupportedAsynchronousActions.value.append(SF_SoftwareInstallationServiceCapabilities::_SupportedAsynchronousActions::enum_Install_from_URI);
    newSvc->SupportedSynchronousActions.null = false;
    newSvc->SupportedSynchronousActions.value.append(SF_SoftwareInstallationServiceCapabilities::_SupportedSynchronousActions::enum_Install_from_URI);
    newSvc->SupportedTargetTypes.null = false;
    newSvc->SupportedTargetTypes.value.append(firmware ? "NIC" : "Host");
    if (firmware)
    {
        newSvc->SupportedExtendedResourceTypes.null = false;
        newSvc->SupportedExtendedResourceTypes.value.append(SF_SoftwareInstallationServiceCapabilities::_SupportedExtendedResourceTypes::enum_Other);
        newSvc->OtherSupportedExtendedResourceTypeDescriptions.null = false;
        newSvc->OtherSupportedExtendedResourceTypeDescriptions.value.append("Firmware image");
    }
    else
    {
        const solarflare::Package& pkg = static_cast<const solarflare::Package&>(sw);
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
    }
    static const uint16 supported[] = 
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

    handler->handle(newSvc);
    

    return true;
}


SF_SoftwareInstallationServiceCapabilities_Provider::SF_SoftwareInstallationServiceCapabilities_Provider()
{
}

SF_SoftwareInstallationServiceCapabilities_Provider::~SF_SoftwareInstallationServiceCapabilities_Provider()
{
}

Load_Status SF_SoftwareInstallationServiceCapabilities_Provider::load()
{
    return LOAD_OK;
}

Unload_Status SF_SoftwareInstallationServiceCapabilities_Provider::unload()
{
    return UNLOAD_OK;
}

Get_Instance_Status SF_SoftwareInstallationServiceCapabilities_Provider::get_instance(
    const SF_SoftwareInstallationServiceCapabilities* model,
    SF_SoftwareInstallationServiceCapabilities*& instance)
{
    return GET_INSTANCE_UNSUPPORTED;
}

Enum_Instances_Status SF_SoftwareInstallationServiceCapabilities_Provider::enum_instances(
    const SF_SoftwareInstallationServiceCapabilities* model,
    Enum_Instances_Handler<SF_SoftwareInstallationServiceCapabilities>* handler)
{
    SWEnum caps(handler);
    solarflare::System::target.forAllSoftware(caps);
    
    return ENUM_INSTANCES_OK;
}

Create_Instance_Status SF_SoftwareInstallationServiceCapabilities_Provider::create_instance(
    SF_SoftwareInstallationServiceCapabilities* instance)
{
    return CREATE_INSTANCE_UNSUPPORTED;
}

Delete_Instance_Status SF_SoftwareInstallationServiceCapabilities_Provider::delete_instance(
    const SF_SoftwareInstallationServiceCapabilities* instance)
{
    return DELETE_INSTANCE_UNSUPPORTED;
}

Modify_Instance_Status SF_SoftwareInstallationServiceCapabilities_Provider::modify_instance(
    const SF_SoftwareInstallationServiceCapabilities* model,
    const SF_SoftwareInstallationServiceCapabilities* instance)
{
    return MODIFY_INSTANCE_UNSUPPORTED;
}

/*@END@*/

CIMPLE_NAMESPACE_END