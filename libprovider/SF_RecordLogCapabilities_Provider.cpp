// Generated by genprov 2.0.24
#include "SF_RecordLogCapabilities_Provider.h"

CIMPLE_NAMESPACE_BEGIN

SF_RecordLogCapabilities_Provider::SF_RecordLogCapabilities_Provider()
{
}

SF_RecordLogCapabilities_Provider::~SF_RecordLogCapabilities_Provider()
{
}

Load_Status SF_RecordLogCapabilities_Provider::load()
{
    return LOAD_OK;
}

Unload_Status SF_RecordLogCapabilities_Provider::unload()
{
    return UNLOAD_OK;
}

Get_Instance_Status SF_RecordLogCapabilities_Provider::get_instance(
    const SF_RecordLogCapabilities* model,
    SF_RecordLogCapabilities*& instance)
{
    return GET_INSTANCE_UNSUPPORTED;
}

Enum_Instances_Status SF_RecordLogCapabilities_Provider::enum_instances(
    const SF_RecordLogCapabilities* model,
    Enum_Instances_Handler<SF_RecordLogCapabilities>* handler)
{
    solarflare::EnumInstances<SF_RecordLogCapabilities>::topmost(handler);
    return ENUM_INSTANCES_OK;
}

Create_Instance_Status SF_RecordLogCapabilities_Provider::create_instance(
    SF_RecordLogCapabilities* instance)
{
    return CREATE_INSTANCE_UNSUPPORTED;
}

Delete_Instance_Status SF_RecordLogCapabilities_Provider::delete_instance(
    const SF_RecordLogCapabilities* instance)
{
    return DELETE_INSTANCE_UNSUPPORTED;
}

Modify_Instance_Status SF_RecordLogCapabilities_Provider::modify_instance(
    const SF_RecordLogCapabilities* model,
    const SF_RecordLogCapabilities* instance)
{
    return MODIFY_INSTANCE_UNSUPPORTED;
}

#if CIM_SCHEMA_VERSION_MINOR < 28
Invoke_Method_Status SF_RecordLogCapabilities_Provider::CreateGoalSettings(
    const SF_RecordLogCapabilities* self,
    const Property< Array<CIM_SettingData*> >& TemplateGoalSettings,
    Property< Array<CIM_SettingData*> >& SupportedGoalSettings,
    Property<uint16>& return_value)

{
    return INVOKE_METHOD_UNSUPPORTED;
}
#endif

/*@END@*/

CIMPLE_NAMESPACE_END
