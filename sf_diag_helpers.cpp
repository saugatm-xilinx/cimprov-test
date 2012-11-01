#include "sf_provider.h"
#include "SF_DiagnosticTest.h"
#include "SF_ConcreteJob.h"

namespace solarflare 
{
    using cimple::Instance;
    using cimple::Meta_Class;
    using cimple::CIM_ComputerSystem;
    using cimple::Ref;
    using cimple::cast;
    using cimple::SF_DiagnosticTest;
    using cimple::SF_ConcreteJob;

    class DiagnosticTestHelper : public CIMHelper {
    public:
        virtual Instance *reference(const SystemElement& obj) const;
        virtual Instance *instance(const SystemElement&) const;
        virtual bool match(const SystemElement& obj, const Instance& inst) const;
    };

    class DiagnosticJobHelper : public ConcreteJobAbstractHelper {
        virtual const char *threadSuffix() const { return "diagThread"; }
        virtual Thread *threadOf(SystemElement& se) const
        {
            return static_cast<Diagnostic&>(se).asyncThread();
        }
    public:
    };


    const CIMHelper* Diagnostic::cimDispatch(const Meta_Class& cls) const
    {
        static const DiagnosticTestHelper diagnosticTest;
        static const DiagnosticJobHelper diagnosticJob;
        if (&cls == &SF_DiagnosticTest::static_meta_class)
            return &diagnosticTest;
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


} // namespace
