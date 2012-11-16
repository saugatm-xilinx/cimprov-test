// Generated by genprov 2.0.24
#ifndef _SF_HostedService_Provider_h
#define _SF_HostedService_Provider_h

#include <cimple/cimple.h>
#include "SF_HostedService.h"
#include "sf_platform.h"

CIMPLE_NAMESPACE_BEGIN

class SF_HostedService_Provider
{
public:

    typedef SF_HostedService Class;

    SF_HostedService_Provider();

    ~SF_HostedService_Provider();

    Load_Status load();

    Unload_Status unload();

    Get_Instance_Status get_instance(
        const SF_HostedService* model,
        SF_HostedService*& instance);

    Enum_Instances_Status enum_instances(
        const SF_HostedService* model,
        Enum_Instances_Handler<SF_HostedService>* handler);

    Create_Instance_Status create_instance(
        SF_HostedService* instance);

    Delete_Instance_Status delete_instance(
        const SF_HostedService* instance);

    Modify_Instance_Status modify_instance(
        const SF_HostedService* model,
        const SF_HostedService* instance);

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
        const SF_HostedService* model,
        const String& role,
        Enum_References_Handler<SF_HostedService>* handler);

    /*@END@*/
};

CIMPLE_NAMESPACE_END

#endif /* _SF_HostedService_Provider_h */
