// Generated by genprov 2.0.24
#include "SF_ConnectorRealizesPort_Provider.h"

CIMPLE_NAMESPACE_BEGIN

SF_ConnectorRealizesPort_Provider::SF_ConnectorRealizesPort_Provider()
{
}

SF_ConnectorRealizesPort_Provider::~SF_ConnectorRealizesPort_Provider()
{
}

Load_Status SF_ConnectorRealizesPort_Provider::load()
{
    return LOAD_OK;
}

Unload_Status SF_ConnectorRealizesPort_Provider::unload()
{
    return UNLOAD_OK;
}

Get_Instance_Status SF_ConnectorRealizesPort_Provider::get_instance(
    const SF_ConnectorRealizesPort* model,
    SF_ConnectorRealizesPort*& instance)
{
    return GET_INSTANCE_UNSUPPORTED;
}

Enum_Instances_Status SF_ConnectorRealizesPort_Provider::enum_instances(
    const SF_ConnectorRealizesPort* model,
    Enum_Instances_Handler<SF_ConnectorRealizesPort>* handler)
{
    return ENUM_INSTANCES_OK;
}

Create_Instance_Status SF_ConnectorRealizesPort_Provider::create_instance(
    SF_ConnectorRealizesPort* instance)
{
    return CREATE_INSTANCE_UNSUPPORTED;
}

Delete_Instance_Status SF_ConnectorRealizesPort_Provider::delete_instance(
    const SF_ConnectorRealizesPort* instance)
{
    return DELETE_INSTANCE_UNSUPPORTED;
}

Modify_Instance_Status SF_ConnectorRealizesPort_Provider::modify_instance(
    const SF_ConnectorRealizesPort* model,
    const SF_ConnectorRealizesPort* instance)
{
    return MODIFY_INSTANCE_UNSUPPORTED;
}

Enum_Associator_Names_Status SF_ConnectorRealizesPort_Provider::enum_associator_names(
    const Instance* instance,
    const String& result_class,
    const String& role,
    const String& result_role,
    Enum_Associator_Names_Handler<Instance>* handler)
{
    return ENUM_ASSOCIATOR_NAMES_UNSUPPORTED;
}

Enum_Associators_Status SF_ConnectorRealizesPort_Provider::enum_associators(
    const Instance* instance,
    const String& result_class,
    const String& role,
    const String& result_role,
    Enum_Associators_Handler<Instance>* handler)
{
    return ENUM_ASSOCIATORS_UNSUPPORTED;
}

Enum_References_Status SF_ConnectorRealizesPort_Provider::enum_references(
    const Instance* instance,
    const SF_ConnectorRealizesPort* model,
    const String& role,
    Enum_References_Handler<SF_ConnectorRealizesPort>* handler)
{
    return ENUM_REFERENCES_UNSUPPORTED;
}

/*@END@*/

CIMPLE_NAMESPACE_END
