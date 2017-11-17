/***************************************************************************//*! \file liprovider/SF_ReferencedProfile_Provider.cpp
** <L5_PRIVATE L5_SOURCE>
** \author  OktetLabs
**  \brief  CIM Provider
**   \date  2013/10/02
**    \cop  (c) Solarflare Communications Inc.
** </L5_PRIVATE>
*//*
\**************************************************************************/

// Generated by genprov 2.0.24
#include "SF_ReferencedProfile_Provider.h"
#include "SF_RegisteredProfile_Provider.h"

CIMPLE_NAMESPACE_BEGIN

SF_ReferencedProfile_Provider::SF_ReferencedProfile_Provider()
{
}

SF_ReferencedProfile_Provider::~SF_ReferencedProfile_Provider()
{
}

Load_Status SF_ReferencedProfile_Provider::load()
{
    return LOAD_OK;
}

Unload_Status SF_ReferencedProfile_Provider::unload()
{
    return UNLOAD_OK;
}

Get_Instance_Status SF_ReferencedProfile_Provider::get_instance(
    const SF_ReferencedProfile* model,
    SF_ReferencedProfile*& instance)
{
    return GET_INSTANCE_UNSUPPORTED;
}

Enum_Instances_Status SF_ReferencedProfile_Provider::enum_instances(
    const SF_ReferencedProfile* model,
    Enum_Instances_Handler<SF_ReferencedProfile>* handler)
{
#if (!TARGET_CIM_SERVER_esxi && !TARGET_CIM_SERVER_esxi_native)
    solarflare::EnumInstances<SF_ReferencedProfile>::topmost(handler);
#endif

    return ENUM_INSTANCES_OK;
}

Create_Instance_Status SF_ReferencedProfile_Provider::create_instance(
    SF_ReferencedProfile* instance)
{
    return CREATE_INSTANCE_UNSUPPORTED;
}

Delete_Instance_Status SF_ReferencedProfile_Provider::delete_instance(
    const SF_ReferencedProfile* instance)
{
    return DELETE_INSTANCE_UNSUPPORTED;
}

Modify_Instance_Status SF_ReferencedProfile_Provider::modify_instance(
    const SF_ReferencedProfile* model,
    const SF_ReferencedProfile* instance)
{
    return MODIFY_INSTANCE_UNSUPPORTED;
}

Enum_Associator_Names_Status SF_ReferencedProfile_Provider::enum_associator_names(
    const Instance* instance,
    const String& result_class,
    const String& role,
    const String& result_role,
    Enum_Associator_Names_Handler<Instance>* handler)
{
    return ENUM_ASSOCIATOR_NAMES_UNSUPPORTED;
}

Enum_Associators_Status SF_ReferencedProfile_Provider::enum_associators(
    const Instance* instance,
    const String& result_class,
    const String& role,
    const String& result_role,
    Enum_Associators_Handler<Instance>* handler)
{
    return ENUM_ASSOCIATORS_UNSUPPORTED;
}

Enum_References_Status SF_ReferencedProfile_Provider::enum_references(
    const Instance* instance,
    const SF_ReferencedProfile* model,
    const String& role,
    Enum_References_Handler<SF_ReferencedProfile>* handler)
{
    return ENUM_REFERENCES_UNSUPPORTED;
}

/*@END@*/

CIMPLE_NAMESPACE_END
