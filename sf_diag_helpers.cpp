#include "sf_provider.h"
#include "SF_DiagnosticTest.h"
#include "SF_ConcreteJob.h"
#include "SF_DiagnosticLog.h"

namespace solarflare 
{
    using cimple::Instance;
    using cimple::Meta_Class;
    using cimple::CIM_ComputerSystem;
    using cimple::Ref;
    using cimple::cast;
    using cimple::SF_DiagnosticTest;
    using cimple::SF_DiagnosticLog;
    using cimple::SF_ConcreteJob;

    class DiagnosticTestHelper : public CIMHelper {
    public:
        virtual Instance *reference(const SystemElement& obj) const;
        virtual Instance *instance(const SystemElement&) const;
        virtual bool match(const SystemElement& obj, const Instance& inst) const;
    };

    class DiagnosticLogHelper : public CIMHelper {
    public:
        virtual Instance *reference(const SystemElement& obj) const;
        virtual Instance *instance(const SystemElement&) const;
        virtual bool match(const SystemElement& obj, const Instance& inst) const;
    };

    class DiagnosticJobHelper : public ConcreteJobAbstractHelper {
    protected:
        virtual const char *threadSuffix() const { return "diagThread"; }
    };


    const CIMHelper* Diagnostic::cimDispatch(const Meta_Class& cls) const
    {
        static const DiagnosticTestHelper diagnosticTest;
        static const DiagnosticJobHelper diagnosticJob;
        static const DiagnosticLogHelper diagnosticLog;
        if (&cls == &SF_DiagnosticTest::static_meta_class)
            return &diagnosticTest;
        if (&cls == &SF_DiagnosticLog::static_meta_class)
            return &diagnosticLog;
        if (&cls == &SF_ConcreteJob::static_meta_class && 
            const_cast<Diagnostic *>(this)->asyncThread() != NULL)
            return &diagnosticJob;
        return NULL;
    }

    Instance *DiagnosticTestHelper::reference(const SystemElement& se) const
    {
        const Diagnostic& diag = static_cast<const Diagnostic&>(se);
        const CIM_ComputerSystem *system = findSystem();
        SF_DiagnosticTest *newSvc = SF_DiagnosticTest::create(true);
        
        newSvc->CreationClassName.set("SF_DiagnosticTest");
        newSvc->Name.set(diag.name());
        newSvc->SystemCreationClassName.set(system->CreationClassName.value);
        newSvc->SystemName.set(system->Name.value);
        
        return newSvc;
    }

    Instance *DiagnosticTestHelper::instance(const solarflare::SystemElement& se) const 
    {
        const solarflare::Diagnostic& diag = static_cast<const solarflare::Diagnostic&>(se);
    
        SF_DiagnosticTest *newSvc = static_cast<SF_DiagnosticTest *>(reference(diag));

        newSvc->Description.set(diag.description());
        newSvc->ElementName.set(diag.name());
        unsigned p = diag.percentage();
        newSvc->Started.set(p > 0 && p < 100);
        newSvc->Characteristics.null = false;

        if (diag.isDestructive())
        {
            newSvc->Characteristics.value.append(SF_DiagnosticTest::_Characteristics::enum_Is_Destructive);
            newSvc->Characteristics.value.append(SF_DiagnosticTest::_Characteristics::enum_Is_Risky);
        }
        newSvc->TestTypes.null = false;
        switch (diag.testKind())
        {
            case solarflare::Diagnostic::FunctionalTest:
                newSvc->TestTypes.value.append(SF_DiagnosticTest::_TestTypes::enum_Functional);
                break;
            case solarflare::Diagnostic::StressTest:
                newSvc->TestTypes.value.append(SF_DiagnosticTest::_TestTypes::enum_Stress);
                break;
            case solarflare::Diagnostic::HealthCheckTest:
                newSvc->TestTypes.value.append(SF_DiagnosticTest::_TestTypes::enum_Health_Check);
                break;
            case solarflare::Diagnostic::MediaAccessTest:
                newSvc->TestTypes.value.append(SF_DiagnosticTest::_TestTypes::enum_Access_Test);
                newSvc->Characteristics.value.append(SF_DiagnosticTest::_Characteristics::enum_Media_Required);
                newSvc->Characteristics.value.append(SF_DiagnosticTest::_Characteristics::enum_Additional_Hardware_Required);
                break;
        }
        return newSvc;
    }


    bool DiagnosticTestHelper::match(const SystemElement& se, const Instance& inst) const
    {
        const cimple::CIM_DiagnosticTest *test = cast<const cimple::CIM_DiagnosticTest *>(&inst);
        
        if (test == NULL)
            return false;

        if (test->CreationClassName.null || test->Name.null || 
            test->CreationClassName.value != "SF_DiagnosticTest" ||
            test->SystemCreationClassName.null || test->SystemName.null)
            return false;
        if (!isOurSystem(test->SystemCreationClassName.value, test->SystemName.value))
            return false;
        
        return test->Name.value == se.name();
    }
    

    Instance *DiagnosticLogHelper::reference(const SystemElement& se) const
    {
        SF_DiagnosticLog *newLog = SF_DiagnosticLog::create(true);
        
        newLog->InstanceID.set(instanceID(se.name()));
        newLog->InstanceID.value.append(" ");
        newLog->InstanceID.value.append(static_cast<const Diagnostic&>(se).log().description());

        return newLog;
    }

    Instance *DiagnosticLogHelper::instance(const SystemElement& se) const
    {
        const Diagnostic& diag = static_cast<const Diagnostic&>(se);
        SF_DiagnosticLog *newLog = static_cast<SF_DiagnosticLog *>(reference(diag));
        
        newLog->Name.set(se.name());
        newLog->Name.value.append(" ");
        newLog->Name.value.append(diag.log().description());
        newLog->ElementName.set(diag.log().description());
        newLog->Description.set(diag.log().description());
        newLog->OperationalStatus.null = false;
        newLog->OperationalStatus.value.append(SF_DiagnosticLog::_OperationalStatus::enum_OK);
        newLog->HealthState.null = false;
        newLog->HealthState.value = SF_DiagnosticLog::_HealthState::enum_OK;
        newLog->EnabledState.null = false;
        newLog->EnabledState.value = (diag.log().isEnabled() ? 
                                      SF_DiagnosticLog::_EnabledState::enum_Enabled :
                                      SF_DiagnosticLog::_EnabledState::enum_Disabled);
        newLog->RequestedState.null = false;
        newLog->RequestedState.value = SF_DiagnosticLog::_RequestedState::enum_No_Change;
        newLog->LogState.null = false;
        newLog->LogState.value = (diag.log().isEnabled() ?
                                  SF_DiagnosticLog::_LogState::enum_Normal :
                                  SF_DiagnosticLog::_LogState::enum_Not_Applicable);
        newLog->OverwritePolicy.null = false;
        newLog->OverwritePolicy.value = SF_DiagnosticLog::_OverwritePolicy::enum_Wraps_When_Full;
        newLog->MaxNumberOfRecords.set(diag.log().logSize());
        newLog->CurrentNumberOfRecords.set(diag.log().currentSize());
        
        return newLog;
    }

    bool DiagnosticLogHelper::match(const SystemElement& se, const Instance& inst) const
    {
        const Diagnostic& diag = static_cast<const Diagnostic&>(se);
        const cimple::CIM_DiagnosticLog *log = cast<const cimple::CIM_DiagnosticLog *>(&inst);
        
        if (log == NULL)
            return false;
        if (log->InstanceID.null)
            return false;
        String id = instanceID(diag.name());
        id.append(" ");
        id.append(diag.log().description());
        return id == log->InstanceID.value;
    }
} // namespace
