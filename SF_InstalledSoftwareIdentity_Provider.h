// Generated by genprov 2.0.24
#ifndef _SF_InstalledSoftwareIdentity_Provider_h
#define _SF_InstalledSoftwareIdentity_Provider_h

#include <cimple/cimple.h>
#include "SF_InstalledSoftwareIdentity.h"
#include "CIM_ComputerSystem.h"
#include "SF_Sysinfo.h"

CIMPLE_NAMESPACE_BEGIN

class SF_InstalledSoftwareIdentity_Provider
{
    class ConstEnum : public solarflare::ConstSoftwareEnumerator
    {
        const CIM_ComputerSystem *from;
        Enum_Instances_Handler<SF_InstalledSoftwareIdentity>* handler;
    public:
        ConstEnum(const CIM_ComputerSystem *f,
                  Enum_Instances_Handler<SF_InstalledSoftwareIdentity>* h) :
            from(f), handler(h) {};
        virtual bool process(const solarflare::SWElement& e);
    };
public:

    typedef SF_InstalledSoftwareIdentity Class;

    SF_InstalledSoftwareIdentity_Provider();

    ~SF_InstalledSoftwareIdentity_Provider();

    Load_Status load();

    Unload_Status unload();

    Get_Instance_Status get_instance(
        const SF_InstalledSoftwareIdentity* model,
        SF_InstalledSoftwareIdentity*& instance);

    Enum_Instances_Status enum_instances(
        const SF_InstalledSoftwareIdentity* model,
        Enum_Instances_Handler<SF_InstalledSoftwareIdentity>* handler);

    Create_Instance_Status create_instance(
        SF_InstalledSoftwareIdentity* instance);

    Delete_Instance_Status delete_instance(
        const SF_InstalledSoftwareIdentity* instance);

    Modify_Instance_Status modify_instance(
        const SF_InstalledSoftwareIdentity* model,
        const SF_InstalledSoftwareIdentity* instance);

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
        const SF_InstalledSoftwareIdentity* model,
        const String& role,
        Enum_References_Handler<SF_InstalledSoftwareIdentity>* handler);

    /*@END@*/
};

CIMPLE_NAMESPACE_END

#endif /* _SF_InstalledSoftwareIdentity_Provider_h */
