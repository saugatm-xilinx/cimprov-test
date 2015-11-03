/***************************************************************************//*! \file liprovider/SF_LibrarySoftwareIdentity_Provider.h
** <L5_PRIVATE L5_SOURCE>
** \author  OktetLabs
**  \brief  CIM Provider
**   \date  2013/10/02
**    \cop  (c) Solarflare Communications Inc.
** </L5_PRIVATE>
*//*
\**************************************************************************/

// Generated by genprov 2.0.24
#ifndef _SF_LibrarySoftwareIdentity_Provider_h
#define _SF_LibrarySoftwareIdentity_Provider_h

#include <cimple/cimple.h>
#include "SF_LibrarySoftwareIdentity.h"
#include "sf_provider.h"

CIMPLE_NAMESPACE_BEGIN

class SF_LibrarySoftwareIdentity_Provider
{
public:
    typedef SF_LibrarySoftwareIdentity Class;

    SF_LibrarySoftwareIdentity_Provider();

    ~SF_LibrarySoftwareIdentity_Provider();

    Load_Status load();

    Unload_Status unload();

    Get_Instance_Status get_instance(
        const SF_LibrarySoftwareIdentity* model,
        SF_LibrarySoftwareIdentity*& instance);

    Enum_Instances_Status enum_instances(
        const SF_LibrarySoftwareIdentity* model,
        Enum_Instances_Handler<SF_LibrarySoftwareIdentity>* handler);

    Create_Instance_Status create_instance(
        SF_LibrarySoftwareIdentity* instance);

    Delete_Instance_Status delete_instance(
        const SF_LibrarySoftwareIdentity* instance);

    Modify_Instance_Status modify_instance(
        const SF_LibrarySoftwareIdentity* model,
        const SF_LibrarySoftwareIdentity* instance);

    /*@END@*/
};

CIMPLE_NAMESPACE_END

#endif /* _SF_LibrarySoftwareIdentity_Provider_h */