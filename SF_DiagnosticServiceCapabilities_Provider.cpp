// Generated by genprov 2.0.24
#include "SF_DiagnosticServiceCapabilities_Provider.h"

CIMPLE_NAMESPACE_BEGIN

SF_DiagnosticServiceCapabilities_Provider::SF_DiagnosticServiceCapabilities_Provider()
{
}

SF_DiagnosticServiceCapabilities_Provider::~SF_DiagnosticServiceCapabilities_Provider()
{
}

Load_Status SF_DiagnosticServiceCapabilities_Provider::load()
{
    return LOAD_OK;
}

Unload_Status SF_DiagnosticServiceCapabilities_Provider::unload()
{
    return UNLOAD_OK;
}

Get_Instance_Status SF_DiagnosticServiceCapabilities_Provider::get_instance(
    const SF_DiagnosticServiceCapabilities* model,
    SF_DiagnosticServiceCapabilities*& instance)
{
    return GET_INSTANCE_UNSUPPORTED;
}

Enum_Instances_Status SF_DiagnosticServiceCapabilities_Provider::enum_instances(
    const SF_DiagnosticServiceCapabilities* model,
    Enum_Instances_Handler<SF_DiagnosticServiceCapabilities>* handler)
{
    return ENUM_INSTANCES_OK;
}

Create_Instance_Status SF_DiagnosticServiceCapabilities_Provider::create_instance(
    SF_DiagnosticServiceCapabilities* instance)
{
    return CREATE_INSTANCE_UNSUPPORTED;
}

Delete_Instance_Status SF_DiagnosticServiceCapabilities_Provider::delete_instance(
    const SF_DiagnosticServiceCapabilities* instance)
{
    return DELETE_INSTANCE_UNSUPPORTED;
}

Modify_Instance_Status SF_DiagnosticServiceCapabilities_Provider::modify_instance(
    const SF_DiagnosticServiceCapabilities* model,
    const SF_DiagnosticServiceCapabilities* instance)
{
    return MODIFY_INSTANCE_UNSUPPORTED;
}

/*@END@*/

CIMPLE_NAMESPACE_END
