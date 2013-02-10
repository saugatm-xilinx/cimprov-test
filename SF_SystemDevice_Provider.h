// Generated by genprov 2.0.24
#ifndef _SF_SystemDevice_Provider_h
#define _SF_SystemDevice_Provider_h

#include <cimple/cimple.h>
#include "SF_SystemDevice.h"
#include "sf_platform.h"

CIMPLE_NAMESPACE_BEGIN

class SF_SystemDevice_Provider
{

public:

    typedef SF_SystemDevice Class;

    SF_SystemDevice_Provider();

    ~SF_SystemDevice_Provider();

    Load_Status load();

    Unload_Status unload();

    Get_Instance_Status get_instance(
        const SF_SystemDevice* model,
        SF_SystemDevice*& instance);

    Enum_Instances_Status enum_instances(
        const SF_SystemDevice* model,
        Enum_Instances_Handler<SF_SystemDevice>* handler);

    Create_Instance_Status create_instance(
        SF_SystemDevice* instance);

    Delete_Instance_Status delete_instance(
        const SF_SystemDevice* instance);

    Modify_Instance_Status modify_instance(
        const SF_SystemDevice* model,
        const SF_SystemDevice* instance);

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
        const SF_SystemDevice* model,
        const String& role,
        Enum_References_Handler<SF_SystemDevice>* handler);

    /*@END@*/
};

CIMPLE_NAMESPACE_END

#endif /* _SF_SystemDevice_Provider_h */
