// Generated by genprov 2.0.24
#ifndef _SF_OwningJobElement_Provider_h
#define _SF_OwningJobElement_Provider_h

#include <cimple/cimple.h>
#include "SF_OwningJobElement.h"
#include "sf_platform.h"

CIMPLE_NAMESPACE_BEGIN

class SF_OwningJobElement_Provider
{
    class Enum : public solarflare::ConstDiagnosticEnumerator {
        Enum_Instances_Handler<SF_OwningJobElement>* handler;
    public:
        Enum(Enum_Instances_Handler<SF_OwningJobElement>* h) :
            handler(h) {}
        virtual bool process(const solarflare::Diagnostic& diag);
    };
public:

    typedef SF_OwningJobElement Class;

    SF_OwningJobElement_Provider();

    ~SF_OwningJobElement_Provider();

    Load_Status load();

    Unload_Status unload();

    Get_Instance_Status get_instance(
        const SF_OwningJobElement* model,
        SF_OwningJobElement*& instance);

    Enum_Instances_Status enum_instances(
        const SF_OwningJobElement* model,
        Enum_Instances_Handler<SF_OwningJobElement>* handler);

    Create_Instance_Status create_instance(
        SF_OwningJobElement* instance);

    Delete_Instance_Status delete_instance(
        const SF_OwningJobElement* instance);

    Modify_Instance_Status modify_instance(
        const SF_OwningJobElement* model,
        const SF_OwningJobElement* instance);

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
        const SF_OwningJobElement* model,
        const String& role,
        Enum_References_Handler<SF_OwningJobElement>* handler);

    /*@END@*/
};

CIMPLE_NAMESPACE_END

#endif /* _SF_OwningJobElement_Provider_h */
