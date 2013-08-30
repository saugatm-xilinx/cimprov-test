// Generated by genprov 2.0.24
#include "SF_SoftwareInstallationService_Provider.h"
#include "SF_SoftwareIdentity_Provider.h"
#include "SF_ComputerSystem_Provider.h"
#include "SF_NICCard_Provider.h"
#include "SF_SoftwareInstallationServiceCapabilities.h"
#include "sf_provider.h"

CIMPLE_NAMESPACE_BEGIN

SF_SoftwareInstallationService_Provider::SF_SoftwareInstallationService_Provider()
{
}

SF_SoftwareInstallationService_Provider::~SF_SoftwareInstallationService_Provider()
{
}

Load_Status SF_SoftwareInstallationService_Provider::load()
{
    return LOAD_OK;
}

Unload_Status SF_SoftwareInstallationService_Provider::unload()
{
    return UNLOAD_OK;
}

Get_Instance_Status SF_SoftwareInstallationService_Provider::get_instance(
    const SF_SoftwareInstallationService* model,
    SF_SoftwareInstallationService*& instance)
{
    return GET_INSTANCE_UNSUPPORTED;
}

Enum_Instances_Status SF_SoftwareInstallationService_Provider::enum_instances(
    const SF_SoftwareInstallationService* model,
    Enum_Instances_Handler<SF_SoftwareInstallationService>* handler)
{    
    solarflare::EnumInstances<SF_SoftwareInstallationService>::
                                              allSoftwareTypes(handler);
    return ENUM_INSTANCES_OK;
}

Create_Instance_Status SF_SoftwareInstallationService_Provider::create_instance(
    SF_SoftwareInstallationService* instance)
{
    return CREATE_INSTANCE_UNSUPPORTED;
}

Delete_Instance_Status SF_SoftwareInstallationService_Provider::delete_instance(
    const SF_SoftwareInstallationService* instance)
{
    return DELETE_INSTANCE_UNSUPPORTED;
}

Modify_Instance_Status SF_SoftwareInstallationService_Provider::modify_instance(
    const SF_SoftwareInstallationService* model,
    const SF_SoftwareInstallationService* instance)
{
    return MODIFY_INSTANCE_UNSUPPORTED;
}

Invoke_Method_Status SF_SoftwareInstallationService_Provider::RequestStateChange(
    const SF_SoftwareInstallationService* self,
    const Property<uint16>& RequestedState,
    CIM_ConcreteJob*& Job,
    const Property<Datetime>& TimeoutPeriod,
    Property<uint32>& return_value)
{
    return INVOKE_METHOD_UNSUPPORTED;
}

Invoke_Method_Status SF_SoftwareInstallationService_Provider::StartService(
    const SF_SoftwareInstallationService* self,
    Property<uint32>& return_value)
{
    return INVOKE_METHOD_UNSUPPORTED;
}

Invoke_Method_Status SF_SoftwareInstallationService_Provider::StopService(
    const SF_SoftwareInstallationService* self,
    Property<uint32>& return_value)
{
    return INVOKE_METHOD_UNSUPPORTED;
}

Invoke_Method_Status SF_SoftwareInstallationService_Provider::CheckSoftwareIdentity(
    const SF_SoftwareInstallationService* self,
    const CIM_SoftwareIdentity* Source,
    const CIM_ManagedElement* Target,
    const CIM_Collection* Collection,
    Property<Array_uint16>& InstallCharacteristics,
    Property<uint32>& return_value)
{
    return INVOKE_METHOD_UNSUPPORTED;
}

Invoke_Method_Status SF_SoftwareInstallationService_Provider::InstallFromSoftwareIdentity(
    const SF_SoftwareInstallationService* self,
    CIM_ConcreteJob*& Job,
    const Property<Array_uint16>& InstallOptions,
    const Property<Array_String>& InstallOptionsValues,
    const CIM_SoftwareIdentity* Source,
    const CIM_ManagedElement* Target,
    const CIM_Collection* Collection,
    Property<uint32>& return_value)
{
    return INVOKE_METHOD_UNSUPPORTED;
}

void SF_SoftwareInstallationService_Provider::Installer::handler(solarflare::SystemElement& se,
                                                                 unsigned)
{
    solarflare::SWElement& sw = static_cast<solarflare::SWElement&>(se);
    if (firstRun)
    {
        ok = true;
        firstRun = false;
    }
    ok = ok && sw.install(uri);
}

void SF_SoftwareInstallationService_Provider::NICInstaller::handler(solarflare::SystemElement& se,
                                                                    unsigned)
{
    solarflare::NIC& nic = static_cast<solarflare::NIC&>(se);
    Installer installer(uri, service);
    installer.forSoftware(nic);
    ok = installer.isOk();
}

Invoke_Method_Status SF_SoftwareInstallationService_Provider::InstallFromURI(
    const SF_SoftwareInstallationService* self,
    CIM_ConcreteJob*& Job,
    const Property<String>& URI,
    const CIM_ManagedElement* Target,
    const Property<Array_uint16>& InstallOptions,
    const Property<Array_String>& InstallOptionsValues,
    Property<uint32>& return_value)
{
    const CIM_ComputerSystem *sys = NULL;

    /// CIMPLE is unable to generate enums for method parameters
    enum ReturnValue 
    {
        OK = 0,
        Error = 2,
        InvalidParameter = 5,
        JobCreated = 4096
    };

    if (URI.null)
    {
        return_value.set(InvalidParameter);
        return INVOKE_METHOD_OK;
    }

    if (!InstallOptions.null)
    {
        for (size_t i = 0; i < InstallOptions.value.size(); i++)
        {
            switch (InstallOptions.value[i])
            {
                case SF_SoftwareInstallationServiceCapabilities::_SupportedInstallOptions::enum_Update:
                case SF_SoftwareInstallationServiceCapabilities::_SupportedInstallOptions::enum_Repair:
                case SF_SoftwareInstallationServiceCapabilities::_SupportedInstallOptions::enum_Force_installation:
                case SF_SoftwareInstallationServiceCapabilities::_SupportedInstallOptions::enum_Defer_target_system_reset:
                case SF_SoftwareInstallationServiceCapabilities::_SupportedInstallOptions::enum_Log:
                case SF_SoftwareInstallationServiceCapabilities::_SupportedInstallOptions::enum_SilentMode:
                case SF_SoftwareInstallationServiceCapabilities::_SupportedInstallOptions::enum_AdministrativeMode:
                    /* option is supported, do nothing */
                    break;
                default:
                    return_value.set(InvalidParameter);
                    return INVOKE_METHOD_OK;
            }
        }
    }

    if (Target == NULL ||
        (sys = cast<const CIM_ComputerSystem *>(Target)) != NULL)
    {
        if (Target != NULL &&
            !solarflare::CIMHelper::isOurSystem(sys))
        {
            return_value.set(InvalidParameter);
            return INVOKE_METHOD_OK;
        }
        Installer installer(URI.value.c_str(), self);
        installer.forSoftware();
        if (installer.isOk())
            return_value.set(OK);
        else
            return_value.set(Error);
    }
    else if (const CIM_Card *card = cast<const CIM_Card *>(Target))
    {
        NICInstaller installer(URI.value.c_str(), self, card);
        installer.forNIC();
        if (installer.isOk())
            return_value.set(OK);
        else
            return_value.set(Error);
    }
    else
    {
        return_value.set(InvalidParameter);
        return INVOKE_METHOD_OK;
    }

    return INVOKE_METHOD_OK;
}

Invoke_Method_Status SF_SoftwareInstallationService_Provider::ChangeAffectedElementsAssignedSequence(
    const SF_SoftwareInstallationService* self,
    const Property< Array<CIM_ManagedElement*> >& ManagedElements,
    const Property<Array_uint16>& AssignedSequence,
    CIM_ConcreteJob*& Job,
    Property<uint32>& return_value)

{
    return INVOKE_METHOD_UNSUPPORTED;
}

Invoke_Method_Status SF_SoftwareInstallationService_Provider::InstallFromByteStream(
    const SF_SoftwareInstallationService* self,
    CIM_ConcreteJob*& Job,
    const Property<Array_uint8>& Image,
    const CIM_ManagedElement* Target,
    const Property<Array_uint16>& InstallOptions,
    const Property<Array_String>& InstallOptionsValues,
    Property<uint32>& return_value)

{
    return INVOKE_METHOD_UNSUPPORTED;
}

/*@END@*/

CIMPLE_NAMESPACE_END
