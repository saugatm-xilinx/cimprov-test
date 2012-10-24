// Generated by genprov 2.0.24
#ifndef _SF_DiagnosticLog_Provider_h
#define _SF_DiagnosticLog_Provider_h

#include <cimple/cimple.h>
#include "SF_DiagnosticLog.h"

CIMPLE_NAMESPACE_BEGIN

class SF_DiagnosticLog_Provider
{
public:

    typedef SF_DiagnosticLog Class;

    SF_DiagnosticLog_Provider();

    ~SF_DiagnosticLog_Provider();

    Load_Status load();

    Unload_Status unload();

    Get_Instance_Status get_instance(
        const SF_DiagnosticLog* model,
        SF_DiagnosticLog*& instance);

    Enum_Instances_Status enum_instances(
        const SF_DiagnosticLog* model,
        Enum_Instances_Handler<SF_DiagnosticLog>* handler);

    Create_Instance_Status create_instance(
        SF_DiagnosticLog* instance);

    Delete_Instance_Status delete_instance(
        const SF_DiagnosticLog* instance);

    Modify_Instance_Status modify_instance(
        const SF_DiagnosticLog* model,
        const SF_DiagnosticLog* instance);

    Invoke_Method_Status RequestStateChange(
        const SF_DiagnosticLog* self,
        const Property<uint16>& RequestedState,
        CIM_ConcreteJob*& Job,
        const Property<Datetime>& TimeoutPeriod,
        Property<uint32>& return_value);

    Invoke_Method_Status ClearLog(
        const SF_DiagnosticLog* self,
        Property<uint32>& return_value);

    /*@END@*/
};

CIMPLE_NAMESPACE_END

#endif /* _SF_DiagnosticLog_Provider_h */
