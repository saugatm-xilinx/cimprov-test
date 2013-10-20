/***************************************************************************//*! \file liprovider/SF_EnabledLogicalElementCapabilities_Provider.cpp
** <L5_PRIVATE L5_SOURCE>
** \author  OktetLabs
**  \brief  CIM Provider
**   \date  2013/10/02
**    \cop  (c) Solarflare Communications Inc.
** </L5_PRIVATE>
*//*
\**************************************************************************/

// Generated by genprov 2.0.24
#include "SF_EnabledLogicalElementCapabilities_Provider.h"
#include "sf_provider.h"

CIMPLE_NAMESPACE_BEGIN

SF_EnabledLogicalElementCapabilities_Provider::SF_EnabledLogicalElementCapabilities_Provider()
{
}

SF_EnabledLogicalElementCapabilities_Provider::~SF_EnabledLogicalElementCapabilities_Provider()
{
}

Load_Status SF_EnabledLogicalElementCapabilities_Provider::load()
{
    solarflare::CIMHelper::initialize();
    return LOAD_OK;
}

Unload_Status SF_EnabledLogicalElementCapabilities_Provider::unload()
{
    return UNLOAD_OK;
}

Get_Instance_Status SF_EnabledLogicalElementCapabilities_Provider::get_instance(
    const SF_EnabledLogicalElementCapabilities* model,
    SF_EnabledLogicalElementCapabilities*& instance)
{
    return GET_INSTANCE_UNSUPPORTED;
}

Enum_Instances_Status SF_EnabledLogicalElementCapabilities_Provider::enum_instances(
    const SF_EnabledLogicalElementCapabilities* model,
    Enum_Instances_Handler<SF_EnabledLogicalElementCapabilities>* handler)
{
    solarflare::EnumInstances<SF_EnabledLogicalElementCapabilities>::allObjects(handler);
    return ENUM_INSTANCES_OK;
}

Create_Instance_Status SF_EnabledLogicalElementCapabilities_Provider::create_instance(
    SF_EnabledLogicalElementCapabilities* instance)
{
    return CREATE_INSTANCE_UNSUPPORTED;
}

Delete_Instance_Status SF_EnabledLogicalElementCapabilities_Provider::delete_instance(
    const SF_EnabledLogicalElementCapabilities* instance)
{
    return DELETE_INSTANCE_UNSUPPORTED;
}

Modify_Instance_Status SF_EnabledLogicalElementCapabilities_Provider::modify_instance(
    const SF_EnabledLogicalElementCapabilities* model,
    const SF_EnabledLogicalElementCapabilities* instance)
{
    return MODIFY_INSTANCE_UNSUPPORTED;
}

#if CIM_SCHEMA_VERSION_MINOR < 28
Invoke_Method_Status SF_EnabledLogicalElementCapabilities_Provider::CreateGoalSettings(
    const SF_EnabledLogicalElementCapabilities* self,
    const Property< Array<CIM_SettingData*> >& TemplateGoalSettings,
    Property< Array<CIM_SettingData*> >& SupportedGoalSettings,
    Property<uint16>& return_value)

{
    return INVOKE_METHOD_UNSUPPORTED;
}
#endif

/*@END@*/

CIMPLE_NAMESPACE_END
