/***************************************************************************//*! \file liprovider/SF_FirmwareSoftwareIdentity_Provider.h
** <L5_PRIVATE L5_SOURCE>
** \author  OktetLabs
**  \brief  CIM Provider
**   \date  2013/10/02
**    \cop  (c) Solarflare Communications Inc.
** </L5_PRIVATE>
*//*
\**************************************************************************/

// Generated by genprov 2.0.24
#ifndef _SF_FirmwareSoftwareIdentity_Provider_h
#define _SF_FirmwareSoftwareIdentity_Provider_h

#include <cimple/cimple.h>
#include "SF_FirmwareSoftwareIdentity.h"
#include "sf_provider.h"

CIMPLE_NAMESPACE_BEGIN

class SF_FirmwareSoftwareIdentity_Provider
{
public:
    typedef SF_FirmwareSoftwareIdentity Class;

    SF_FirmwareSoftwareIdentity_Provider();

    ~SF_FirmwareSoftwareIdentity_Provider();

    Load_Status load();

    Unload_Status unload();

    Get_Instance_Status get_instance(
        const SF_FirmwareSoftwareIdentity* model,
        SF_FirmwareSoftwareIdentity*& instance);

    Enum_Instances_Status enum_instances(
        const SF_FirmwareSoftwareIdentity* model,
        Enum_Instances_Handler<SF_FirmwareSoftwareIdentity>* handler);

    Create_Instance_Status create_instance(
        SF_FirmwareSoftwareIdentity* instance);

    Delete_Instance_Status delete_instance(
        const SF_FirmwareSoftwareIdentity* instance);

    Modify_Instance_Status modify_instance(
        const SF_FirmwareSoftwareIdentity* model,
        const SF_FirmwareSoftwareIdentity* instance);

    /*@END@*/
};

CIMPLE_NAMESPACE_END

#endif /* _SF_FirmwareSoftwareIdentity_Provider_h */