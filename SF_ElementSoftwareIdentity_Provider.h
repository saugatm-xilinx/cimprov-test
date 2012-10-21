// Generated by genprov 2.0.24
#ifndef _SF_ElementSoftwareIdentity_Provider_h
#define _SF_ElementSoftwareIdentity_Provider_h

#include <cimple/cimple.h>
#include "SF_ElementSoftwareIdentity.h"
#include "sf_platform.h"

CIMPLE_NAMESPACE_BEGIN

class SF_ElementSoftwareIdentity_Provider
{
    class SWEnum : public solarflare::ConstSoftwareEnumerator {
        Enum_Instances_Handler<SF_ElementSoftwareIdentity>* const handler;
    public:
        SWEnum(Enum_Instances_Handler<SF_ElementSoftwareIdentity>* h) :
            handler(h) {};
        virtual bool process(const solarflare::SWElement& se);
    };

    class NICBinder : public solarflare::ConstNICEnumerator {
        Enum_Instances_Handler<SF_ElementSoftwareIdentity>* const handler;
        const solarflare::SWElement *const softItem;
    public:
        NICBinder(Enum_Instances_Handler<SF_ElementSoftwareIdentity>* h,
                  const solarflare::SWElement *s) :
            handler(h), softItem(s) {};
        virtual bool process(const solarflare::NIC& nic);
    };

public:

    typedef SF_ElementSoftwareIdentity Class;

    SF_ElementSoftwareIdentity_Provider();

    ~SF_ElementSoftwareIdentity_Provider();

    Load_Status load();

    Unload_Status unload();

    Get_Instance_Status get_instance(
        const SF_ElementSoftwareIdentity* model,
        SF_ElementSoftwareIdentity*& instance);

    Enum_Instances_Status enum_instances(
        const SF_ElementSoftwareIdentity* model,
        Enum_Instances_Handler<SF_ElementSoftwareIdentity>* handler);

    Create_Instance_Status create_instance(
        SF_ElementSoftwareIdentity* instance);

    Delete_Instance_Status delete_instance(
        const SF_ElementSoftwareIdentity* instance);

    Modify_Instance_Status modify_instance(
        const SF_ElementSoftwareIdentity* model,
        const SF_ElementSoftwareIdentity* instance);

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
        const SF_ElementSoftwareIdentity* model,
        const String& role,
        Enum_References_Handler<SF_ElementSoftwareIdentity>* handler);

    /*@END@*/
};

CIMPLE_NAMESPACE_END

#endif /* _SF_ElementSoftwareIdentity_Provider_h */
