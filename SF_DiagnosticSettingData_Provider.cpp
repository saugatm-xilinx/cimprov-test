// Generated by genprov 2.0.24
#include "SF_DiagnosticSettingData_Provider.h"

CIMPLE_NAMESPACE_BEGIN

SF_DiagnosticSettingData_Provider::SF_DiagnosticSettingData_Provider()
{
}

SF_DiagnosticSettingData_Provider::~SF_DiagnosticSettingData_Provider()
{
}

Load_Status SF_DiagnosticSettingData_Provider::load()
{
    return LOAD_OK;
}

Unload_Status SF_DiagnosticSettingData_Provider::unload()
{
    return UNLOAD_OK;
}

Get_Instance_Status SF_DiagnosticSettingData_Provider::get_instance(
    const SF_DiagnosticSettingData* model,
    SF_DiagnosticSettingData*& instance)
{
    return GET_INSTANCE_UNSUPPORTED;
}

Enum_Instances_Status SF_DiagnosticSettingData_Provider::enum_instances(
    const SF_DiagnosticSettingData* model,
    Enum_Instances_Handler<SF_DiagnosticSettingData>* handler)
{
    return ENUM_INSTANCES_OK;
}

Create_Instance_Status SF_DiagnosticSettingData_Provider::create_instance(
    SF_DiagnosticSettingData* instance)
{
    return CREATE_INSTANCE_UNSUPPORTED;
}

Delete_Instance_Status SF_DiagnosticSettingData_Provider::delete_instance(
    const SF_DiagnosticSettingData* instance)
{
    return DELETE_INSTANCE_UNSUPPORTED;
}

Modify_Instance_Status SF_DiagnosticSettingData_Provider::modify_instance(
    const SF_DiagnosticSettingData* model,
    const SF_DiagnosticSettingData* instance)
{
    return MODIFY_INSTANCE_UNSUPPORTED;
}

/*@END@*/

CIMPLE_NAMESPACE_END
