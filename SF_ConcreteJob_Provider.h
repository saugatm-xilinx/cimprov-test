// Generated by genprov 2.0.24
#ifndef _SF_ConcreteJob_Provider_h
#define _SF_ConcreteJob_Provider_h

#include <cimple/cimple.h>
#include "SF_ConcreteJob.h"
#include "sf_platform.h"

CIMPLE_NAMESPACE_BEGIN

class SF_ConcreteJob_Provider
{
public:

    typedef SF_ConcreteJob Class;

    SF_ConcreteJob_Provider();

    ~SF_ConcreteJob_Provider();

    Load_Status load();

    Unload_Status unload();

    Get_Instance_Status get_instance(
        const SF_ConcreteJob* model,
        SF_ConcreteJob*& instance);

    Enum_Instances_Status enum_instances(
        const SF_ConcreteJob* model,
        Enum_Instances_Handler<SF_ConcreteJob>* handler);

    Create_Instance_Status create_instance(
        SF_ConcreteJob* instance);

    Delete_Instance_Status delete_instance(
        const SF_ConcreteJob* instance);

    Modify_Instance_Status modify_instance(
        const SF_ConcreteJob* model,
        const SF_ConcreteJob* instance);

    Invoke_Method_Status KillJob(
        const SF_ConcreteJob* self,
        const Property<boolean>& DeleteOnKill,
        Property<uint32>& return_value);

    Invoke_Method_Status RequestStateChange(
        const SF_ConcreteJob* self,
        const Property<uint16>& RequestedState,
        const Property<Datetime>& TimeoutPeriod,
        Property<uint32>& return_value);

    Invoke_Method_Status GetError(
        const SF_ConcreteJob* self,
        CIM_Error*& Error,
        Property<uint32>& return_value);

    /*@END@*/
};

CIMPLE_NAMESPACE_END

#endif /* _SF_ConcreteJob_Provider_h */
