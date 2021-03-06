/***************************************************************************//*! \file liprovider/SF_FirmwareSoftwareIdentity_Provider.cpp
** <L5_PRIVATE L5_SOURCE>
** \author  OktetLabs
**  \brief  CIM Provider
**   \date  2013/10/02
**    \cop  (c) Solarflare Communications Inc.
** </L5_PRIVATE>
*//*
\**************************************************************************/

// Generated by genprov 2.0.24
#include "SF_FirmwareSoftwareIdentity_Provider.h"
#include "sf_provider.h"

CIMPLE_NAMESPACE_BEGIN

using solarflare::System;
using solarflare::SWElement;

SF_FirmwareSoftwareIdentity_Provider::SF_FirmwareSoftwareIdentity_Provider()
{
}

SF_FirmwareSoftwareIdentity_Provider::~SF_FirmwareSoftwareIdentity_Provider()
{
}

Load_Status SF_FirmwareSoftwareIdentity_Provider::load()
{
    return LOAD_OK;
}

Unload_Status SF_FirmwareSoftwareIdentity_Provider::unload()
{
    return UNLOAD_OK;
}

Get_Instance_Status SF_FirmwareSoftwareIdentity_Provider::get_instance(
    const SF_FirmwareSoftwareIdentity* model,
    SF_FirmwareSoftwareIdentity*& instance)
{
    return GET_INSTANCE_UNSUPPORTED;
}

Enum_Instances_Status SF_FirmwareSoftwareIdentity_Provider::enum_instances(
    const SF_FirmwareSoftwareIdentity* model,
    Enum_Instances_Handler<SF_FirmwareSoftwareIdentity>* handler)
{
    solarflare::EnumInstances<SF_FirmwareSoftwareIdentity>::allSoftware(handler);
    return ENUM_INSTANCES_OK;
}

Create_Instance_Status SF_FirmwareSoftwareIdentity_Provider::create_instance(
    SF_FirmwareSoftwareIdentity* instance)
{
    return CREATE_INSTANCE_UNSUPPORTED;
}

Delete_Instance_Status SF_FirmwareSoftwareIdentity_Provider::delete_instance(
    const SF_FirmwareSoftwareIdentity* instance)
{
    return DELETE_INSTANCE_UNSUPPORTED;
}

Modify_Instance_Status SF_FirmwareSoftwareIdentity_Provider::modify_instance(
    const SF_FirmwareSoftwareIdentity* model,
    const SF_FirmwareSoftwareIdentity* instance)
{
    return MODIFY_INSTANCE_UNSUPPORTED;
}

/*@END@*/

CIMPLE_NAMESPACE_END
