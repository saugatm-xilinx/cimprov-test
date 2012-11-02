// Generated by genprov 2.0.24
#include "SF_EnabledLogicalElementCapabilities_Provider.h"

CIMPLE_NAMESPACE_BEGIN

SF_EnabledLogicalElementCapabilities *
SF_EnabledLogicalElementCapabilities_Provider::makeReference(const solarflare::SystemElement& el, 
                                                             const char *suffix)
{
    SF_EnabledLogicalElementCapabilities* caps = SF_EnabledLogicalElementCapabilities::create(true);
    
    caps->InstanceID.set(el.name());
    caps->InstanceID.value.append(" ");
    caps->InstanceID.value.append(suffix);
    caps->InstanceID.value.append(" Capabilities");
    return caps;
}

SF_EnabledLogicalElementCapabilities *
SF_EnabledLogicalElementCapabilities_Provider::makeInstance(const solarflare::SystemElement& el, 
                                                            const char *suffix,
                                                            bool manageable)
{
    SF_EnabledLogicalElementCapabilities* caps = makeReference(el, suffix);
    
    caps->ElementNameEditSupported.set(false);
    caps->RequestedStatesSupported.null = false;
    if (manageable)
    {
        static const uint16 states[] = {
            SF_EnabledLogicalElementCapabilities::_RequestedStatesSupported::enum_Disabled,
            SF_EnabledLogicalElementCapabilities::_RequestedStatesSupported::enum_Enabled,
            SF_EnabledLogicalElementCapabilities::_RequestedStatesSupported::enum_Reset,
        };
        caps->RequestedStatesSupported.value.append(states, sizeof(states) / sizeof(*states));
    }
    return caps;
}

bool SF_EnabledLogicalElementCapabilities_Provider::IntfEnum::process(const solarflare::SystemElement& el)
{
    const solarflare::Interface& intf = static_cast<const solarflare::Interface&>(el);
    
    handler->handle(makeInstance(intf, "Port", true));
    handler->handle(makeInstance(intf, "Endpoint", false));
    return true;
}

bool SF_EnabledLogicalElementCapabilities_Provider::NICEnum::process(const solarflare::SystemElement &el)
{
    const solarflare::NIC& nic = static_cast<const solarflare::NIC&>(el);

    handler->handle(makeInstance(nic, "Controller", false));
    return true;
}

SF_EnabledLogicalElementCapabilities_Provider::SF_EnabledLogicalElementCapabilities_Provider()
{
}

SF_EnabledLogicalElementCapabilities_Provider::~SF_EnabledLogicalElementCapabilities_Provider()
{
}

Load_Status SF_EnabledLogicalElementCapabilities_Provider::load()
{
    return LOAD_OK;
}

Unload_Status SF_EnabledLogicalElementCapabilities_Provider::unload()
{
    return UNLOAD_OK;
}

Get_Instance_Status SF_EnabledLogicalElementCapabilities_Provider::get_instance(
    const SF_EnabledLogicalElementCapabilities* model,
    SF_EnabledLogicalElementCapabilities*& instance)
{
    return GET_INSTANCE_UNSUPPORTED;
}

Enum_Instances_Status SF_EnabledLogicalElementCapabilities_Provider::enum_instances(
    const SF_EnabledLogicalElementCapabilities* model,
    Enum_Instances_Handler<SF_EnabledLogicalElementCapabilities>* handler)
{
    NICEnum nicelements(handler);
    IntfEnum intfelements(handler);

    solarflare::System::target.forAllInterfaces(intfelements);
    solarflare::System::target.forAllNICs(nicelements);
    return ENUM_INSTANCES_OK;
}

Create_Instance_Status SF_EnabledLogicalElementCapabilities_Provider::create_instance(
    SF_EnabledLogicalElementCapabilities* instance)
{
    return CREATE_INSTANCE_UNSUPPORTED;
}

Delete_Instance_Status SF_EnabledLogicalElementCapabilities_Provider::delete_instance(
    const SF_EnabledLogicalElementCapabilities* instance)
{
    return DELETE_INSTANCE_UNSUPPORTED;
}

Modify_Instance_Status SF_EnabledLogicalElementCapabilities_Provider::modify_instance(
    const SF_EnabledLogicalElementCapabilities* model,
    const SF_EnabledLogicalElementCapabilities* instance)
{
    return MODIFY_INSTANCE_UNSUPPORTED;
}

/*@END@*/

CIMPLE_NAMESPACE_END