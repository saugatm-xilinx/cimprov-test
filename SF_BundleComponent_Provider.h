// Generated by genprov 2.0.24
#ifndef _SF_BundleComponent_Provider_h
#define _SF_BundleComponent_Provider_h

#include <cimple/cimple.h>
#include "SF_BundleComponent.h"
#include "sf_platform.h"

CIMPLE_NAMESPACE_BEGIN

class SF_BundleComponent_Provider
{
    class ConstEnum : public solarflare::ConstSoftwareEnumerator {
        Enum_Instances_Handler<SF_BundleComponent>* handler;
        const solarflare::Package *master;
        unsigned idx;
    public:
        ConstEnum(Enum_Instances_Handler<SF_BundleComponent>* h) :
            handler(h), master(NULL), idx(0) {};
        virtual bool process(const solarflare::SWElement& e);
    };
public:

    typedef SF_BundleComponent Class;

    SF_BundleComponent_Provider();

    ~SF_BundleComponent_Provider();

    Load_Status load();

    Unload_Status unload();

    Get_Instance_Status get_instance(
        const SF_BundleComponent* model,
        SF_BundleComponent*& instance);

    Enum_Instances_Status enum_instances(
        const SF_BundleComponent* model,
        Enum_Instances_Handler<SF_BundleComponent>* handler);

    Create_Instance_Status create_instance(
        SF_BundleComponent* instance);

    Delete_Instance_Status delete_instance(
        const SF_BundleComponent* instance);

    Modify_Instance_Status modify_instance(
        const SF_BundleComponent* model,
        const SF_BundleComponent* instance);

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
        const SF_BundleComponent* model,
        const String& role,
        Enum_References_Handler<SF_BundleComponent>* handler);

    /*@END@*/
};

CIMPLE_NAMESPACE_END

#endif /* _SF_BundleComponent_Provider_h */
