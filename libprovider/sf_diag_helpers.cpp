/***************************************************************************//*! \file liprovider/sf_diag_helpers.cpp
** <L5_PRIVATE L5_SOURCE>
** \author  OktetLabs
**  \brief  CIM Provider
**   \date  2013/10/02
**    \cop  (c) Solarflare Communications Inc.
** </L5_PRIVATE>
*//*
\**************************************************************************/

#include "sf_provider.h"
#include "SF_DiagnosticTest.h"
#include "SF_ConcreteJob.h"
#include "SF_AffectedJobElement.h"
#include "SF_AvailableDiagnosticService.h"
#include "SF_PortController.h"
#include "SF_NICCard.h"
#include "SF_DiagnosticLog.h"
#include "SF_DiagnosticLogCapabilities.h"
#include "SF_DiagnosticCompletionRecord.h"
#include "SF_DiagnosticUseOfLog.h"
#include "SF_LogManagesRecord.h"
#include "SF_ElementConformsToProfile.h"
#include "SF_RecordAppliesToElement.h"
#include "SF_DiagnosticServiceCapabilities.h"
#include "SF_ElementCapabilities.h"
#include "SF_DiagElementSoftwareIdentity.h"
#include "SF_SoftwareIdentity.h"
#include "SF_HostedService.h"
#include "SF_OwningJobElement.h"
#include "SF_ServiceAffectsController.h"

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
    using cimple::SF_AffectedJobElement;
    using cimple::SF_PortController;
    using cimple::SF_NICCard;
    using cimple::SF_AvailableDiagnosticService;
    using cimple::SF_ElementConformsToProfile;
    using cimple::SF_DiagnosticCompletionRecord;
    using cimple::SF_DiagnosticUseOfLog;
    using cimple::SF_LogManagesRecord;
    using cimple::SF_RecordAppliesToElement;
    using cimple::SF_DiagnosticLogCapabilities;
    using cimple::SF_DiagnosticServiceCapabilities;
    using cimple::SF_ElementCapabilities;
    using cimple::SF_DiagElementSoftwareIdentity;
    using cimple::SF_SoftwareIdentity;
    using cimple::SF_HostedService;
    using cimple::SF_OwningJobElement;
    using cimple::SF_ServiceAffectsController;

    class DiagnosticTestHelper : public CIMHelper {
    public:
        virtual Instance *reference(const SystemElement& obj, unsigned) const;
        virtual Instance *instance(const SystemElement&, unsigned) const;
        virtual bool match(const SystemElement& obj, const Instance& inst, unsigned) const;
    };

    class DiagnosticLogHelper : public CIMHelper {
    public:
        virtual Instance *reference(const SystemElement& obj, unsigned) const;
        virtual Instance *instance(const SystemElement&, unsigned) const;
        virtual bool match(const SystemElement& obj, const Instance& inst, unsigned) const;
    };

    class DiagnosticServiceCapabilitiesHelper : public CIMHelper {
    public:
        virtual Instance *reference(const SystemElement& obj, unsigned) const;
        virtual Instance *instance(const SystemElement&, unsigned) const;
    };

    class DiagnosticLogCapabilitiesHelper : public CIMHelper {
    public:
        virtual Instance *reference(const SystemElement& obj, unsigned) const;
        virtual Instance *instance(const SystemElement&, unsigned) const;
    };

    class DiagnosticCompletionRecordHelper : public CIMHelper {
        static String recordID(const String&, unsigned);
    public:
        virtual unsigned nObjects(const SystemElement& obj) const;
        virtual Instance *reference(const SystemElement& obj, unsigned) const;
        virtual Instance *instance(const SystemElement&, unsigned) const;
        virtual bool match(const SystemElement& obj, const Instance& inst, unsigned) const;
    };


    class DiagnosticJobHelper : public ConcreteJobAbstractHelper {
    protected:
        virtual const char *threadSuffix() const { return "diagThread"; }
    };

    class AffectedJobElementHelper : public CIMHelper {
        virtual Instance *instance(const SystemElement &se, unsigned) const;
    };

    class DiagnosticOwningJobElementHelper : public CIMHelper {
        virtual Instance *instance(const SystemElement &se, unsigned) const;
    };


    class AvailableDiagnosticServiceHelper : public CIMHelper {
        virtual Instance *instance(const SystemElement &se, unsigned) const;
    };

    class DiagnosticUseOfLogHelper : public CIMHelper {
    public:
        virtual Instance *instance(const SystemElement &se, unsigned) const;
    };

    class DiagnosticLogManagesRecordHelper : public CIMHelper {
    public:
        virtual unsigned nObjects(const SystemElement& obj) const;
        virtual Instance *instance(const SystemElement &se, unsigned) const;
    };

    class DiagnosticRecordAppliesToElementHelper : public CIMHelper {
    public:
        virtual unsigned nObjects(const SystemElement& obj) const;
        virtual Instance *instance(const SystemElement &se, unsigned) const;
    };

    class DiagnosticElementSoftwareIdentityHelper : public CIMHelper {
    public:
        virtual Instance *instance(const SystemElement &se, unsigned) const;
    };

    class DiagnosticServiceAffectsElementHelper : public CIMHelper {
    public:
        virtual Instance *instance(const SystemElement &se, unsigned) const;
    };

    class DiagnosticHostedServiceHelper : public CIMHelper {
    public:
        virtual Instance *instance(const SystemElement &se, unsigned) const;
    };

    class DiagnosticConformsToProfile : public CIMHelper {
        virtual unsigned nObjects(const SystemElement &se) const;
        virtual Instance *instance(const SystemElement &se, unsigned) const;
    };

    class LogAndServiceCapsLinkHelper : public CIMHelper {
        ElementCapabilitiesHelper logCaps;
        ElementCapabilitiesHelper serviceCaps;
    public:
        LogAndServiceCapsLinkHelper() :
            logCaps(SF_DiagnosticLog::static_meta_class,
                    SF_DiagnosticLogCapabilities::static_meta_class),
            serviceCaps(SF_DiagnosticTest::static_meta_class,
                        SF_DiagnosticServiceCapabilities::static_meta_class) {}
        virtual unsigned nObjects(const SystemElement&) const { return 2; }
        virtual Instance *instance(const SystemElement& obj, unsigned) const;
    };
        
    const CIMHelper* Diagnostic::cimDispatch(const Meta_Class& cls) const
    {
        static const DiagnosticTestHelper diagnosticTest;
        static const DiagnosticJobHelper diagnosticJob;
        static const DiagnosticLogHelper diagnosticLog;
        static const AffectedJobElementHelper affectedJobElement;
        static const DiagnosticOwningJobElementHelper owningJobElement;
        static const AvailableDiagnosticServiceHelper availableDiagnosticService;
        static const DiagnosticUseOfLogHelper useOfLog;
        static const DiagnosticLogManagesRecordHelper logManagesRecord;
        static const DiagnosticRecordAppliesToElementHelper recordAppliesToElement;
        static const DiagnosticCompletionRecordHelper diagnosticCompletionRecord;
        static const DiagnosticConformsToProfile conforming;
        static const DiagnosticLogCapabilitiesHelper logCaps;
        static const DiagnosticServiceCapabilitiesHelper serviceCaps;
        static const LogAndServiceCapsLinkHelper capsLink;
        static const DiagnosticElementSoftwareIdentityHelper elementSoftwareIdentity;
        static const DiagnosticHostedServiceHelper hostedService;
        static const DiagnosticServiceAffectsElementHelper serviceAffectsElement;

        if (&cls == &SF_DiagnosticTest::static_meta_class)
            return &diagnosticTest;
        if (&cls == &SF_DiagnosticLog::static_meta_class)
            return &diagnosticLog;
        if (&cls == &SF_AvailableDiagnosticService::static_meta_class)
            return &availableDiagnosticService;
        if (const_cast<Diagnostic *>(this)->asyncThread() != NULL)
        {
            if (&cls == &SF_ConcreteJob::static_meta_class)
                return &diagnosticJob;
            if (&cls == &SF_AffectedJobElement::static_meta_class)
                return &affectedJobElement;
            if (&cls == &SF_OwningJobElement::static_meta_class)
                return &owningJobElement;
        }
        if (&cls == &SF_DiagnosticCompletionRecord::static_meta_class)
            return &diagnosticCompletionRecord;
        if (&cls == &SF_DiagnosticUseOfLog::static_meta_class)
            return &useOfLog;
        if (&cls == &SF_LogManagesRecord::static_meta_class)
            return &logManagesRecord;
        if (&cls == &SF_RecordAppliesToElement::static_meta_class)
            return &recordAppliesToElement;
        if (&cls == &SF_DiagnosticLogCapabilities::static_meta_class)
            return &logCaps;
        if (&cls == &SF_DiagnosticServiceCapabilities::static_meta_class)
            return &serviceCaps;
        if (&cls == &SF_ElementCapabilities::static_meta_class)
            return &capsLink;
        if (&cls == &SF_DiagElementSoftwareIdentity::static_meta_class)
            return &elementSoftwareIdentity;
        if (&cls == &SF_HostedService::static_meta_class)
            return &hostedService;
        if (&cls == &SF_ServiceAffectsController::static_meta_class)
            return &serviceAffectsElement;
        if (&cls == &SF_ElementConformsToProfile::static_meta_class)
            return &conforming;
        
        return NULL;
    }

    Instance *DiagnosticTestHelper::reference(const SystemElement& se, unsigned) const
    {
        const Diagnostic& diag = static_cast<const Diagnostic&>(se);
        const CIM_ComputerSystem *system = findSystem();
        SF_DiagnosticTest *newSvc = SF_DiagnosticTest::create(true);
        
        newSvc->CreationClassName.set("SF_DiagnosticTest");
        newSvc->Name.set(instanceID(diag.name()));
        newSvc->SystemCreationClassName.set(system->CreationClassName.value);
        newSvc->SystemName.set(system->Name.value);

        return newSvc;
    }

    Instance *DiagnosticTestHelper::instance(const solarflare::SystemElement& se, unsigned idx) const 
    {
        const solarflare::Diagnostic& diag = static_cast<const solarflare::Diagnostic&>(se);
    
        SF_DiagnosticTest *newSvc = static_cast<SF_DiagnosticTest *>(reference(diag, idx));

        newSvc->Description.set(diag.description());
#if CIM_SCHEMA_VERSION_MINOR > 0
        newSvc->ElementName.set(diag.name());
#endif
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


    bool DiagnosticTestHelper::match(const SystemElement& se, const Instance& inst, unsigned) const
    {
        const cimple::CIM_DiagnosticTest *test = cast<const cimple::CIM_DiagnosticTest *>(&inst);
        
        if (test == NULL)
            return false;

#if !defined(TARGET_CIM_SERVER_wmi)
        // In earlier Windows CIM schema, there may be no such properties
        // among keys of CIM_DiagnosticTest, so all these would be NULL.
        if (test->CreationClassName.null || test->Name.null || 
            test->CreationClassName.value != "SF_DiagnosticTest" ||
            test->SystemCreationClassName.null)
            return false;
        if (!isOurSystem(test->SystemCreationClassName.value,
                         test->SystemName.null ?
                                  String("") : test->SystemName.value))
            return false;
#endif
        
        return test->Name.value == instanceID(se.name());
    }
    

    Instance *DiagnosticLogHelper::reference(const SystemElement& se, unsigned) const
    {
        SF_DiagnosticLog *newLog = SF_DiagnosticLog::create(true);
        
        newLog->InstanceID.set(instanceID(se.name()));
        newLog->InstanceID.value.append(" ");
        newLog->InstanceID.value.append(static_cast<const Diagnostic&>(se).log().description());

        return newLog;
    }

    Instance *DiagnosticLogHelper::instance(const SystemElement& se, unsigned idx) const
    {
        const Diagnostic& diag = static_cast<const Diagnostic&>(se);
        SF_DiagnosticLog *newLog = static_cast<SF_DiagnosticLog *>(reference(diag, idx));
        
        newLog->Name.set(se.name());
        newLog->Name.value.append(" ");
        newLog->Name.value.append(diag.log().description());
#if CIM_SCHEMA_VERSION_MINOR > 0
        newLog->ElementName.set(diag.log().description());
#endif
        newLog->Description.set(diag.log().description());
#if CIM_SCHEMA_VERSION_MINOR > 0
        newLog->OperationalStatus.null = false;
        newLog->OperationalStatus.value.append(SF_DiagnosticLog::_OperationalStatus::enum_OK);
        newLog->HealthState.null = false;
        newLog->HealthState.value = SF_DiagnosticLog::_HealthState::enum_OK;
        newLog->OperatingStatus.null = false;
        newLog->OperatingStatus.value =
               SF_DiagnosticLog::_OperatingStatus::enum_Not_Available;
#endif
        newLog->EnabledState.null = false;
        newLog->EnabledState.value = (diag.log().isEnabled() ? 
                                      SF_DiagnosticLog::_EnabledState::enum_Enabled :
                                      SF_DiagnosticLog::_EnabledState::enum_Disabled);
        newLog->RequestedState.null = false;
        newLog->RequestedState.value =
                    System::getLogReqState(newLog->InstanceID.value);
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

    bool DiagnosticLogHelper::match(const SystemElement& se, const Instance& inst, unsigned) const
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

    Instance *AffectedJobElementHelper::instance(const SystemElement& se, unsigned) const
    {
        const Diagnostic& diag = static_cast<const Diagnostic&>(se);
    
        SF_AffectedJobElement *link = SF_AffectedJobElement::create(true);
    
        link->AffectedElement = cast<cimple::CIM_PortController *>(diag.nic()->cimReference(SF_PortController::static_meta_class));
        link->AffectingElement = cast<cimple::CIM_Job *>(diag.cimReference(SF_ConcreteJob::static_meta_class));
        link->ElementEffects.null = false;
        if (diag.isExclusive())
        {
            link->ElementEffects.value.append(SF_AffectedJobElement::_ElementEffects::enum_Exclusive_Use);
        }
        
        return link;
    }

    Instance *AvailableDiagnosticServiceHelper::instance(const SystemElement& se, unsigned) const
    {
        const Diagnostic& diag = static_cast<const Diagnostic&>(se);
        SF_AvailableDiagnosticService *link = SF_AvailableDiagnosticService::create(true);
    
        link->ServiceProvided = cast<cimple::CIM_DiagnosticService *>(diag.cimReference(SF_DiagnosticTest::static_meta_class));
        link->UserOfService = cast<cimple::CIM_Card *>(diag.nic()->cimReference(SF_NICCard::static_meta_class));
        link->EstimatedDurationOfService.null = false;
        link->EstimatedDurationOfService.value = SF_AvailableDiagnosticService::_EstimatedDurationOfService::enum_Unknown;

        return link;
    }

    unsigned DiagnosticCompletionRecordHelper::nObjects(const SystemElement& se) const
    {
        return static_cast<const Diagnostic&>(se).log().currentSize();
    }

    String
    DiagnosticCompletionRecordHelper::recordID(const String& d, unsigned idx)
    {
        Buffer buf;
        buf.appends(CIMHelper::instanceID(d).c_str());
        buf.appends(":diagThread:");
        buf.append_uint64(idx);
        return buf.data();
    }

    Instance *
    DiagnosticCompletionRecordHelper::reference(const SystemElement& se, unsigned idx) const
    {
        SF_DiagnosticCompletionRecord *l = SF_DiagnosticCompletionRecord::create(true);
        const Diagnostic& diag = static_cast<const Diagnostic&>(se);
    
        l->InstanceID.set(recordID(diag.name(), idx));
        return l;
    }
    
    Instance *
    DiagnosticCompletionRecordHelper::instance(const SystemElement& se, unsigned idx) const
    {
#if  CIM_SCHEMA_VERSION_MINOR == 26
        static unsigned const severityMap[] = {
            SF_DiagnosticCompletionRecord::_PerceivedSeverity::enum_Fatal_NonRecoverable,
            SF_DiagnosticCompletionRecord::_PerceivedSeverity::enum_Major,
            SF_DiagnosticCompletionRecord::_PerceivedSeverity::enum_Minor,
            SF_DiagnosticCompletionRecord::_PerceivedSeverity::enum_Information,
            SF_DiagnosticCompletionRecord::_PerceivedSeverity::enum_Information,
        };
#endif
        
        char id[32];
        SF_DiagnosticCompletionRecord *le = static_cast<SF_DiagnosticCompletionRecord *>(reference(se, idx));
        const Diagnostic& diag = static_cast<const Diagnostic&>(se);
        const Logger& log = diag.log();
        LogEntry entry = log.get(idx);
        
        le->RecordFormat.set("");
        le->RecordData.set(entry.message());
        le->CreationTimeStamp.set(entry.stamp());
        le->ServiceName.set(instanceID(diag.name()));
        le->ManagedElementName.set(diag.nic()->name());
        le->ExpirationDate.set(Datetime::now());
        le->RecordType.null = false;
        le->RecordType.value = SF_DiagnosticCompletionRecord::_RecordType::enum_Results;
       
#if  CIM_SCHEMA_VERSION_MINOR == 26 
        le->PerceivedSeverity.null = false;
        le->PerceivedSeverity.value = severityMap[entry.severity()];
#endif
        
        le->LoopsPassed.set(entry.passed());
        le->LoopsFailed.set(entry.failed());
        sprintf(id, "%8.8x", entry.error());
        le->ErrorCode.null = false;
        le->ErrorCode.value.append(String(id));
        le->ErrorCount.null = false;
        le->CompletionState.null = false;
        if (entry.error() != 0 || entry.failed() != 0)
        {
            le->ErrorCount.value.append(entry.failed() ? entry.failed() : 1);
            le->CompletionState.value = SF_DiagnosticCompletionRecord::_CompletionState::enum_Failed;
        }
        else
        {
            le->ErrorCount.value.append(0);
            le->CompletionState.value = SF_DiagnosticCompletionRecord::_CompletionState::enum_OK;
        }
        
        return le;
    }

    bool
    DiagnosticCompletionRecordHelper::match(const SystemElement& se, const Instance& inst, unsigned idx) const 
    {
        const cimple::CIM_DiagnosticCompletionRecord *l = cast<const cimple::CIM_DiagnosticCompletionRecord *>(&inst);
        const Diagnostic& diag = static_cast<const Diagnostic &>(se);

        if (l == NULL)
            return false;
        
        if (l->InstanceID.null)
            return false;
        
        return l->InstanceID.value == recordID(diag.name(), idx);
    }

    Instance *DiagnosticUseOfLogHelper::instance(const solarflare::SystemElement& se, unsigned) const
    {
        const solarflare::Diagnostic& diag = static_cast<const solarflare::Diagnostic&>(se);
    
        SF_DiagnosticUseOfLog *link = SF_DiagnosticUseOfLog::create(true);
        
        link->Antecedent = cast<cimple::CIM_Log *>(diag.cimReference(SF_DiagnosticLog::static_meta_class));
        link->Dependent = cast<cimple::CIM_DiagnosticTest *>(diag.cimReference(SF_DiagnosticTest::static_meta_class));

        return link;
    }

    unsigned DiagnosticLogManagesRecordHelper::nObjects(const SystemElement& se) const
    {
        static const DiagnosticCompletionRecordHelper delegate;
        return delegate.nObjects(se);
    }

    Instance *
    DiagnosticLogManagesRecordHelper::instance(const SystemElement& se, unsigned idx) const
    {
        SF_LogManagesRecord *link = SF_LogManagesRecord::create(true);
        
        link->Log = cast<cimple::CIM_Log *>(se.cimReference(SF_DiagnosticLog::static_meta_class));
        link->Record = cast<cimple::CIM_RecordForLog *>(se.cimReference(SF_DiagnosticCompletionRecord::static_meta_class, idx));

        return link;
    }    

    unsigned DiagnosticRecordAppliesToElementHelper::nObjects(const SystemElement& se) const
    {
        static const DiagnosticCompletionRecordHelper delegate;
        return delegate.nObjects(se);
    }

    Instance *
    DiagnosticRecordAppliesToElementHelper::instance(const SystemElement& se, unsigned idx) const
    {
        SF_RecordAppliesToElement *link = SF_RecordAppliesToElement::create(true);
        const Diagnostic& diag = static_cast<const Diagnostic&>(se);
    
        link->Antecedent = cast<cimple::CIM_RecordForLog *>(se.cimReference(SF_DiagnosticCompletionRecord::static_meta_class, idx));
        link->Dependent = cast<cimple::CIM_ManagedElement *>(diag.nic()->cimReference(SF_NICCard::static_meta_class));
        
        return link;
    }

    Instance *LogAndServiceCapsLinkHelper::instance(const SystemElement& se, unsigned idx) const
    {
        switch (idx)
        {
            case 0:
                return logCaps.instance(se, 0);
            case 1:
                return serviceCaps.instance(se, 0);
            default:
                return NULL;
        }
    }

    Instance *DiagnosticServiceCapabilitiesHelper::reference(const SystemElement& se, unsigned) const
    {
        SF_DiagnosticServiceCapabilities *newSvc = SF_DiagnosticServiceCapabilities::create(true);

        newSvc->InstanceID.set(instanceID(se.name()));
        newSvc->InstanceID.value.append(" Capabilities");
        return newSvc;
    }

    Instance *DiagnosticServiceCapabilitiesHelper::instance(const SystemElement& se, unsigned idx) const
    {
        const solarflare::Diagnostic& diag = static_cast<const Diagnostic&>(se);
        SF_DiagnosticServiceCapabilities *newSvc = static_cast<SF_DiagnosticServiceCapabilities *>(reference(se, idx));

        newSvc->Description.set(diag.description());
        newSvc->ElementName.set(diag.name());

        newSvc->SupportedLogOptions.null = false;
        newSvc->SupportedLogOptions.value.append(SF_DiagnosticServiceCapabilities::_SupportedLogOptions::enum_Results);
        
        newSvc->SupportedLogStorage.null = false;
#if CIM_SCHEMA_VERSION_MINOR < 28 || CIM_SCHEMA_VERSION_MINOR >= 32
        newSvc->SupportedLogStorage.value.append(SF_DiagnosticServiceCapabilities::_SupportedLogStorage::enum_DiagnosticLog);
#else
        newSvc->SupportedLogStorage.value.append(SF_DiagnosticServiceCapabilities::_SupportedLogStorage::enum_DiagnosticRecordLog);
#endif
        newSvc->SupportedLogStorage.value.append(SF_DiagnosticServiceCapabilities::_SupportedLogStorage::enum_File);
        
        newSvc->SupportedExecutionControls.null = false;
        newSvc->SupportedExecutionControls.value.append(SF_DiagnosticServiceCapabilities::_SupportedExecutionControls::enum_Terminate_Job);
        
        return newSvc;
    }

    Instance *DiagnosticLogCapabilitiesHelper::reference(const SystemElement& se, unsigned) const
    {
        SF_DiagnosticLogCapabilities *newDlc = SF_DiagnosticLogCapabilities::create(true);

        newDlc->InstanceID.set(instanceID(se.name()));
        newDlc->InstanceID.value.append(" ");
        newDlc->InstanceID.value.append(static_cast<const Diagnostic&>(se).log().description());
        newDlc->InstanceID.value.append(" Capabilities");
        return newDlc;
    }

    Instance *DiagnosticLogCapabilitiesHelper::instance(const SystemElement& se, unsigned idx) const
    {
        const solarflare::Diagnostic& diag = static_cast<const Diagnostic&>(se);
        SF_DiagnosticLogCapabilities *newDlc = static_cast<SF_DiagnosticLogCapabilities *>(reference(se, idx));

        newDlc->Description.set(diag.description());
        newDlc->ElementName.set(diag.name());

        newDlc->SupportedRecordTypes.null = false;
        newDlc->SupportedRecordTypes.value.append(SF_DiagnosticLogCapabilities::_SupportedRecordTypes::enum_Record_Data);
        newDlc->ElementNameEditSupported.set(false);

        static const cimple::uint16 states[] = {
            SF_DiagnosticLogCapabilities::_RequestedStatesSupported::enum_Disabled,
            SF_DiagnosticLogCapabilities::_RequestedStatesSupported::enum_Enabled,
            SF_DiagnosticLogCapabilities::_RequestedStatesSupported::enum_Reset,
        };
        newDlc->RequestedStatesSupported.null = false;
        newDlc->RequestedStatesSupported.value.append(states, sizeof(states) / sizeof(*states));

        return newDlc;
    }

    Instance *DiagnosticElementSoftwareIdentityHelper::instance(const solarflare::SystemElement& se, unsigned) const
    {
        const Diagnostic& diag = static_cast<const solarflare::Diagnostic&>(se);
        const SWElement *tool = diag.diagnosticTool();

        if (tool == NULL)
            return NULL;
        
        SF_DiagElementSoftwareIdentity *item = SF_DiagElementSoftwareIdentity::create(true);
    
        item->Antecedent =
          cast<cimple::CIM_SoftwareIdentity *>(
                          tool->cimReference(
                                 SF_SoftwareIdentity::static_meta_class));
        item->Dependent = cast<cimple::CIM_DiagnosticTest *>(diag.cimReference(SF_DiagnosticTest::static_meta_class));
        item->Dependent = cast<cimple::CIM_DiagnosticTest *>(diag.cimReference(SF_DiagnosticTest::static_meta_class));
        item->ElementSoftwareStatus.null = false;
        item->ElementSoftwareStatus.value.append(SF_DiagElementSoftwareIdentity::_ElementSoftwareStatus::enum_Current);
        item->ElementSoftwareStatus.value.append(SF_DiagElementSoftwareIdentity::_ElementSoftwareStatus::enum_Next);
        item->ElementSoftwareStatus.value.append(SF_DiagElementSoftwareIdentity::_ElementSoftwareStatus::enum_Default);

        return item;
    }

    Instance * 
    DiagnosticHostedServiceHelper::instance(const solarflare::SystemElement& se, unsigned) const
    {
        SF_HostedService *link = SF_HostedService::create(true);
        link->Antecedent = solarflare::CIMHelper::systemRef();
        link->Dependent = cast<cimple::CIM_Service *>(se.cimReference(SF_DiagnosticTest::static_meta_class));
#if NEED_ASSOC_IN_ROOT_CIMV2
        link->Dependent->__name_space = CIMHelper::solarflareNS;
#endif 
        return link;
    }

    Instance * 
    DiagnosticServiceAffectsElementHelper::instance(const solarflare::SystemElement& se, unsigned) const
    {
        const Diagnostic& diag = static_cast<const Diagnostic&>(se);
        SF_ServiceAffectsController *link = SF_ServiceAffectsController::create(true);
        
        link->AffectedElement = cast<cimple::CIM_PortController *>(diag.nic()->cimReference(SF_PortController::static_meta_class));
        link->AffectingElement = cast<cimple::CIM_Service *>(diag.cimReference(SF_DiagnosticTest::static_meta_class));

        return link;
    }


    Instance *
    DiagnosticOwningJobElementHelper::instance(const solarflare::SystemElement& se, unsigned) const
    {
        const solarflare::Diagnostic& diag = static_cast<const solarflare::Diagnostic&>(se);
        
        if (const_cast<Diagnostic&>(diag).asyncThread() == NULL)
            return NULL;

        SF_OwningJobElement *link = SF_OwningJobElement::create(true);
        
        link->OwningElement = cast<cimple::CIM_DiagnosticTest *>(diag.cimReference(SF_DiagnosticTest::static_meta_class));
        link->OwnedElement = cast<cimple::CIM_Job *>(diag.cimReference(SF_ConcreteJob::static_meta_class));
        
        return link;
    }

    unsigned DiagnosticConformsToProfile::nObjects(const SystemElement &se) const
    {
#if !TARGET_CIM_SERVER_esxi
        return CIMHelper::nObjects(se) + (se.cimDispatch(SF_ConcreteJob::static_meta_class) != NULL);
#else
        // On ESXi Job Control profile support is not declared due to
        // incorrect CIM PAT Provides Tags Validation test.
        return CIMHelper::nObjects(se);
#endif
    }
    
    Instance *DiagnosticConformsToProfile::instance(const SystemElement &se, unsigned idx) const
    {
        switch (idx)
        {
            case 0:
                return DMTFProfileInfo::DiagnosticsProfile.          \
                conformingElement(se.cimReference(SF_DiagnosticTest::static_meta_class));
            case 1:
                return DMTFProfileInfo::JobControlProfile.              \
                conformingElement(se.cimReference(SF_ConcreteJob::static_meta_class));
            default:
                return NULL;
        }
    }
    
} // namespace
