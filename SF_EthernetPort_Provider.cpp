// Generated by genprov 2.0.24
#include "SF_EthernetPort_Provider.h"
#include "SF_ComputerSystem_Provider.h"
#include "sf_provider.h"

CIMPLE_NAMESPACE_BEGIN

SF_EthernetPort_Provider::SF_EthernetPort_Provider()
{
}

SF_EthernetPort_Provider::~SF_EthernetPort_Provider()
{
}

Load_Status SF_EthernetPort_Provider::load()
{
    solarflare::System::target.initialize();
    return LOAD_OK;
}

Unload_Status SF_EthernetPort_Provider::unload()
{
    return UNLOAD_OK;
}

Get_Instance_Status SF_EthernetPort_Provider::get_instance(
    const SF_EthernetPort* model,
    SF_EthernetPort*& instance)
{
    return GET_INSTANCE_UNSUPPORTED;
}

Enum_Instances_Status SF_EthernetPort_Provider::enum_instances(
    const SF_EthernetPort* model,
    Enum_Instances_Handler<SF_EthernetPort>* handler)
{
    solarflare::EnumInstances<SF_EthernetPort>::allInterfaces(handler);
    return ENUM_INSTANCES_OK;
}

Create_Instance_Status SF_EthernetPort_Provider::create_instance(
    SF_EthernetPort* instance)
{
    return CREATE_INSTANCE_UNSUPPORTED;
}

Delete_Instance_Status SF_EthernetPort_Provider::delete_instance(
    const SF_EthernetPort* instance)
{
    return DELETE_INSTANCE_UNSUPPORTED;
}

Modify_Instance_Status SF_EthernetPort_Provider::modify_instance(
    const SF_EthernetPort* model,
    const SF_EthernetPort* instance)
{
    solarflare::Interface *intf = solarflare::Lookup::findInterface(*model);

    if (intf == NULL)
        return MODIFY_INSTANCE_NOT_FOUND;
    
    if (!instance->RequestedSpeed.null)
    {
        solarflare::Port::Speed sp = 
        solarflare::Port::speedValue(instance->RequestedSpeed.value);
        
        if (sp == solarflare::Port::SpeedUnknown ||
            sp > intf->nic()->maxLinkSpeed())
            return MODIFY_INSTANCE_INVALID_PARAMETER;

        intf->port()->linkSpeed(sp);
    }

    return MODIFY_INSTANCE_OK;
}

Invoke_Method_Status SF_EthernetPort_Provider::RequestStateChange(
    const SF_EthernetPort* self,
    const Property<uint16>& RequestedState,
    CIM_ConcreteJob*& Job,
    const Property<Datetime>& TimeoutPeriod,
    Property<uint32>& return_value)
{
    /// CIMPLE is unable to generate enums for method parameters
    enum ReturnValue 
    {
        OK = 0,
        Error = 2,
        InvalidParameter = 5,
        BadTimeout = 4098,
    };
    solarflare::Interface *intf = solarflare::Lookup::findInterface(*self);
    if (intf == NULL || RequestedState.null)
    {
        return_value.set(InvalidParameter);
    }
    else if (!TimeoutPeriod.null && !(TimeoutPeriod.value == Datetime()))
    {
        return_value.set(BadTimeout);
    }
    else
    {
        return_value.set(OK);
        switch (RequestedState.value)
        {
            case SF_EthernetPort::_RequestedState::enum_Enabled:
                intf->enable(true);
                break;
            case SF_EthernetPort::_RequestedState::enum_Disabled:
                intf->enable(false);
                break;
            case SF_EthernetPort::_RequestedState::enum_Reset:
                if (intf->ifStatus())
                {
                    intf->enable(false);
                    intf->enable(true);
                }
                break;
            default:
                return_value.set(InvalidParameter);
        }
    }

    return INVOKE_METHOD_OK;
}

Invoke_Method_Status SF_EthernetPort_Provider::SetPowerState(
    const SF_EthernetPort* self,
    const Property<uint16>& PowerState,
    const Property<Datetime>& Time,
    Property<uint32>& return_value)
{
    return INVOKE_METHOD_UNSUPPORTED;
}

Invoke_Method_Status SF_EthernetPort_Provider::Reset(
    const SF_EthernetPort* self,
    Property<uint32>& return_value)
{
    return INVOKE_METHOD_UNSUPPORTED;
}

Invoke_Method_Status SF_EthernetPort_Provider::EnableDevice(
    const SF_EthernetPort* self,
    const Property<boolean>& Enabled,
    Property<uint32>& return_value)
{
    return INVOKE_METHOD_UNSUPPORTED;
}

Invoke_Method_Status SF_EthernetPort_Provider::OnlineDevice(
    const SF_EthernetPort* self,
    const Property<boolean>& Online,
    Property<uint32>& return_value)
{
    return INVOKE_METHOD_UNSUPPORTED;
}

Invoke_Method_Status SF_EthernetPort_Provider::QuiesceDevice(
    const SF_EthernetPort* self,
    const Property<boolean>& Quiesce,
    Property<uint32>& return_value)
{
    return INVOKE_METHOD_UNSUPPORTED;
}

Invoke_Method_Status SF_EthernetPort_Provider::SaveProperties(
    const SF_EthernetPort* self,
    Property<uint32>& return_value)
{
    return INVOKE_METHOD_UNSUPPORTED;
}

Invoke_Method_Status SF_EthernetPort_Provider::RestoreProperties(
    const SF_EthernetPort* self,
    Property<uint32>& return_value)
{
    return INVOKE_METHOD_UNSUPPORTED;
}

/*@END@*/

CIMPLE_NAMESPACE_END
