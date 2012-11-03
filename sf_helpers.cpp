#include "sf_provider.h"
#include "CIM_ComputerSystem.h"
#include "SF_ConcreteJob.h"
#include "SF_LogEntry.h"
#include "SF_RecordLog.h"

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

    cimple::Instance *SystemElement::cimAssociation(const cimple::Meta_Class& from,
                                                    const cimple::Meta_Class& assoc,
                                                    const cimple::Meta_Class& to) const
    {
        const CIMHelper *helper = cimDispatch(from, assoc, to);
        return helper ? helper->instance(*this) : NULL;
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

    using cimple::SF_LogEntry;
    
    class LogEntryHelper : public CIMLogEntryHelper {
    public:
        virtual cimple::Instance *instance(const SystemElement&, const Logger&, const LogEntry&) const;
        virtual cimple::Instance *reference(const SystemElement&, const Logger&, const LogEntry& obj) const;
    };
    
    cimple::Instance *LogEntryHelper::reference(const SystemElement&, const Logger& log, const LogEntry& entry) const
    {
        Buffer buf;
        SF_LogEntry *l = SF_LogEntry::create(true);
        
        buf.appends(solarflare::System::target.prefix().c_str());
        buf.append(':');
        buf.appends(log.description());
        buf.append('#');
        buf.append_uint64(entry.id());
        l->InstanceID.set(buf.data());
        return l;
    }

    cimple::Instance *LogEntryHelper::instance(const SystemElement& scope, const Logger& log, const LogEntry& entry) const
    {
        static unsigned const severityMap[] = {
            SF_LogEntry::_PerceivedSeverity::enum_Fatal_NonRecoverable,
            SF_LogEntry::_PerceivedSeverity::enum_Major,
            SF_LogEntry::_PerceivedSeverity::enum_Minor,
            SF_LogEntry::_PerceivedSeverity::enum_Information,
            SF_LogEntry::_PerceivedSeverity::enum_Information,
        };
        
        char id[17];
        SF_LogEntry *le = static_cast<SF_LogEntry *>(reference(scope, log, entry));
        le->LogInstanceID.set(CIMHelper::instanceID(log.description()));
        le->LogName.set(log.description());
        sprintf(id, "%16.16llx", entry.id());
        le->ElementName.set(id);
        le->RecordID.set(id);
        le->RecordFormat.set("");
        le->RecordData.set(entry.message());
        le->CreationTimeStamp.set(entry.stamp());
        le->PerceivedSeverity.null = false;
        le->PerceivedSeverity.value = severityMap[entry.severity()];

        return le;
    }

    const CIMLogEntryHelper *CIMLogEntryHelper::dispatch(const Meta_Class& cls)
    {
        static const LogEntryHelper logEntryHelper;
        
        if (&cls == &SF_LogEntry::static_meta_class)
            return &logEntryHelper;
        return NULL;
    }

    using cimple::SF_RecordLog;

    class RecordLogHelper : public CIMLoggerHelper {
    public:
        virtual Instance *instance(const Logger& log) const;
        virtual Instance *reference(const Logger& log) const;
        virtual bool match(const Logger& log, const Instance& inst) const;
    };


    Instance *RecordLogHelper::reference(const Logger& log) const
    {
        SF_RecordLog *newLog = SF_RecordLog::create(true);
        
        newLog->InstanceID.set(CIMHelper::instanceID(log.description()));
        return newLog;
    }

    Instance *RecordLogHelper::instance(const solarflare::Logger& log) const
    {
        SF_RecordLog *newLog = static_cast<SF_RecordLog *>(reference(log));

        newLog->Name.set(log.description());
        newLog->ElementName.set(log.description());
        newLog->Description.set(log.description());
        newLog->OperationalStatus.null = false;
        newLog->OperationalStatus.value.append(SF_RecordLog::_OperationalStatus::enum_OK);
        newLog->HealthState.null = false;
        newLog->HealthState.value = SF_RecordLog::_HealthState::enum_OK;
        newLog->EnabledState.null = false;
        newLog->EnabledState.value = (log.isEnabled() ? 
                                      SF_RecordLog::_EnabledState::enum_Enabled :
                                      SF_RecordLog::_EnabledState::enum_Disabled);
        newLog->RequestedState.null = false;
        newLog->RequestedState.value = SF_RecordLog::_RequestedState::enum_No_Change;
        newLog->LogState.null = false;
        newLog->LogState.value = (log.isEnabled() ?
                                  SF_RecordLog::_LogState::enum_Normal :
                                  SF_RecordLog::_LogState::enum_Not_Applicable);
        newLog->OverwritePolicy.null = false;
        newLog->OverwritePolicy.value = SF_RecordLog::_OverwritePolicy::enum_Wraps_When_Full;
        newLog->MaxNumberOfRecords.set(log.logSize());
        newLog->CurrentNumberOfRecords.set(log.currentSize());
        
        return newLog;
    }

    bool RecordLogHelper::match(const Logger& log, const Instance& instance) const
    {
        const cimple::CIM_RecordLog *logObj = cast<const cimple::CIM_RecordLog *>(&instance);
        if (logObj == NULL)
            return false;

        if (logObj->InstanceID.null)
            return false;
        
        return CIMHelper::instanceID(log.description()) == logObj->InstanceID.value;
    }

    const CIMLoggerHelper *CIMLoggerHelper::dispatch(const Meta_Class& cls)
    {
        static const RecordLogHelper recordLogHelper;
        
        if (&cls == &SF_RecordLog::static_meta_class)
            return &recordLogHelper;
        return NULL;
    }

    
} // namespace
