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
    solarflare::EnumInstances<SF_ConcreteJob>::allObjects(handler);
    
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

void SF_ConcreteJob_Provider::StopThread::handler(solarflare::SystemElement& se, unsigned)
{
    solarflare::Thread *thr = se.embeddedThread();
    if (thr != NULL)
        thr->stop();
}

void SF_ConcreteJob_Provider::GetThreadError::handler(solarflare::SystemElement& se, unsigned)
{
    solarflare::Thread *thr = se.embeddedThread();
    if (thr != NULL)
    {
        switch (thr->currentState())
        {
            case solarflare::Thread::Failed:
            case solarflare::Thread::Aborting:
            case solarflare::Thread::Aborted:
                errdest = CIM_Error::create(true);
                errdest->ErrorType.null = false;
                errdest->ErrorType.value = CIM_Error::_ErrorType::enum_Unknown;
                break;
            default:
                /* nothing */
                break;
        }
    }
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
    if (RequestedState.null)
    {
        return_value.set(InvalidParameter);
    }
    else if (!TimeoutPeriod.null && !(TimeoutPeriod.value == Datetime()))
    {
        return_value.set(BadTimeout);
    }
    else
    {
        switch (RequestedState.value)
        {
            case Terminate:
            case Kill:
            {
                StopThread stopper(self);
                if (stopper.forThread())
                {
                    return_value.set(OK);
                    break;
                }
                // fallthrough
            }
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
    GetThreadError geterror(Error, self);
        
    if (geterror.forThread())
    {
        return_value.set(OK);
    }
    else
    {
        return_value.set(Failed);
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
