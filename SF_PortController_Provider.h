// Generated by genprov 2.0.24
#ifndef _SF_PortController_Provider_h
#define _SF_PortController_Provider_h

#include <cimple/cimple.h>
#include "SF_PortController.h"
#include "sf_platform.h"

CIMPLE_NAMESPACE_BEGIN

class SF_PortController_Provider
{
    class NICEnum : public solarflare::ConstElementEnumerator
    {
        Enum_Instances_Handler<SF_PortController>* handler;
    public:
        NICEnum(Enum_Instances_Handler<SF_PortController>* h) :
            handler(h) {}
        virtual bool process(const solarflare::SystemElement& nic);
    };


    class PortCounter : public solarflare::ConstElementEnumerator
    {
        unsigned cnt;
    public:
        PortCounter() : cnt(0) {};
        virtual bool process(const solarflare::SystemElement&) { cnt++; return true; }
        unsigned count() const { return cnt; }
    };
public:

    static SF_PortController *makeReference(const solarflare::NIC& nic);

    typedef SF_PortController Class;

    SF_PortController_Provider();

    ~SF_PortController_Provider();

    Load_Status load();

    Unload_Status unload();

    Get_Instance_Status get_instance(
        const SF_PortController* model,
        SF_PortController*& instance);

    Enum_Instances_Status enum_instances(
        const SF_PortController* model,
        Enum_Instances_Handler<SF_PortController>* handler);

    Create_Instance_Status create_instance(
        SF_PortController* instance);

    Delete_Instance_Status delete_instance(
        const SF_PortController* instance);

    Modify_Instance_Status modify_instance(
        const SF_PortController* model,
        const SF_PortController* instance);

    Invoke_Method_Status RequestStateChange(
        const SF_PortController* self,
        const Property<uint16>& RequestedState,
        CIM_ConcreteJob*& Job,
        const Property<Datetime>& TimeoutPeriod,
        Property<uint32>& return_value);

    Invoke_Method_Status SetPowerState(
        const SF_PortController* self,
        const Property<uint16>& PowerState,
        const Property<Datetime>& Time,
        Property<uint32>& return_value);

    Invoke_Method_Status Reset(
        const SF_PortController* self,
        Property<uint32>& return_value);

    Invoke_Method_Status EnableDevice(
        const SF_PortController* self,
        const Property<boolean>& Enabled,
        Property<uint32>& return_value);

    Invoke_Method_Status OnlineDevice(
        const SF_PortController* self,
        const Property<boolean>& Online,
        Property<uint32>& return_value);

    Invoke_Method_Status QuiesceDevice(
        const SF_PortController* self,
        const Property<boolean>& Quiesce,
        Property<uint32>& return_value);

    Invoke_Method_Status SaveProperties(
        const SF_PortController* self,
        Property<uint32>& return_value);

    Invoke_Method_Status RestoreProperties(
        const SF_PortController* self,
        Property<uint32>& return_value);

    /*@END@*/
};

CIMPLE_NAMESPACE_END

#endif /* _SF_PortController_Provider_h */
