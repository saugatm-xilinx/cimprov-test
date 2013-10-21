/***************************************************************************//*! \file liprovider/SF_LogManagesRecord_Provider.h
** <L5_PRIVATE L5_SOURCE>
** \author  OktetLabs
**  \brief  CIM Provider
**   \date  2013/10/02
**    \cop  (c) Solarflare Communications Inc.
** </L5_PRIVATE>
*//*
\**************************************************************************/

// Generated by genprov 2.0.24
#ifndef _SF_LogManagesRecord_Provider_h
#define _SF_LogManagesRecord_Provider_h

#include <cimple/cimple.h>
#include "SF_LogManagesRecord.h"
#include "sf_platform.h"

CIMPLE_NAMESPACE_BEGIN

class SF_LogManagesRecord_Provider
{    
public:

    typedef SF_LogManagesRecord Class;

    SF_LogManagesRecord_Provider();

    ~SF_LogManagesRecord_Provider();

    Load_Status load();

    Unload_Status unload();

    Get_Instance_Status get_instance(
        const SF_LogManagesRecord* model,
        SF_LogManagesRecord*& instance);

    Enum_Instances_Status enum_instances(
        const SF_LogManagesRecord* model,
        Enum_Instances_Handler<SF_LogManagesRecord>* handler);

    Create_Instance_Status create_instance(
        SF_LogManagesRecord* instance);

    Delete_Instance_Status delete_instance(
        const SF_LogManagesRecord* instance);

    Modify_Instance_Status modify_instance(
        const SF_LogManagesRecord* model,
        const SF_LogManagesRecord* instance);

    Enum_Associator_Names_Status enum_associator_names(
        const Instance* instance,
        const String& result_class,
        const String& role,
        const String& result_role,
        Enum_Associator_Names_Handler<Instance>* handler);

    Enum_Associators_Status enum_associators(
        const Instance* instance,
        const String& result_class,
        const String& role,
        const String& result_role,
        Enum_Associators_Handler<Instance>* handler);

    Enum_References_Status enum_references(
        const Instance* instance,
        const SF_LogManagesRecord* model,
        const String& role,
        Enum_References_Handler<SF_LogManagesRecord>* handler);

    /*@END@*/
};

CIMPLE_NAMESPACE_END

#endif /* _SF_LogManagesRecord_Provider_h */
