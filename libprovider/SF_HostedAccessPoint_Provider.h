// Generated by genprov 2.0.24
#ifndef _SF_HostedAccessPoint_Provider_h
#define _SF_HostedAccessPoint_Provider_h

#include <cimple/cimple.h>
#include "SF_HostedAccessPoint.h"
#include "sf_platform.h"

CIMPLE_NAMESPACE_BEGIN

class SF_HostedAccessPoint_Provider
{
public:

    typedef SF_HostedAccessPoint Class;

    SF_HostedAccessPoint_Provider();

    ~SF_HostedAccessPoint_Provider();

    Load_Status load();

    Unload_Status unload();

    Get_Instance_Status get_instance(
        const SF_HostedAccessPoint* model,
        SF_HostedAccessPoint*& instance);

    Enum_Instances_Status enum_instances(
        const SF_HostedAccessPoint* model,
        Enum_Instances_Handler<SF_HostedAccessPoint>* handler);

    Create_Instance_Status create_instance(
        SF_HostedAccessPoint* instance);

    Delete_Instance_Status delete_instance(
        const SF_HostedAccessPoint* instance);

    Modify_Instance_Status modify_instance(
        const SF_HostedAccessPoint* model,
        const SF_HostedAccessPoint* instance);

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
        const SF_HostedAccessPoint* model,
        const String& role,
        Enum_References_Handler<SF_HostedAccessPoint>* handler);

    /*@END@*/
};

CIMPLE_NAMESPACE_END

#endif /* _SF_HostedAccessPoint_Provider_h */