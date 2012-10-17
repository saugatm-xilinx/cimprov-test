// Generated by genprov 2.0.24
#ifndef _SF_SoftwareIdentity_Provider_h
#define _SF_SoftwareIdentity_Provider_h

#include <cimple/cimple.h>
#include "SF_SoftwareIdentity.h"
#include "SF_Sysinfo.h"

CIMPLE_NAMESPACE_BEGIN

class SF_SoftwareIdentity_Provider
{
    static SF_SoftwareIdentity *swElement(const solarflare::SWElement& ve);
    static void addTargetOS(SF_SoftwareIdentity *id);
    static void addPackageType(SF_SoftwareIdentity *id, solarflare::Package::PkgType type);
    static SF_SoftwareIdentity *hostSoftware(const solarflare::SWElement& ve);

    class SWEnum : public solarflare::ConstSoftwareEnumerator {
        Enum_Instances_Handler<SF_SoftwareIdentity>* const handler;
    public:
        SWEnum(Enum_Instances_Handler<SF_SoftwareIdentity>* h) :
            handler(h) {};
        virtual bool process(const solarflare::SWElement& se);
    };
public:

    static SF_SoftwareIdentity *makeReference(const solarflare::SWElement& ve);

    typedef SF_SoftwareIdentity Class;

    SF_SoftwareIdentity_Provider();

    ~SF_SoftwareIdentity_Provider();

    Load_Status load();

    Unload_Status unload();

    Get_Instance_Status get_instance(
        const SF_SoftwareIdentity* model,
        SF_SoftwareIdentity*& instance);

    Enum_Instances_Status enum_instances(
        const SF_SoftwareIdentity* model,
        Enum_Instances_Handler<SF_SoftwareIdentity>* handler);

    Create_Instance_Status create_instance(
        SF_SoftwareIdentity* instance);

    Delete_Instance_Status delete_instance(
        const SF_SoftwareIdentity* instance);

    Modify_Instance_Status modify_instance(
        const SF_SoftwareIdentity* model,
        const SF_SoftwareIdentity* instance);

    /*@END@*/
};

CIMPLE_NAMESPACE_END

#endif /* _SF_SoftwareIdentity_Provider_h */
