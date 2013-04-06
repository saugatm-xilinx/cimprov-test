#include "sf_provider.h"
#include "SF_SoftwareIdentity.h"
#include "SF_SoftwareInstallationService.h"
#include "SF_BundleComponent.h"
#include "SF_ConcreteJob.h"
#include "CIM_OperatingSystem.h"
#include "SF_LogEntry.h"
#include "SF_RecordLog.h"
#include "SF_RecordLogCapabilities.h"
#include "SF_UseOfLog.h"
#include "SF_LogManagesRecord.h"
#include "SF_RegisteredProfile.h"
#include "SF_ReferencedProfile.h"
#include "SF_ElementCapabilities.h"

namespace solarflare 
{
    using cimple::Instance;
    using cimple::Meta_Class;
    using cimple::CIM_ComputerSystem;
    using cimple::Ref;
    using cimple::cast;
    using cimple::is_a;
    using cimple::SF_LogEntry;
    using cimple::SF_RecordLog;
    using cimple::SF_RecordLogCapabilities;
    using cimple::SF_ElementCapabilities;
    using cimple::SF_UseOfLog;
    using cimple::SF_LogManagesRecord;
    using cimple::SF_RegisteredProfile;
    using cimple::SF_ReferencedProfile;

    class LogEntryHelper : public CIMHelper {
        const Logger *logger(unsigned& idx) const;
    public:
        virtual unsigned nObjects(const SystemElement&) const;
        virtual cimple::Instance *instance(const SystemElement&, unsigned idx) const;
        virtual cimple::Instance *reference(const SystemElement&, unsigned idx) const;
        virtual bool match(const SystemElement&, const Instance& inst, unsigned idx) const;
        static String logInstanceID(const String& s, unsigned idx);
        static unsigned indexToLogIndex(unsigned idx);
    };

    class RecordLogHelper : public CIMHelper {
    public:
        virtual unsigned nObjects(const SystemElement&) const;
        virtual Instance *instance(const SystemElement&, unsigned idx) const;
        virtual Instance *reference(const SystemElement&, unsigned idx) const;
        virtual bool match(const SystemElement&, const Instance& inst, unsigned idx) const;
    };

    class RecordLogCapabilitiesHelper : public CIMHelper {
    public:
        virtual unsigned nObjects(const SystemElement&) const;
        virtual Instance *instance(const SystemElement&, unsigned idx) const;
        virtual Instance *reference(const SystemElement&, unsigned idx) const;
    };

    class RecordLogUseOfLogHelper : public CIMHelper {
    public:
        virtual unsigned nObjects(const SystemElement&) const;
        virtual Instance *instance(const SystemElement&, unsigned idx) const;
    };

    class RecordLogManagesRecordHelper : public CIMHelper {
    public:
        virtual unsigned nObjects(const SystemElement&) const;
        virtual Instance *instance(const SystemElement&, unsigned idx) const;
    };

    class RegisteredProfileHelper : public CIMHelper {
    public:
        virtual unsigned nObjects(const SystemElement&) const;
        virtual Instance *instance(const SystemElement&, unsigned idx) const;
        virtual Instance *reference(const SystemElement&, unsigned idx) const;
        virtual bool match(const SystemElement&, const Instance& inst, unsigned idx) const;
    };

    class ReferencedProfileHelper : public CIMHelper {
        const DMTFProfileInfo *refProfile(unsigned idx, const DMTFProfileInfo*& ref) const;
    public:
        virtual unsigned nObjects(const SystemElement&) const;
        virtual Instance *instance(const SystemElement&, unsigned idx) const;
    };

    String LogEntryHelper::logInstanceID(const String& s, unsigned idx)
    {
        Buffer buf;
        buf.appends(CIMHelper::instanceID(s).c_str());
        buf.append('#');
        buf.append_uint64(idx);
        return buf.data();
    }

    unsigned LogEntryHelper::nObjects(const SystemElement&) const
    {
        unsigned n = 0;
        for (unsigned i = 0; Logger::knownLogs[i] != NULL; i++)
            n += Logger::knownLogs[i]->currentSize();
        return n;
    }

    const Logger *LogEntryHelper::logger(unsigned& idx) const
    {
        for (unsigned i = 0;  Logger::knownLogs[i] != NULL; i++)
        {
            if (idx < Logger::knownLogs[i]->currentSize())
                return Logger::knownLogs[i];
        }
        return NULL;
    }

    unsigned LogEntryHelper::indexToLogIndex(unsigned idx)
    {
        for (unsigned i = 0;  Logger::knownLogs[i] != NULL; i++)
        {
            if (idx < Logger::knownLogs[i]->currentSize())
                return i;
        }
        return unsigned(-1);
    }
    
    cimple::Instance *LogEntryHelper::reference(const SystemElement&, unsigned idx) const
    {
        const Logger *log = logger(idx);
        SF_LogEntry *l = SF_LogEntry::create(true);

        l->InstanceID.set(logInstanceID(log->description(), idx));
        return l;
    }

    cimple::Instance *LogEntryHelper::instance(const SystemElement& sys, unsigned idx) const
    {
#if CIM_SCHEMA_VERSION_MINOR == 26
        static unsigned const severityMap[] = {
            SF_LogEntry::_PerceivedSeverity::enum_Fatal_NonRecoverable,
            SF_LogEntry::_PerceivedSeverity::enum_Major,
            SF_LogEntry::_PerceivedSeverity::enum_Minor,
            SF_LogEntry::_PerceivedSeverity::enum_Information,
            SF_LogEntry::_PerceivedSeverity::enum_Information,
        };
#endif
        
        char id[17];
        SF_LogEntry *le = static_cast<SF_LogEntry *>(reference(sys, idx));
        const Logger *log = logger(idx);
        LogEntry entry = log->get(idx);
        le->LogInstanceID.set(CIMHelper::instanceID(log->description()));
        le->LogName.set(log->description());
        sprintf(id, "%16.16llx", entry.id());
        le->ElementName.set(id);
        le->RecordID.set(id);
        le->RecordFormat.set("");
        le->RecordData.set(entry.message());
        le->CreationTimeStamp.set(entry.stamp());
#if CIM_SCHEMA_VERSION_MINOR == 26
        le->PerceivedSeverity.null = false;
        le->PerceivedSeverity.value = severityMap[entry.severity()];
#endif

        return le;
    }

    bool LogEntryHelper::match(const SystemElement&, const Instance& inst, unsigned idx) const
    {
        const cimple::CIM_LogEntry* le = static_cast<const cimple::CIM_LogEntry *>(&inst);
        const Logger *log = logger(idx);
        if (le == NULL)
            return false;
        if (le->InstanceID.null)
            return false;
        return le->InstanceID.value == instanceID(log->description());
    }

    unsigned RecordLogHelper::nObjects(const SystemElement&) const
    {
        unsigned n = 0;
        for (unsigned i = 0; Logger::knownLogs[i] != NULL; i++)
            n++;
        return n;
    }

    Instance *RecordLogHelper::reference(const SystemElement&, unsigned idx) const
    {
        SF_RecordLog *newLog = SF_RecordLog::create(true);
        
        newLog->InstanceID.set(CIMHelper::instanceID(Logger::knownLogs[idx]->description()));
        return newLog;
    }

    Instance *RecordLogHelper::instance(const SystemElement& sys, unsigned idx) const
    {
        SF_RecordLog *newLog = static_cast<SF_RecordLog *>(reference(sys, idx));
        const Logger *log = Logger::knownLogs[idx];

        newLog->Name.set(log->description());
        newLog->ElementName.set(log->description());
        newLog->Description.set(log->description());
        newLog->OperationalStatus.null = false;
        newLog->OperationalStatus.value.append(SF_RecordLog::_OperationalStatus::enum_OK);
        newLog->HealthState.null = false;
        newLog->HealthState.value = SF_RecordLog::_HealthState::enum_OK;
        newLog->EnabledState.null = false;
        newLog->EnabledState.value = (log->isEnabled() ? 
                                      SF_RecordLog::_EnabledState::enum_Enabled :
                                      SF_RecordLog::_EnabledState::enum_Disabled);
        newLog->RequestedState.null = false;
        newLog->RequestedState.value = SF_RecordLog::_RequestedState::enum_No_Change;
        newLog->LogState.null = false;
        newLog->LogState.value = (log->isEnabled() ?
                                  SF_RecordLog::_LogState::enum_Normal :
                                  SF_RecordLog::_LogState::enum_Not_Applicable);
        newLog->OverwritePolicy.null = false;
        newLog->OverwritePolicy.value = SF_RecordLog::_OverwritePolicy::enum_Wraps_When_Full;
        newLog->MaxNumberOfRecords.set(log->logSize());
        newLog->CurrentNumberOfRecords.set(log->currentSize());
        
        return newLog;
    }

    bool RecordLogHelper::match(const SystemElement&, const Instance& instance, unsigned idx) const
    {
        const cimple::CIM_RecordLog *logObj = cast<const cimple::CIM_RecordLog *>(&instance);
        if (logObj == NULL)
            return false;

        if (logObj->InstanceID.null)
            return false;
        
        return CIMHelper::instanceID(Logger::knownLogs[idx]->description()) == logObj->InstanceID.value;
    }

    unsigned RecordLogCapabilitiesHelper::nObjects(const SystemElement&) const
    {
        unsigned n = 0;
        for (unsigned i = 0; Logger::knownLogs[i] != NULL; i++)
            n++;
        return n;
    }

    Instance *RecordLogCapabilitiesHelper::reference(const SystemElement&, unsigned idx) const
    {
        SF_RecordLogCapabilities *newRlc = SF_RecordLogCapabilities::create(true);
        newRlc->InstanceID.set(CIMHelper::instanceID(Logger::knownLogs[idx]->description()));
        newRlc->InstanceID.value.append(" Capabilities");

        return newRlc;
    }

    Instance *RecordLogCapabilitiesHelper::instance(const SystemElement& sys, unsigned idx) const
    {
        SF_RecordLogCapabilities *newRlc = static_cast<SF_RecordLogCapabilities *>(reference(sys, idx));
        const Logger *log = Logger::knownLogs[idx];

        
        newRlc->Description.set(log->description());
        newRlc->ElementName.set(log->description());
        
        newRlc->SupportedRecordTypes.null = false;
        newRlc->SupportedRecordTypes.value.append(SF_RecordLogCapabilities::_SupportedRecordTypes::enum_Record_Data);
        newRlc->ElementNameEditSupported.set(false);
        newRlc->RequestedStatesSupported.null = false;

        return newRlc;
    }

    unsigned RecordLogUseOfLogHelper::nObjects(const SystemElement& sys) const
    {
        static const RecordLogHelper delegate;
        return delegate.nObjects(sys);
    }

    Instance *RecordLogUseOfLogHelper::instance(const SystemElement& se, unsigned idx) const
    {
        Instance *log = se.cimReference(SF_RecordLog::static_meta_class, idx);
        CIM_ComputerSystem *sys = solarflare::CIMHelper::findSystem()->clone();
        SF_UseOfLog *link = SF_UseOfLog::create(true);
        
        link->Antecedent = static_cast<cimple::CIM_Log *>(log);
        link->Dependent = cast<cimple::CIM_ManagedSystemElement *>(sys);
        return link;
    }

    unsigned RecordLogManagesRecordHelper::nObjects(const SystemElement& sys) const
    {
        static const LogEntryHelper delegate;
        return delegate.nObjects(sys);
    }

    Instance *RecordLogManagesRecordHelper::instance(const SystemElement& se, unsigned idx) const
    {
        Instance *entry = se.cimReference(SF_LogEntry::static_meta_class, idx);
        unsigned logId = LogEntryHelper::indexToLogIndex(idx);
        
        SF_LogManagesRecord *link = SF_LogManagesRecord::create(true);

        link->Log = cast<cimple::CIM_Log *>(se.cimReference(SF_RecordLog::static_meta_class, idx));
        link->Record = static_cast<cimple::CIM_RecordForLog *>(entry);

        return link;
    }

    unsigned RegisteredProfileHelper::nObjects(const SystemElement&) const
    {
        unsigned n = 0;
        for (unsigned i = 0; DMTFProfileInfo::knownDMTFProfiles[i] != NULL; i++)
            n++;
        return n;
    }

    Instance *RegisteredProfileHelper::reference(const SystemElement&, unsigned idx) const 
    {
        return DMTFProfileInfo::knownDMTFProfiles[idx]->reference();
    }

    Instance *RegisteredProfileHelper::instance(const SystemElement& sys, unsigned idx) const
    {
        SF_RegisteredProfile *prof = static_cast<SF_RegisteredProfile *>(reference(sys, idx));
        prof->RegisteredOrganization.null = false;
        prof->RegisteredOrganization.value = SF_RegisteredProfile::_RegisteredOrganization::enum_DMTF;
        prof->RegisteredName.set(DMTFProfileInfo::knownDMTFProfiles[idx]->profileName());
        prof->RegisteredVersion.set(DMTFProfileInfo::knownDMTFProfiles[idx]->profileVersion());
        prof->AdvertiseTypes.null = false;
        prof->AdvertiseTypes.value.append(SF_RegisteredProfile::_AdvertiseTypes::enum_SLP);
        return prof;
    }

    bool RegisteredProfileHelper::match(const SystemElement& sys, const Instance& inst, unsigned idx) const
    {
        const cimple::CIM_RegisteredProfile *prof = static_cast<const cimple::CIM_RegisteredProfile *>(&inst);
        
        if (prof == NULL)
            return false;
        if (prof->InstanceID.null)
            return false;
        
        return prof->InstanceID.value == DMTFProfileInfo::knownDMTFProfiles[idx]->profileId();
    }

    unsigned ReferencedProfileHelper::nObjects(const SystemElement&) const
    {
        unsigned n = 0;
        for (unsigned i = 0; DMTFProfileInfo::knownDMTFProfiles[i] != NULL; i++)
        {
            const DMTFProfileInfo * const *refs = DMTFProfileInfo::knownDMTFProfiles[i]->referencedProfiles();
            if (refs != NULL)
            {
                for (unsigned j = 0; refs[j] != NULL; j++)
                    n++;
            }
        }
        return n;
    }

    const DMTFProfileInfo *ReferencedProfileHelper::refProfile(unsigned idx, const DMTFProfileInfo*& ref) const
    {
        for (unsigned i = 0; DMTFProfileInfo::knownDMTFProfiles[i] != NULL; i++)
        {
            const DMTFProfileInfo * const *refs = DMTFProfileInfo::knownDMTFProfiles[i]->referencedProfiles();
            if (refs != NULL)
            {
                for (unsigned j = 0; refs[j] != NULL; j++)
                {
                    if (idx == 0)
                    {
                        ref = refs[j];
                        return DMTFProfileInfo::knownDMTFProfiles[i];
                    }
                    else
                    {
                        idx--;
                    }
                }
            }
        }
        ref = NULL;
        return NULL;
    }

    Instance *ReferencedProfileHelper::instance(const SystemElement&, unsigned idx) const
    {
        const DMTFProfileInfo *ref;
        const DMTFProfileInfo *base = refProfile(idx, ref);
        SF_ReferencedProfile *prof = SF_ReferencedProfile::create(true);
                    
        prof->Dependent = cast<cimple::CIM_RegisteredProfile *>(base->reference());
        prof->Antecedent = cast<cimple::CIM_RegisteredProfile *>(ref->reference());

        return prof;
    }


    const CIMHelper *System::cimDispatch(const Meta_Class& cls) const
    {
        static const LogEntryHelper logEntryHelper;
        static const RecordLogHelper recordLogHelper;
        static const RecordLogCapabilitiesHelper recordLogCapabilitiesHelper;
        static const RecordLogUseOfLogHelper useOfLogHelper;
        static const RecordLogManagesRecordHelper logManagesRecordHelper;
        static const RegisteredProfileHelper registeredProfileHelper;
        static const ReferencedProfileHelper referencedProfileHelper;
        static const ElementCapabilitiesHelper capsLink(SF_RecordLog::static_meta_class,
                                                        SF_RecordLogCapabilities::static_meta_class);

        if (&cls == &SF_LogEntry::static_meta_class)
            return &logEntryHelper;
        if (&cls == &SF_RecordLog::static_meta_class)
            return &recordLogHelper;
        if (&cls == &SF_RecordLogCapabilities::static_meta_class)
            return &recordLogCapabilitiesHelper;          
        if (&cls == &SF_UseOfLog::static_meta_class)
            return &useOfLogHelper;
        if (&cls == &SF_LogManagesRecord::static_meta_class)
            return &logManagesRecordHelper;
        if (&cls == &SF_RegisteredProfile::static_meta_class)
            return &registeredProfileHelper;
        if (&cls == &SF_ReferencedProfile::static_meta_class)
            return &referencedProfileHelper;
        if (&cls == &SF_ElementCapabilities::static_meta_class)
            return &capsLink;
        return NULL;
    }

} // namespace
