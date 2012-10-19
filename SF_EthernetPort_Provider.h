// Generated by genprov 2.0.24
#ifndef _SF_EthernetPort_Provider_h
#define _SF_EthernetPort_Provider_h

#include <cimple/cimple.h>
#include "SF_EthernetPort.h"
#include "sf_platform.h"

CIMPLE_NAMESPACE_BEGIN

class SF_EthernetPort_Provider
{
    class InterfaceEnum : public solarflare::ConstInterfaceEnumerator {
        Enum_Instances_Handler<SF_EthernetPort>* handler;
    public:
        InterfaceEnum(Enum_Instances_Handler<SF_EthernetPort>* h) :
            handler(h) {};
        virtual bool process(const solarflare::Interface& inrf);
    };

public:

    class InterfaceFinder : public solarflare::InterfaceEnumerator 
    {
        solarflare::Interface *obj;
        String devId;
    public:
        InterfaceFinder(const String& id) :
            obj(NULL), devId(id) {};
        virtual bool process(solarflare::Interface& inrf);
        solarflare::Interface *found() const { return obj; }
            
    };

    static SF_EthernetPort *makeReference(const solarflare::Interface& intf);
    static solarflare::Interface *findByInstance(const SF_EthernetPort& port);

    typedef SF_EthernetPort Class;

    SF_EthernetPort_Provider();

    ~SF_EthernetPort_Provider();

    Load_Status load();

    Unload_Status unload();

    Get_Instance_Status get_instance(
        const SF_EthernetPort* model,
        SF_EthernetPort*& instance);

    Enum_Instances_Status enum_instances(
        const SF_EthernetPort* model,
        Enum_Instances_Handler<SF_EthernetPort>* handler);

    Create_Instance_Status create_instance(
        SF_EthernetPort* instance);

    Delete_Instance_Status delete_instance(
        const SF_EthernetPort* instance);

    Modify_Instance_Status modify_instance(
        const SF_EthernetPort* model,
        const SF_EthernetPort* instance);

    Invoke_Method_Status RequestStateChange(
        const SF_EthernetPort* self,
        const Property<uint16>& RequestedState,
        CIM_ConcreteJob*& Job,
        const Property<Datetime>& TimeoutPeriod,
        Property<uint32>& return_value);

    Invoke_Method_Status SetPowerState(
        const SF_EthernetPort* self,
        const Property<uint16>& PowerState,
        const Property<Datetime>& Time,
        Property<uint32>& return_value);

    Invoke_Method_Status Reset(
        const SF_EthernetPort* self,
        Property<uint32>& return_value);

    Invoke_Method_Status EnableDevice(
        const SF_EthernetPort* self,
        const Property<boolean>& Enabled,
        Property<uint32>& return_value);

    Invoke_Method_Status OnlineDevice(
        const SF_EthernetPort* self,
        const Property<boolean>& Online,
        Property<uint32>& return_value);

    Invoke_Method_Status QuiesceDevice(
        const SF_EthernetPort* self,
        const Property<boolean>& Quiesce,
        Property<uint32>& return_value);

    Invoke_Method_Status SaveProperties(
        const SF_EthernetPort* self,
        Property<uint32>& return_value);

    Invoke_Method_Status RestoreProperties(
        const SF_EthernetPort* self,
        Property<uint32>& return_value);

    /*@END@*/
};

CIMPLE_NAMESPACE_END

#endif /* _SF_EthernetPort_Provider_h */
