// Generated by genprov 2.0.24
#include "SF_ElementCapabilities_Provider.h"
#include "SF_SoftwareInstallationServiceCapabilities_Provider.h"
#include "SF_SoftwareInstallationService_Provider.h"
#include "SF_EthernetPort_Provider.h"
#include "SF_PortController_Provider.h"
#include "SF_LANEndpoint_Provider.h"
#include "SF_EnabledLogicalElementCapabilities_Provider.h"
#include "SF_DiagnosticServiceCapabilities_Provider.h"
#include "SF_DiagnosticTest_Provider.h"
#include "sf_provider.h"

CIMPLE_NAMESPACE_BEGIN


SF_ElementCapabilities_Provider::SF_ElementCapabilities_Provider()
{
}

SF_ElementCapabilities_Provider::~SF_ElementCapabilities_Provider()
{
}

Load_Status SF_ElementCapabilities_Provider::load()
{
    solarflare::CIMHelper::initialize();
    return LOAD_OK;
}

Unload_Status SF_ElementCapabilities_Provider::unload()
{
    return UNLOAD_OK;
}

Get_Instance_Status SF_ElementCapabilities_Provider::get_instance(
    const SF_ElementCapabilities* model,
    SF_ElementCapabilities*& instance)
{
    return GET_INSTANCE_UNSUPPORTED;
}

Enum_Instances_Status SF_ElementCapabilities_Provider::enum_instances(
    const SF_ElementCapabilities* model,
    Enum_Instances_Handler<SF_ElementCapabilities>* handler)
{
    solarflare::EnumInstances<SF_ElementCapabilities>::allObjects(handler);
    return ENUM_INSTANCES_OK;
}

Create_Instance_Status SF_ElementCapabilities_Provider::create_instance(
    SF_ElementCapabilities* instance)
{
    return CREATE_INSTANCE_UNSUPPORTED;
}

Delete_Instance_Status SF_ElementCapabilities_Provider::delete_instance(
    const SF_ElementCapabilities* instance)
{
    return DELETE_INSTANCE_UNSUPPORTED;
}

Modify_Instance_Status SF_ElementCapabilities_Provider::modify_instance(
    const SF_ElementCapabilities* model,
    const SF_ElementCapabilities* instance)
{
    return MODIFY_INSTANCE_UNSUPPORTED;
}

Enum_Associator_Names_Status SF_ElementCapabilities_Provider::enum_associator_names(
    const Instance* instance,
    const String& result_class,
    const String& role,
    const String& result_role,
    Enum_Associator_Names_Handler<Instance>* handler)
{
    return ENUM_ASSOCIATOR_NAMES_UNSUPPORTED;
}

Enum_Associators_Status SF_ElementCapabilities_Provider::enum_associators(
    const Instance* instance,
    const String& result_class,
    const String& role,
    const String& result_role,
    Enum_Associators_Handler<Instance>* handler)
{
    return ENUM_ASSOCIATORS_UNSUPPORTED;
}

Enum_References_Status SF_ElementCapabilities_Provider::enum_references(
    const Instance* instance,
    const SF_ElementCapabilities* model,
    const String& role,
    Enum_References_Handler<SF_ElementCapabilities>* handler)
{
    return ENUM_REFERENCES_UNSUPPORTED;
}

/*@END@*/

CIMPLE_NAMESPACE_END
