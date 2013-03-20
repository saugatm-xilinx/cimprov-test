// Generated by genprov 2.0.24
#ifndef _SF_UseOfLog_Provider_h
#define _SF_UseOfLog_Provider_h

#include <cimple/cimple.h>
#include "SF_UseOfLog.h"
#include "sf_platform.h"

CIMPLE_NAMESPACE_BEGIN

class SF_UseOfLog_Provider
{
public:

    typedef SF_UseOfLog Class;

    SF_UseOfLog_Provider();

    ~SF_UseOfLog_Provider();

    Load_Status load();

    Unload_Status unload();

    Get_Instance_Status get_instance(
        const SF_UseOfLog* model,
        SF_UseOfLog*& instance);

    Enum_Instances_Status enum_instances(
        const SF_UseOfLog* model,
        Enum_Instances_Handler<SF_UseOfLog>* handler);

    Create_Instance_Status create_instance(
        SF_UseOfLog* instance);

    Delete_Instance_Status delete_instance(
        const SF_UseOfLog* instance);

    Modify_Instance_Status modify_instance(
        const SF_UseOfLog* model,
        const SF_UseOfLog* instance);

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
        const SF_UseOfLog* model,
        const String& role,
        Enum_References_Handler<SF_UseOfLog>* handler);

    /*@END@*/
};

CIMPLE_NAMESPACE_END

#endif /* _SF_UseOfLog_Provider_h */