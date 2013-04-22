// Generated by genprov 2.0.24
#include "SF_AffectedJobElement_Provider.h"
#include "SF_PortController_Provider.h"
#include "SF_ConcreteJob_Provider.h"

CIMPLE_NAMESPACE_BEGIN

SF_AffectedJobElement_Provider::SF_AffectedJobElement_Provider()
{
}

SF_AffectedJobElement_Provider::~SF_AffectedJobElement_Provider()
{
}

Load_Status SF_AffectedJobElement_Provider::load()
{
    return LOAD_OK;
}

Unload_Status SF_AffectedJobElement_Provider::unload()
{
    return UNLOAD_OK;
}

Get_Instance_Status SF_AffectedJobElement_Provider::get_instance(
    const SF_AffectedJobElement* model,
    SF_AffectedJobElement*& instance)
{
    return GET_INSTANCE_UNSUPPORTED;
}

Enum_Instances_Status SF_AffectedJobElement_Provider::enum_instances(
    const SF_AffectedJobElement* model,
    Enum_Instances_Handler<SF_AffectedJobElement>* handler)
{
#if !TARGET_CIM_SERVER_esxi
    solarflare::EnumInstances<SF_AffectedJobElement>::allDiagnostics(handler);
#endif
    return ENUM_INSTANCES_OK;
}

Create_Instance_Status SF_AffectedJobElement_Provider::create_instance(
    SF_AffectedJobElement* instance)
{
    return CREATE_INSTANCE_UNSUPPORTED;
}

Delete_Instance_Status SF_AffectedJobElement_Provider::delete_instance(
    const SF_AffectedJobElement* instance)
{
    return DELETE_INSTANCE_UNSUPPORTED;
}

Modify_Instance_Status SF_AffectedJobElement_Provider::modify_instance(
    const SF_AffectedJobElement* model,
    const SF_AffectedJobElement* instance)
{
    return MODIFY_INSTANCE_UNSUPPORTED;
}

Enum_Associator_Names_Status SF_AffectedJobElement_Provider::enum_associator_names(
    const Instance* instance,
    const String& result_class,
    const String& role,
    const String& result_role,
    Enum_Associator_Names_Handler<Instance>* handler)
{
    return ENUM_ASSOCIATOR_NAMES_UNSUPPORTED;
}

Enum_Associators_Status SF_AffectedJobElement_Provider::enum_associators(
    const Instance* instance,
    const String& result_class,
    const String& role,
    const String& result_role,
    Enum_Associators_Handler<Instance>* handler)
{
    return ENUM_ASSOCIATORS_UNSUPPORTED;
}

Enum_References_Status SF_AffectedJobElement_Provider::enum_references(
    const Instance* instance,
    const SF_AffectedJobElement* model,
    const String& role,
    Enum_References_Handler<SF_AffectedJobElement>* handler)
{
    return ENUM_REFERENCES_UNSUPPORTED;
}

/*@END@*/

CIMPLE_NAMESPACE_END
