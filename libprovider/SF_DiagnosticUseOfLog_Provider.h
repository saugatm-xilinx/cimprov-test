// Generated by genprov 2.0.24
#ifndef _SF_DiagnosticUseOfLog_Provider_h
#define _SF_DiagnosticUseOfLog_Provider_h

#include <cimple/cimple.h>
#include "SF_DiagnosticUseOfLog.h"
#include "sf_platform.h"

CIMPLE_NAMESPACE_BEGIN

class SF_DiagnosticUseOfLog_Provider
{
public:

    typedef SF_DiagnosticUseOfLog Class;

    SF_DiagnosticUseOfLog_Provider();

    ~SF_DiagnosticUseOfLog_Provider();

    Load_Status load();

    Unload_Status unload();

    Get_Instance_Status get_instance(
        const SF_DiagnosticUseOfLog* model,
        SF_DiagnosticUseOfLog*& instance);

    Enum_Instances_Status enum_instances(
        const SF_DiagnosticUseOfLog* model,
        Enum_Instances_Handler<SF_DiagnosticUseOfLog>* handler);

    Create_Instance_Status create_instance(
        SF_DiagnosticUseOfLog* instance);

    Delete_Instance_Status delete_instance(
        const SF_DiagnosticUseOfLog* instance);

    Modify_Instance_Status modify_instance(
        const SF_DiagnosticUseOfLog* model,
        const SF_DiagnosticUseOfLog* instance);

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
        const SF_DiagnosticUseOfLog* model,
        const String& role,
        Enum_References_Handler<SF_DiagnosticUseOfLog>* handler);

    /*@END@*/
};

CIMPLE_NAMESPACE_END

#endif /* _SF_DiagnosticUseOfLog_Provider_h */