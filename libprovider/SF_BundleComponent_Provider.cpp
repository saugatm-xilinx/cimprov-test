/***************************************************************************//*! \file liprovider/SF_BundleComponent_Provider.cpp
** <L5_PRIVATE L5_SOURCE>
** \author  OktetLabs
**  \brief  CIM Provider
**   \date  2013/10/02
**    \cop  (c) Solarflare Communications Inc.
** </L5_PRIVATE>
*//*
\**************************************************************************/

// Generated by genprov 2.0.24
#include "SF_BundleComponent_Provider.h"
#include "sf_provider.h"

CIMPLE_NAMESPACE_BEGIN


SF_BundleComponent_Provider::SF_BundleComponent_Provider()
{
}

SF_BundleComponent_Provider::~SF_BundleComponent_Provider()
{
}

Load_Status SF_BundleComponent_Provider::load()
{
    solarflare::CIMHelper::initialize();
    return LOAD_OK;
}

Unload_Status SF_BundleComponent_Provider::unload()
{
    return UNLOAD_OK;
}

Get_Instance_Status SF_BundleComponent_Provider::get_instance(
    const SF_BundleComponent* model,
    SF_BundleComponent*& instance)
{
    return GET_INSTANCE_UNSUPPORTED;
}

Enum_Instances_Status SF_BundleComponent_Provider::enum_instances(
    const SF_BundleComponent* model,
    Enum_Instances_Handler<SF_BundleComponent>* handler)
{
#if (!TARGET_CIM_SERVER_esxi && !TARGET_CIM_SERVER_esxi_native)
    solarflare::EnumInstances<SF_BundleComponent>::allSoftware(handler);
#endif
    
    return ENUM_INSTANCES_OK;
}

Create_Instance_Status SF_BundleComponent_Provider::create_instance(
    SF_BundleComponent* instance)
{
    return CREATE_INSTANCE_UNSUPPORTED;
}

Delete_Instance_Status SF_BundleComponent_Provider::delete_instance(
    const SF_BundleComponent* instance)
{
    return DELETE_INSTANCE_UNSUPPORTED;
}

Modify_Instance_Status SF_BundleComponent_Provider::modify_instance(
    const SF_BundleComponent* model,
    const SF_BundleComponent* instance)
{
    return MODIFY_INSTANCE_UNSUPPORTED;
}

Enum_Associator_Names_Status SF_BundleComponent_Provider::enum_associator_names(
    const Instance* instance,
    const String& result_class,
    const String& role,
    const String& result_role,
    Enum_Associator_Names_Handler<Instance>* handler)
{
    return ENUM_ASSOCIATOR_NAMES_UNSUPPORTED;
}

Enum_Associators_Status SF_BundleComponent_Provider::enum_associators(
    const Instance* instance,
    const String& result_class,
    const String& role,
    const String& result_role,
    Enum_Associators_Handler<Instance>* handler)
{
    return ENUM_ASSOCIATORS_UNSUPPORTED;
}

Enum_References_Status SF_BundleComponent_Provider::enum_references(
    const Instance* instance,
    const SF_BundleComponent* model,
    const String& role,
    Enum_References_Handler<SF_BundleComponent>* handler)
{
    return ENUM_REFERENCES_UNSUPPORTED;
}

/*@END@*/

CIMPLE_NAMESPACE_END
