// Generated by genprov 2.0.24
#include "SF_ConcreteJob_Provider.h"
#include "sf_provider.h"

CIMPLE_NAMESPACE_BEGIN

SF_ConcreteJob_Provider::SF_ConcreteJob_Provider()
{
}

SF_ConcreteJob_Provider::~SF_ConcreteJob_Provider()
{
}

Load_Status SF_ConcreteJob_Provider::load()
{
    return LOAD_OK;
}

Unload_Status SF_ConcreteJob_Provider::unload()
{
    return UNLOAD_OK;
}

Get_Instance_Status SF_ConcreteJob_Provider::get_instance(
    const SF_ConcreteJob* model,
    SF_ConcreteJob*& instance)
{
    return GET_INSTANCE_UNSUPPORTED;
}

Enum_Instances_Status SF_ConcreteJob_Provider::enum_instances(
    const SF_ConcreteJob* model,
    Enum_Instances_Handler<SF_ConcreteJob>* handler)
{
    solarflare::EnumInstances<SF_ConcreteJob>::allSoftware(handler);
    solarflare::EnumInstances<SF_ConcreteJob>::allDiagnostics(handler);
    
    return ENUM_INSTANCES_OK;
}

Create_Instance_Status SF_ConcreteJob_Provider::create_instance(
    SF_ConcreteJob* instance)
{
    return CREATE_INSTANCE_UNSUPPORTED;
}

Delete_Instance_Status SF_ConcreteJob_Provider::delete_instance(
    const SF_ConcreteJob* instance)
{
    return DELETE_INSTANCE_UNSUPPORTED;
}

Modify_Instance_Status SF_ConcreteJob_Provider::modify_instance(
    const SF_ConcreteJob* model,
    const SF_ConcreteJob* instance)
{
    return MODIFY_INSTANCE_UNSUPPORTED;
}

Invoke_Method_Status SF_ConcreteJob_Provider::KillJob(
    const SF_ConcreteJob* self,
    const Property<boolean>& DeleteOnKill,
    Property<uint32>& return_value)
{
    return INVOKE_METHOD_UNSUPPORTED;
}

Invoke_Method_Status SF_ConcreteJob_Provider::RequestStateChange(
    const SF_ConcreteJob* self,
    const Property<uint16>& RequestedState,
    const Property<Datetime>& TimeoutPeriod,
    Property<uint32>& return_value)
{
    // CIMPLE is unable to generate enums for method parameters
    enum ReturnValue 
    {
        OK = 0,
        Error = 2,
        InvalidParameter = 5,
        BadTimeout = 4098,
    };
    enum RequestedStateValue
    {
        Terminate = 4,
        Kill = 5,
    };
    solarflare::Thread *thr = solarflare::Lookup::findThread(*self);
    if (thr == NULL || RequestedState.null)
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
            case Terminate:
            case Kill:
                thr->stop();
                break;
            default:
                return_value.set(InvalidParameter);
        }
    }

    return INVOKE_METHOD_OK;
}

Invoke_Method_Status SF_ConcreteJob_Provider::GetError(
    const SF_ConcreteJob* self,
    CIM_Error*& Error,
    Property<uint32>& return_value)
{
    // CIMPLE is unable to generate enums for method parameters
    enum ReturnValue 
    {
        OK = 0,
        Failed = 4,
    };
    solarflare::Thread *thr = solarflare::Lookup::findThread(*self);
    if (thr == NULL)
    {
        return_value.set(Failed);
    }
    else
    {
        return_value.set(OK);
        switch (thr->currentState())
        {
            case solarflare::Thread::Failed:
            case solarflare::Thread::Aborting:
            case solarflare::Thread::Aborted:
                Error = CIM_Error::create(true);
                Error->ErrorType.null = false;
                Error->ErrorType.value = CIM_Error::_ErrorType::enum_Unknown;
                break;
            default:
                /* nothing */
                break;
        }
    }
    
    return INVOKE_METHOD_OK;
}

#if CIM_SCHEMA_VERSION_MINOR == 26
Invoke_Method_Status SF_ConcreteJob_Provider::GetErrors(
    const SF_ConcreteJob* self,
    Property< Array<CIM_Error*> >& Errors,
    Property<uint32>& return_value)

{
    return INVOKE_METHOD_UNSUPPORTED;
}
#endif

/*@END@*/

CIMPLE_NAMESPACE_END