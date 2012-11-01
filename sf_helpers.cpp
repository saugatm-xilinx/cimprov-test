#include "sf_provider.h"
#include "CIM_ComputerSystem.h"
#include "SF_ConcreteJob.h"

namespace solarflare 
{
    using cimple::Instance;
    using cimple::Meta_Class;
    using cimple::CIM_ComputerSystem;
    using cimple::Ref;
    using cimple::cast;
    using cimple::SF_ConcreteJob;

    cimple::Instance *SystemElement::cimInstance(const cimple::Meta_Class& cls) const
    {
        const CIMHelper *helper = cimDispatch(cls);
        return helper ? helper->instance(*this) : NULL;
    }

    cimple::Instance *SystemElement::cimReference(const cimple::Meta_Class& cls) const
    {
        const CIMHelper *helper = cimDispatch(cls);
        return helper ? helper->reference(*this) : NULL;
    }
    
    bool SystemElement::cimIsMe(const cimple::Instance& obj) const
    {
        const CIMHelper *helper = cimDispatch(*obj.meta_class);
        return helper ? helper->match(*this, obj) : false;
    }

    const char CIMHelper::solarflareNS[] = "root/solarflare";
    const char CIMHelper::ibmseNS[] = "root/ibmse";
    const char CIMHelper::interopNS[] = "root/pg_interop";
    const char CIMHelper::baseNS[] = "root/cimv2";
    
    String CIMHelper::instanceID(const String& name)
    {
        String result = System::target.prefix();
        result.append(":");
        result.append(name);
        return result;
    }

    Ref<CIM_ComputerSystem> CIMHelper::cimSystem;

    const CIM_ComputerSystem *CIMHelper::findSystem()
    {
        static const char * const namespaces[] = 
        {ibmseNS, solarflareNS, baseNS, NULL};
        
        if (cimSystem)
            return cast<CIM_ComputerSystem *>(cimSystem.ptr());
        
        Ref<CIM_ComputerSystem> system = CIM_ComputerSystem::create();
        Ref<Instance> sysInstance;
        
        for (const char * const *ns = namespaces; *ns != NULL; ns++)
        {
            cimple::Instance_Enumerator ie;
            
            if (cimple::cimom::enum_instances(*ns, system.ptr(), ie) != 0)
                continue;
            
            sysInstance = ie();
            if (sysInstance)
            {
                break;
            }
        }
        if (sysInstance)
        {
            cimSystem.reset(cast<CIM_ComputerSystem *>(sysInstance.ptr()));
        }
        return cimSystem.ptr();
    }

    bool CIMHelper::isOurSystem(const String& sysclass, const String& sysname)
    {
        const CIM_ComputerSystem* ourSys = findSystem();
        
        return (ourSys->CreationClassName.value == sysclass &&
                ourSys->Name.value == sysname);
    }

    Thread *Lookup::findThread(const Instance& inst)
    {
        Lookup finder(&inst);
        System::target.forAllDiagnostics(finder);
        if (!finder.found())
            System::target.forAllSoftware(finder);
        return (finder.found() ?
                finder.found()->embeddedThread() :
                NULL);
    }

    SystemElement *Lookup::findAny(const Instance& inst)
    {
        Lookup finder(&inst);
        if (!System::target.forAllSoftware(finder))
            return finder.found();
        if (!System::target.forAllNICs(finder))
        return finder.found();
        if (!System::target.forAllInterfaces(finder))
            return finder.found();
        if (!System::target.forAllPorts(finder))
            return finder.found();
        if (!System::target.forAllDiagnostics(finder))
            return finder.found();
        return NULL;
    }
    
    Instance *ConcreteJobAbstractHelper::reference(const SystemElement& obj) const
    {
        SF_ConcreteJob *job = SF_ConcreteJob::create(true);
        job->InstanceID.set(instanceID(obj.name()));
        job->InstanceID.value.append(":");
        job->InstanceID.value.append(threadSuffix());
        return job;
    }

    Instance *ConcreteJobAbstractHelper::instance(const SystemElement& obj) const
    {
        Thread *th = const_cast<SystemElement&>(obj).embeddedThread();
        SF_ConcreteJob *job = static_cast<SF_ConcreteJob *>(reference(obj));
    
        job->OperationalStatus.null = false;
        job->JobState.null = false;
        switch (th->currentState())
        {
            case Thread::NotRun:
                job->OperationalStatus.value.append(SF_ConcreteJob::_OperationalStatus::enum_Dormant);
                job->JobState.value = SF_ConcreteJob::_JobState::enum_New;
                break;
            case Thread::Running:
                job->OperationalStatus.value.append(SF_ConcreteJob::_OperationalStatus::enum_OK);
                job->JobState.value = SF_ConcreteJob::_JobState::enum_Running;
                break;
            case Thread::Succeeded:
                job->OperationalStatus.value.append(SF_ConcreteJob::_OperationalStatus::enum_OK);
                job->OperationalStatus.value.append(SF_ConcreteJob::_OperationalStatus::enum_Completed);
                job->JobState.value = SF_ConcreteJob::_JobState::enum_Completed;
                break;
            case Thread::Failed:
                job->OperationalStatus.value.append(SF_ConcreteJob::_OperationalStatus::enum_Error);
                job->OperationalStatus.value.append(SF_ConcreteJob::_OperationalStatus::enum_Completed);
                job->JobState.value = SF_ConcreteJob::_JobState::enum_Exception;
                break;
            case Thread::Aborting:
                job->OperationalStatus.value.append(SF_ConcreteJob::_OperationalStatus::enum_Error);
                job->JobState.value = SF_ConcreteJob::_JobState::enum_Shutting_Down;
                break;
            case Thread::Aborted:
                job->OperationalStatus.value.append(SF_ConcreteJob::_OperationalStatus::enum_Error);
                job->JobState.value = SF_ConcreteJob::_JobState::enum_Killed;
                break;
        }
        job->PercentComplete.set(th->percentage());
        job->DeleteOnCompletion.set(false);
        return job;
    }
    
    bool
    ConcreteJobAbstractHelper::match(const SystemElement& se, const Instance& inst) const
    {
        const cimple::CIM_ConcreteJob *job = cast<const cimple::CIM_ConcreteJob *>(&inst);
        if (job == NULL)
            return false;

        if (job->InstanceID.null)
            return false;

        String id = instanceID(se.name());
        id.append(":");
        id.append(threadSuffix());
        
        return id == job->InstanceID.value;
    }
    
} // namespace
