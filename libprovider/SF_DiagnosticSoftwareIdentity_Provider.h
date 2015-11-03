/***************************************************************************//*! \file liprovider/SF_DiagnosticSoftwareIdentity_Provider.h
** <L5_PRIVATE L5_SOURCE>
** \author  OktetLabs
**  \brief  CIM Provider
**   \date  2013/10/02
**    \cop  (c) Solarflare Communications Inc.
** </L5_PRIVATE>
*//*
\**************************************************************************/

// Generated by genprov 2.0.24
#ifndef _SF_DiagnosticSoftwareIdentity_Provider_h
#define _SF_DiagnosticSoftwareIdentity_Provider_h

#include <cimple/cimple.h>
#include "SF_DiagnosticSoftwareIdentity.h"
#include "sf_provider.h"

CIMPLE_NAMESPACE_BEGIN

class SF_DiagnosticSoftwareIdentity_Provider
{
public:
    typedef SF_DiagnosticSoftwareIdentity Class;

    SF_DiagnosticSoftwareIdentity_Provider();

    ~SF_DiagnosticSoftwareIdentity_Provider();

    Load_Status load();

    Unload_Status unload();

    Get_Instance_Status get_instance(
        const SF_DiagnosticSoftwareIdentity* model,
        SF_DiagnosticSoftwareIdentity*& instance);

    Enum_Instances_Status enum_instances(
        const SF_DiagnosticSoftwareIdentity* model,
        Enum_Instances_Handler<SF_DiagnosticSoftwareIdentity>* handler);

    Create_Instance_Status create_instance(
        SF_DiagnosticSoftwareIdentity* instance);

    Delete_Instance_Status delete_instance(
        const SF_DiagnosticSoftwareIdentity* instance);

    Modify_Instance_Status modify_instance(
        const SF_DiagnosticSoftwareIdentity* model,
        const SF_DiagnosticSoftwareIdentity* instance);

    /*@END@*/
};

CIMPLE_NAMESPACE_END

#endif /* _SF_DiagnosticSoftwareIdentity_Provider_h */