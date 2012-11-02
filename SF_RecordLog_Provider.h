// Generated by genprov 2.0.24
#ifndef _SF_RecordLog_Provider_h
#define _SF_RecordLog_Provider_h

#include <cimple/cimple.h>
#include "SF_RecordLog.h"
#include "sf_logging.h"

CIMPLE_NAMESPACE_BEGIN

class SF_RecordLog_Provider
{
    static SF_RecordLog *makeInstance(const solarflare::Logger& log);
public:

    static SF_RecordLog *makeReference(const solarflare::Logger& log);
    static solarflare::Logger *findByInstance(const SF_RecordLog& instance);

    typedef SF_RecordLog Class;

    SF_RecordLog_Provider();

    ~SF_RecordLog_Provider();

    Load_Status load();

    Unload_Status unload();

    Get_Instance_Status get_instance(
        const SF_RecordLog* model,
        SF_RecordLog*& instance);

    Enum_Instances_Status enum_instances(
        const SF_RecordLog* model,
        Enum_Instances_Handler<SF_RecordLog>* handler);

    Create_Instance_Status create_instance(
        SF_RecordLog* instance);

    Delete_Instance_Status delete_instance(
        const SF_RecordLog* instance);

    Modify_Instance_Status modify_instance(
        const SF_RecordLog* model,
        const SF_RecordLog* instance);

    Invoke_Method_Status RequestStateChange(
        const SF_RecordLog* self,
        const Property<uint16>& RequestedState,
        CIM_ConcreteJob*& Job,
        const Property<Datetime>& TimeoutPeriod,
        Property<uint32>& return_value);

    Invoke_Method_Status ClearLog(
        const SF_RecordLog* self,
        Property<uint32>& return_value);

    /*@END@*/
};

CIMPLE_NAMESPACE_END

#endif /* _SF_RecordLog_Provider_h */