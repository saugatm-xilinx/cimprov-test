// Generated by genprov 2.0.24
#ifndef _SF_ElementCapabilities_Provider_h
#define _SF_ElementCapabilities_Provider_h

#include <cimple/cimple.h>
#include "SF_ElementCapabilities.h"
#include "sf_platform.h"

CIMPLE_NAMESPACE_BEGIN

class SF_ElementCapabilities_Provider
{
    class NICEnum : public solarflare::ConstElementEnumerator
    {
        Enum_Instances_Handler<SF_ElementCapabilities>* handler;
    public:
        NICEnum(Enum_Instances_Handler<SF_ElementCapabilities>* h) :
            handler(h) {}
        virtual bool process(const solarflare::SystemElement& se);
    };


    class DiagEnum : public solarflare::ConstElementEnumerator
    {
        Enum_Instances_Handler<SF_ElementCapabilities>* handler;
    public:
        DiagEnum(Enum_Instances_Handler<SF_ElementCapabilities>* h) :
            handler(h) {}
        virtual bool process(const solarflare::SystemElement& se);
    };

    class IntfEnum : public solarflare::ConstElementEnumerator
    {
        Enum_Instances_Handler<SF_ElementCapabilities>* handler;
    public:
        IntfEnum(Enum_Instances_Handler<SF_ElementCapabilities>* h) :
            handler(h) {}
        virtual bool process(const solarflare::SystemElement& se);
    };

    class SWEnum : public solarflare::ConstElementEnumerator
    {
        Enum_Instances_Handler<SF_ElementCapabilities>* handler;
    public:
        SWEnum(Enum_Instances_Handler<SF_ElementCapabilities>* h) :
            handler(h) {}
        virtual bool process(const solarflare::SystemElement& se);
    };


public:

    typedef SF_ElementCapabilities Class;

    SF_ElementCapabilities_Provider();

    ~SF_ElementCapabilities_Provider();

    Load_Status load();

    Unload_Status unload();

    Get_Instance_Status get_instance(
        const SF_ElementCapabilities* model,
        SF_ElementCapabilities*& instance);

    Enum_Instances_Status enum_instances(
        const SF_ElementCapabilities* model,
        Enum_Instances_Handler<SF_ElementCapabilities>* handler);

    Create_Instance_Status create_instance(
        SF_ElementCapabilities* instance);

    Delete_Instance_Status delete_instance(
        const SF_ElementCapabilities* instance);

    Modify_Instance_Status modify_instance(
        const SF_ElementCapabilities* model,
        const SF_ElementCapabilities* instance);

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
        const SF_ElementCapabilities* model,
        const String& role,
        Enum_References_Handler<SF_ElementCapabilities>* handler);

    /*@END@*/
};

CIMPLE_NAMESPACE_END

#endif /* _SF_ElementCapabilities_Provider_h */
