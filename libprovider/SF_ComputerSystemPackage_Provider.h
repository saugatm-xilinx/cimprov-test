// Generated by genprov 2.0.24
#ifndef _SF_ComputerSystemPackage_Provider_h
#define _SF_ComputerSystemPackage_Provider_h

#include <cimple/cimple.h>
#include "SF_ComputerSystemPackage.h"

CIMPLE_NAMESPACE_BEGIN

class SF_ComputerSystemPackage_Provider
{
public:

    typedef SF_ComputerSystemPackage Class;

    SF_ComputerSystemPackage_Provider();

    ~SF_ComputerSystemPackage_Provider();

    Load_Status load();

    Unload_Status unload();

    Get_Instance_Status get_instance(
        const SF_ComputerSystemPackage* model,
        SF_ComputerSystemPackage*& instance);

    Enum_Instances_Status enum_instances(
        const SF_ComputerSystemPackage* model,
        Enum_Instances_Handler<SF_ComputerSystemPackage>* handler);

    Create_Instance_Status create_instance(
        SF_ComputerSystemPackage* instance);

    Delete_Instance_Status delete_instance(
        const SF_ComputerSystemPackage* instance);

    Modify_Instance_Status modify_instance(
        const SF_ComputerSystemPackage* model,
        const SF_ComputerSystemPackage* instance);

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
        const SF_ComputerSystemPackage* model,
        const String& role,
        Enum_References_Handler<SF_ComputerSystemPackage>* handler);

    /*@END@*/
};

CIMPLE_NAMESPACE_END

#endif /* _SF_ComputerSystemPackage_Provider_h */
