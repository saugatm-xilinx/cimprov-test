// Generated by genprov 2.0.24
#include "SF_LogEntry_Provider.h"
#include "sf_provider.h"

CIMPLE_NAMESPACE_BEGIN

SF_LogEntry_Provider::SF_LogEntry_Provider()
{
}

SF_LogEntry_Provider::~SF_LogEntry_Provider()
{
}

Load_Status SF_LogEntry_Provider::load()
{
    solarflare::CIMHelper::initialize();
    return LOAD_OK;
}

Unload_Status SF_LogEntry_Provider::unload()
{
    return UNLOAD_OK;
}

Get_Instance_Status SF_LogEntry_Provider::get_instance(
    const SF_LogEntry* model,
    SF_LogEntry*& instance)
{
    return GET_INSTANCE_UNSUPPORTED;
}

Enum_Instances_Status SF_LogEntry_Provider::enum_instances(
    const SF_LogEntry* model,
    Enum_Instances_Handler<SF_LogEntry>* handler)
{
    solarflare::EnumInstances<SF_LogEntry>::topmost(handler);
    return ENUM_INSTANCES_OK;
}

Create_Instance_Status SF_LogEntry_Provider::create_instance(
    SF_LogEntry* instance)
{
    return CREATE_INSTANCE_UNSUPPORTED;
}

Delete_Instance_Status SF_LogEntry_Provider::delete_instance(
    const SF_LogEntry* instance)
{
    return DELETE_INSTANCE_UNSUPPORTED;
}

Modify_Instance_Status SF_LogEntry_Provider::modify_instance(
    const SF_LogEntry* model,
    const SF_LogEntry* instance)
{
    return MODIFY_INSTANCE_UNSUPPORTED;
}

/*@END@*/

CIMPLE_NAMESPACE_END