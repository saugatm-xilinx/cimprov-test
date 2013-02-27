// Generated by genprov 2.0.24
#include "SF_DiagnosticTest_Provider.h"
#include "SF_ComputerSystem_Provider.h"
#include "SF_ConcreteJob_Provider.h"
#include "SF_NICCard_Provider.h"
#include "sf_provider.h"

CIMPLE_NAMESPACE_BEGIN

SF_DiagnosticTest_Provider::SF_DiagnosticTest_Provider()
{
}

SF_DiagnosticTest_Provider::~SF_DiagnosticTest_Provider()
{
}

Load_Status SF_DiagnosticTest_Provider::load()
{
    solarflare::CIMHelper::initialize();
    return LOAD_OK;
}

Unload_Status SF_DiagnosticTest_Provider::unload()
{
    return UNLOAD_OK;
}

Get_Instance_Status SF_DiagnosticTest_Provider::get_instance(
    const SF_DiagnosticTest* model,
    SF_DiagnosticTest*& instance)
{
    return GET_INSTANCE_UNSUPPORTED;
}

Enum_Instances_Status SF_DiagnosticTest_Provider::enum_instances(
    const SF_DiagnosticTest* model,
    Enum_Instances_Handler<SF_DiagnosticTest>* handler)
{
    solarflare::EnumInstances<SF_DiagnosticTest>::allDiagnostics(handler);
    return ENUM_INSTANCES_OK;
}

Create_Instance_Status SF_DiagnosticTest_Provider::create_instance(
    SF_DiagnosticTest* instance)
{
    return CREATE_INSTANCE_UNSUPPORTED;
}

Delete_Instance_Status SF_DiagnosticTest_Provider::delete_instance(
    const SF_DiagnosticTest* instance)
{
    return DELETE_INSTANCE_UNSUPPORTED;
}

Modify_Instance_Status SF_DiagnosticTest_Provider::modify_instance(
    const SF_DiagnosticTest* model,
    const SF_DiagnosticTest* instance)
{
    return MODIFY_INSTANCE_UNSUPPORTED;
}

Invoke_Method_Status SF_DiagnosticTest_Provider::RequestStateChange(
    const SF_DiagnosticTest* self,
    const Property<uint16>& RequestedState,
    CIM_ConcreteJob*& Job,
    const Property<Datetime>& TimeoutPeriod,
    Property<uint32>& return_value)
{
    return INVOKE_METHOD_UNSUPPORTED;
}

Invoke_Method_Status SF_DiagnosticTest_Provider::StartService(
    const SF_DiagnosticTest* self,
    Property<uint32>& return_value)
{
    return INVOKE_METHOD_UNSUPPORTED;
}

Invoke_Method_Status SF_DiagnosticTest_Provider::StopService(
    const SF_DiagnosticTest* self,
    Property<uint32>& return_value)
{
    return INVOKE_METHOD_UNSUPPORTED;
}

Invoke_Method_Status SF_DiagnosticTest_Provider::RunDiagnostic(
    const SF_DiagnosticTest* self,
    const CIM_ManagedElement* ManagedElement,
    const CIM_DiagnosticSetting* DiagSetting,
    const CIM_JobSettingData* JobSetting,
    CIM_ConcreteJob*& Job,
    Property<uint32>& return_value)
{
    return INVOKE_METHOD_UNSUPPORTED;
}

Invoke_Method_Status SF_DiagnosticTest_Provider::RunDiagnosticService(
    const SF_DiagnosticTest* self,
    const CIM_ManagedElement* ManagedElement,
    const CIM_DiagnosticSettingData* DiagnosticSettings,
    const CIM_JobSettingData* JobSettings,
    CIM_ConcreteJob*& Job,
    Property<uint32>& return_value)
{
    solarflare::Diagnostic *diag = solarflare::Lookup::findDiagnostic(*self);
    enum Result 
    {
        OK = 0,
        Error = 2,
        TimedOut = 3,
        Failed = 4,
        InvalidParameter = 5
    };
    
    if (diag == NULL || ManagedElement == NULL)
    {
        return_value.set(InvalidParameter);
    }
    else
    {
        const CIM_Card *card = cast<CIM_Card *>(ManagedElement);
        
        if (card == NULL || solarflare::Lookup::findNIC(*card) != diag->nic())
        {
            return_value.set(InvalidParameter);
        }
        else
        {
            diag->run(false);
            Job = cast<CIM_ConcreteJob *>(diag->cimReference(SF_ConcreteJob::static_meta_class));
            return_value.set(OK);
        }
    }

    return INVOKE_METHOD_OK;
}

Invoke_Method_Status SF_DiagnosticTest_Provider::RunTest(
    const SF_DiagnosticTest* self,
    const CIM_ManagedSystemElement* SystemElement,
    const CIM_DiagnosticSetting* Setting,
    CIM_DiagnosticResult*& Result,
    Property<uint32>& return_value)
{
    return INVOKE_METHOD_UNSUPPORTED;
}

Invoke_Method_Status SF_DiagnosticTest_Provider::ClearResults(
    const SF_DiagnosticTest* self,
    const CIM_ManagedSystemElement* SystemElement,
    Property<Array_String>& ResultsNotCleared,
    Property<uint32>& return_value)
{
    return INVOKE_METHOD_UNSUPPORTED;
}

Invoke_Method_Status SF_DiagnosticTest_Provider::DiscontinueTest(
    const SF_DiagnosticTest* self,
    const CIM_ManagedSystemElement* SystemElement,
    const CIM_DiagnosticResult* Result,
    Property<boolean>& TestingStopped,
    Property<uint32>& return_value)
{
    return INVOKE_METHOD_UNSUPPORTED;
}

Invoke_Method_Status SF_DiagnosticTest_Provider::ChangeAffectedElementsAssignedSequence(
    const SF_DiagnosticTest* self,
    const Property< Array<CIM_ManagedElement*> >& ManagedElements,
    const Property<Array_uint16>& AssignedSequence,
    CIM_ConcreteJob*& Job,
    Property<uint32>& return_value)

{
    return INVOKE_METHOD_UNSUPPORTED;
}

/*@END@*/

CIMPLE_NAMESPACE_END
