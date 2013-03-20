// Generated by genprov 2.0.24
#include "SF_ComputerSystem_Provider.h"
#include "sf_platform.h"

CIMPLE_NAMESPACE_BEGIN

SF_ComputerSystem_Provider::SF_ComputerSystem_Provider()
{
}

SF_ComputerSystem_Provider::~SF_ComputerSystem_Provider()
{
}

Load_Status SF_ComputerSystem_Provider::load()
{
    solarflare::System::target.initialize();
    return LOAD_OK;
}

Unload_Status SF_ComputerSystem_Provider::unload()
{
    return UNLOAD_OK;
}

Get_Instance_Status SF_ComputerSystem_Provider::get_instance(
    const SF_ComputerSystem* model,
    SF_ComputerSystem*& instance)
{
    return GET_INSTANCE_UNSUPPORTED;
}

Enum_Instances_Status SF_ComputerSystem_Provider::enum_instances(
    const SF_ComputerSystem* model,
    Enum_Instances_Handler<SF_ComputerSystem>* handler)
{
    return ENUM_INSTANCES_OK;
}

Create_Instance_Status SF_ComputerSystem_Provider::create_instance(
    SF_ComputerSystem* instance)
{
    return CREATE_INSTANCE_UNSUPPORTED;
}

Delete_Instance_Status SF_ComputerSystem_Provider::delete_instance(
    const SF_ComputerSystem* instance)
{
    return DELETE_INSTANCE_UNSUPPORTED;
}

Modify_Instance_Status SF_ComputerSystem_Provider::modify_instance(
    const SF_ComputerSystem* model,
    const SF_ComputerSystem* instance)
{
    return MODIFY_INSTANCE_UNSUPPORTED;
}

Invoke_Method_Status SF_ComputerSystem_Provider::RequestStateChange(
    const SF_ComputerSystem* self,
    const Property<uint16>& RequestedState,
    CIM_ConcreteJob*& Job,
    const Property<Datetime>& TimeoutPeriod,
    Property<uint32>& return_value)
{
    return INVOKE_METHOD_UNSUPPORTED;
}

Invoke_Method_Status SF_ComputerSystem_Provider::SetPowerState(
    const SF_ComputerSystem* self,
    const Property<uint32>& PowerState,
    const Property<Datetime>& Time,
    Property<uint32>& return_value)
{
    return INVOKE_METHOD_UNSUPPORTED;
}

/*@END@*/

CIMPLE_NAMESPACE_END