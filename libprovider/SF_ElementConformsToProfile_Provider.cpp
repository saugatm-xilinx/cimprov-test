// Generated by genprov 2.0.24
#include "SF_ElementConformsToProfile_Provider.h"
#include "SF_SoftwareIdentity_Provider.h"
#include "SF_SoftwareInstallationService_Provider.h"
#include "SF_NICCard_Provider.h"
#include "SF_EthernetPort_Provider.h"
#include "SF_PhysicalConnector_Provider.h"
#include "SF_ConcreteJob_Provider.h"
#include "SF_DiagnosticTest_Provider.h"
#include "SF_ProviderLog_Provider.h"
#include "SF_RegisteredProfile_Provider.h"
#include "sf_provider.h"

CIMPLE_NAMESPACE_BEGIN

SF_ElementConformsToProfile_Provider::SF_ElementConformsToProfile_Provider()
{
}

SF_ElementConformsToProfile_Provider::~SF_ElementConformsToProfile_Provider()
{
}

Load_Status SF_ElementConformsToProfile_Provider::load()
{
    solarflare::CIMHelper::initialize();
    return LOAD_OK;
}

Unload_Status SF_ElementConformsToProfile_Provider::unload()
{
    return UNLOAD_OK;
}

Get_Instance_Status SF_ElementConformsToProfile_Provider::get_instance(
    const SF_ElementConformsToProfile* model,
    SF_ElementConformsToProfile*& instance)
{
    return GET_INSTANCE_UNSUPPORTED;
}

Enum_Instances_Status SF_ElementConformsToProfile_Provider::enum_instances(
    const SF_ElementConformsToProfile* model,
    Enum_Instances_Handler<SF_ElementConformsToProfile>* handler)
{
    solarflare::EnumInstances<SF_ElementConformsToProfile>::allObjects(handler);

    return ENUM_INSTANCES_OK;
}

Create_Instance_Status SF_ElementConformsToProfile_Provider::create_instance(
    SF_ElementConformsToProfile* instance)
{
    return CREATE_INSTANCE_UNSUPPORTED;
}

Delete_Instance_Status SF_ElementConformsToProfile_Provider::delete_instance(
    const SF_ElementConformsToProfile* instance)
{
    return DELETE_INSTANCE_UNSUPPORTED;
}

Modify_Instance_Status SF_ElementConformsToProfile_Provider::modify_instance(
    const SF_ElementConformsToProfile* model,
    const SF_ElementConformsToProfile* instance)
{
    return MODIFY_INSTANCE_UNSUPPORTED;
}

Enum_Associator_Names_Status SF_ElementConformsToProfile_Provider::enum_associator_names(
    const Instance* instance,
    const String& result_class,
    const String& role,
    const String& result_role,
    Enum_Associator_Names_Handler<Instance>* handler)
{
    return ENUM_ASSOCIATOR_NAMES_UNSUPPORTED;
}

Enum_Associators_Status SF_ElementConformsToProfile_Provider::enum_associators(
    const Instance* instance,
    const String& result_class,
    const String& role,
    const String& result_role,
    Enum_Associators_Handler<Instance>* handler)
{
    return ENUM_ASSOCIATORS_UNSUPPORTED;
}

Enum_References_Status SF_ElementConformsToProfile_Provider::enum_references(
    const Instance* instance,
    const SF_ElementConformsToProfile* model,
    const String& role,
    Enum_References_Handler<SF_ElementConformsToProfile>* handler)
{
    return ENUM_REFERENCES_UNSUPPORTED;
}

/*@END@*/

CIMPLE_NAMESPACE_END
