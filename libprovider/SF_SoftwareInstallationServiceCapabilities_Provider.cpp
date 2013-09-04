// Generated by genprov 2.0.24
#include "SF_SoftwareInstallationServiceCapabilities_Provider.h"
#include "SF_SoftwareIdentity_Provider.h"
#include "sf_provider.h"

CIMPLE_NAMESPACE_BEGIN

SF_SoftwareInstallationServiceCapabilities_Provider::SF_SoftwareInstallationServiceCapabilities_Provider()
{
}

SF_SoftwareInstallationServiceCapabilities_Provider::~SF_SoftwareInstallationServiceCapabilities_Provider()
{
}

Load_Status SF_SoftwareInstallationServiceCapabilities_Provider::load()
{
    solarflare::CIMHelper::initialize();
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
    solarflare::EnumInstances<SF_SoftwareInstallationServiceCapabilities>::
                                                  allSoftwareTypes(handler);
    
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

#if CIM_SCHEMA_VERSION_MINOR < 28
Invoke_Method_Status SF_SoftwareInstallationServiceCapabilities_Provider::CreateGoalSettings(
    const SF_SoftwareInstallationServiceCapabilities* self,
    const Property< Array<CIM_SettingData*> >& TemplateGoalSettings,
    Property< Array<CIM_SettingData*> >& SupportedGoalSettings,
    Property<uint16>& return_value)

{
    return INVOKE_METHOD_UNSUPPORTED;
}
#endif

/*@END@*/

CIMPLE_NAMESPACE_END
