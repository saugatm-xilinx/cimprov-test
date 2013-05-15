// Generated by genprov 2.0.24
#ifndef _SF_LANEndpoint_Provider_h
#define _SF_LANEndpoint_Provider_h

#include <cimple/cimple.h>
#include "CIM_ConcreteJob.h"
#include "SF_LANEndpoint.h"
#include "sf_platform.h"

CIMPLE_NAMESPACE_BEGIN

class SF_LANEndpoint_Provider
{
public:

    typedef SF_LANEndpoint Class;

    SF_LANEndpoint_Provider();

    ~SF_LANEndpoint_Provider();

    Load_Status load();

    Unload_Status unload();

    Get_Instance_Status get_instance(
        const SF_LANEndpoint* model,
        SF_LANEndpoint*& instance);

    Enum_Instances_Status enum_instances(
        const SF_LANEndpoint* model,
        Enum_Instances_Handler<SF_LANEndpoint>* handler);

    Create_Instance_Status create_instance(
        SF_LANEndpoint* instance);

    Delete_Instance_Status delete_instance(
        const SF_LANEndpoint* instance);

    Modify_Instance_Status modify_instance(
        const SF_LANEndpoint* model,
        const SF_LANEndpoint* instance);

    Invoke_Method_Status RequestStateChange(
        const SF_LANEndpoint* self,
        const Property<uint16>& RequestedState,
        CIM_ConcreteJob*& Job,
        const Property<Datetime>& TimeoutPeriod,
        Property<uint32>& return_value);

    Invoke_Method_Status BroadcastReset(
        const SF_LANEndpoint* self,
        Property<uint32>& return_value);

    /*@END@*/
};

CIMPLE_NAMESPACE_END

#endif /* _SF_LANEndpoint_Provider_h */
