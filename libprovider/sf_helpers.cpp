/***************************************************************************//*! \file liprovider/sf_helpers.cpp
** <L5_PRIVATE L5_SOURCE>
** \author  OktetLabs
**  \brief  CIM Provider
**   \date  2013/10/02
**    \cop  (c) Solarflare Communications Inc.
** </L5_PRIVATE>
*//*
\**************************************************************************/

#include "sf_provider.h"
#include "CIM_ComputerSystem.h"
#include "CIM_Chassis.h"
#include "CIM_ElementConformsToProfile.h"
#include "CIM_RegisteredProfile.h"
#ifdef TARGET_CIM_SERVER_pegasus
#include "IBMPSG_ComputerSystem.h"
#include "IBMSD_ComputerSystem.h"
#include "IBMSD_SPComputerSystem.h"
#include "IBMSD_Chassis.h"
#include "PG_ComputerSystem.h"
#endif
#ifdef TARGET_CIM_SERVER_wmi
#include "Win32_ComputerSystem.h"
#include "Win32_SystemEnclosure.h"
#endif
#ifdef TARGET_CIM_SERVER_esxi
#include "OMC_UnitaryComputerSystem.h"
#include "OMC_Chassis.h"
#endif
#include "SF_ConcreteJob.h"
#include "SF_EnabledLogicalElementCapabilities.h"
#include "SF_ElementCapabilities.h"
#include "SF_RegisteredProfile.h"
#include "SF_ReferencedProfile.h"

#include "sf_alerts.h"
#include "sf_logging.h"

namespace solarflare
{
    using cimple::Instance;
    using cimple::Meta_Class;
    using cimple::CIM_ComputerSystem;
    using cimple::CIM_Chassis;
    using cimple::CIM_ElementConformsToProfile;
    using cimple::CIM_RegisteredProfile;
    using cimple::Ref;
    using cimple::cast;
    using cimple::SF_ConcreteJob;
    using cimple::SF_EnabledLogicalElementCapabilities;
    using cimple::SF_ElementCapabilities;
    using cimple::SF_RegisteredProfile;
    using cimple::SF_ReferencedProfile;
    using cimple::SF_ElementConformsToProfile;

    cimple::Instance *SystemElement::cimReference(const cimple::Meta_Class& cls, unsigned idx) const
    {
        const CIMHelper *helper = cimDispatch(cls);
        return helper ? helper->reference(*this, idx) : NULL;
    }

    const char CIMHelper::solarflareNS[] = SF_IMPLEMENTATION_NS;
    const char CIMHelper::ibmseNS[] = "root/ibmse";
    const char CIMHelper::ibmsdNS[] = "root/ibmsd";
    const char CIMHelper::interopNS[] = SF_INTEROP_NS;
    const char CIMHelper::baseNS[] = "root/cimv2";

    String CIMHelper::instanceID(const String& name)
    {
        String result = System::target.prefix();
        result.append(":");
        result.append(name);
        return result;
    }

    Ref<CIM_ComputerSystem> CIMHelper::cimSystem;
    Ref<CIM_Chassis>        CIMHelper::cimChassis;

    const CIM_ComputerSystem *CIMHelper::findSystem()
    {
        static cimple::Mutex findSystemMutex;
        static const char * const namespaces[] =
        {
#ifdef TARGET_CIM_SERVER_pegasus
         ibmseNS, 
#endif
         baseNS, 
         solarflareNS, 
         NULL
        };
        static const Meta_Class * const csysMetaclasses[] = 
        {
#ifdef TARGET_CIM_SERVER_pegasus
            &cimple::IBMPSG_ComputerSystem::static_meta_class,
            &cimple::IBMSD_ComputerSystem::static_meta_class,
            &cimple::IBMSD_SPComputerSystem::static_meta_class,
            &cimple::PG_ComputerSystem::static_meta_class,
#endif
#ifdef TARGET_CIM_SERVER_wmi
            &cimple::Win32_ComputerSystem::static_meta_class,
#endif
#ifdef TARGET_CIM_SERVER_esxi
            &cimple::OMC_UnitaryComputerSystem::static_meta_class,
#endif
            &CIM_ComputerSystem::static_meta_class,
            NULL
        };

        cimple::Auto_Mutex guard(findSystemMutex);

        if (cimSystem)
            return cast<CIM_ComputerSystem *>(cimSystem.ptr());

        // Try to find Base Server Profile instance
        Ref<Instance> cimInstance;
        Ref<CIM_RegisteredProfile> cimRPModel = CIM_RegisteredProfile::create();
        Ref<CIM_RegisteredProfile> cimBSP;

        cimple::Instance_Enumerator profIE;

        if (cimple::cimom::enum_instances(interopNS,
                cimRPModel.ptr(), profIE) == 0)
        {
            for (cimInstance = profIE(); !!cimInstance;
                 profIE++, cimInstance = profIE())
            {
                cimBSP.reset(cast<CIM_RegisteredProfile *>
                                              (cimInstance.ptr()));
                if (!(cimBSP->RegisteredName.null) &&
                    strcmp(cimBSP->RegisteredName.value.c_str(),
                            "Base Server") == 0)
                    break;
                cimBSP.reset(cast<CIM_RegisteredProfile *>(NULL));
            }

            if (cimBSP.ptr())
            {
                // Try to find association with Base Server profile
                for (const char * const *ns = namespaces; *ns != NULL; ns++)
                {
                    if (strcmp(*ns, solarflareNS) == 0)
                        continue;
     
                    Ref<Instance> assocInstance;
                    Ref<CIM_ElementConformsToProfile> cimAssocModel =
                                    CIM_ElementConformsToProfile::create();
                    Ref<CIM_ElementConformsToProfile> cimAssoc;
                    cimple::Instance_Enumerator assocIE;

                    if (cimple::cimom::enum_instances(*ns,
                                                      cimAssocModel.ptr(),
                                                      assocIE) != 0)
                    {
                        PROVIDER_LOG_ERR("%s():    failed to enumerate "
                                         "CIM_ElementConformsToProfile",
                                         __FUNCTION__);
                        if (cimSystem.ptr() == NULL)
                            PROVIDER_LOG_ERR("%s():    returning NULL "
                                             "as system", __FUNCTION__);
                        return cimSystem.ptr();
                    }
                   
                    for (assocInstance = assocIE(); !!assocInstance;
                         assocIE++, assocInstance = assocIE())
                    {
                        cimAssoc.reset(
                              cast<CIM_ElementConformsToProfile *>(
                                                   assocInstance.ptr()));
                        if (
                      cimAssoc->ConformantStandard &&
                      !(cimAssoc->ConformantStandard->InstanceID.null) &&
                      strcmp(cimAssoc->ConformantStandard->
                                          InstanceID.value.c_str(),
                             cimBSP->InstanceID.value.c_str()) == 0)
                        {
                            if (cimAssoc->ManagedElement)
                            {
                                cimSystem.reset(
                                    cast<CIM_ComputerSystem *>(
                                               cimAssoc->ManagedElement));
                                break;
                            }
                        }
                        
                        cimAssoc.reset(
                            cast<CIM_ElementConformsToProfile *>(NULL));
                    }

                    if (cimSystem.ptr())
                        return cimSystem.ptr();     
                }
            }
        }
 
        // There is no instance associated with Base profile - take the first one
        Ref<Instance> sysInstance;

        for (const Meta_Class * const *mcs = csysMetaclasses; 
             *mcs != NULL && !bool(sysInstance); mcs++)
        {
            Ref<CIM_ComputerSystem> system = cimple::create(*mcs);
            for (const char * const *ns = namespaces; 
                 *ns != NULL && !bool(sysInstance); ns++)
            {
                cimple::Instance_Enumerator ie;
                
                if (cimple::cimom::enum_instances(*ns, system.ptr(), ie) != 0)
                    continue;
                
                sysInstance = ie();
            }
        }
        if (sysInstance)
        {
            cimSystem.reset(cast<CIM_ComputerSystem *>(sysInstance.ptr()));
        }
        if (cimSystem.ptr() == NULL)
            PROVIDER_LOG_ERR("%s(): system not found, returning NULL",
                             __FUNCTION__);
        return cimSystem.ptr();
    }

    bool CIMHelper::isOurSystem(const String& sysclass, const String& sysname)
    {
        const CIM_ComputerSystem* ourSys = findSystem();

        return (ourSys->CreationClassName.value == sysclass &&
                ourSys->Name.value == sysname);
    }

    const CIM_Chassis *CIMHelper::findChassis()
    {
        static cimple::Mutex findChassisMutex;
        static const char * const namespaces[] =
        {
#ifdef TARGET_CIM_SERVER_pegasus
         ibmseNS, 
#endif
         baseNS, 
         solarflareNS, 
         NULL
        };
        static const Meta_Class * const chassisMetaclasses[] = 
        {
#ifdef TARGET_CIM_SERVER_pegasus
            &cimple::IBMSD_Chassis::static_meta_class,
#endif
#ifdef TARGET_CIM_SERVER_wmi
            &cimple::Win32_SystemEnclosure::static_meta_class,
#endif
#ifdef TARGET_CIM_SERVER_esxi
            &cimple::OMC_Chassis::static_meta_class,
#endif
            &CIM_Chassis::static_meta_class,
            NULL
        };

        cimple::Auto_Mutex guard(findChassisMutex);

        if (cimChassis)
            return cast<CIM_Chassis *>(cimChassis.ptr());

        Ref<Instance> chassisInstance;

        for (const Meta_Class * const *mcs = chassisMetaclasses;
             *mcs != NULL && !bool(chassisInstance); mcs++)
        {
            Ref<CIM_Chassis> chassis = cimple::create(*mcs);
            for (const char * const *ns = namespaces; 
                 *ns != NULL && !bool(chassisInstance); ns++)
            {
                cimple::Instance_Enumerator ie;
                
                if (cimple::cimom::enum_instances(*ns, chassis.ptr(),
                                                  ie) != 0)
                    continue;
                
                chassisInstance = ie();
            }
        }
        if (chassisInstance)
            cimChassis.reset(cast<CIM_Chassis *>(chassisInstance.ptr()));

        if (cimChassis.ptr() == NULL)
            PROVIDER_LOG_ERR("%s(): chassis not found, returning NULL",
                             __FUNCTION__);
        return cimChassis.ptr();

    }

    CIMInstanceNotify<cimple::SF_JobCreated> onJobCreated(cimple::SF_ConcreteJob::static_meta_class);
#if CIM_SCHEMA_VERSION_MINOR > 0
    CIMJobChangeStateNotify<cimple::SF_JobStarted> onJobStarted(cimple::SF_ConcreteJob::_OperationalStatus::enum_Dormant);
    CIMJobChangeStateNotify<cimple::SF_JobProgress> onJobProgress(cimple::SF_ConcreteJob::_OperationalStatus::enum_OK);
    CIMJobChangeStateNotify<cimple::SF_JobError> onJobError(cimple::SF_ConcreteJob::_OperationalStatus::enum_OK);
    CIMJobChangeStateNotify<cimple::SF_JobSuccess> onJobSuccess(cimple::SF_ConcreteJob::_OperationalStatus::enum_OK);
#else
    CIMJobChangeStateNotify<cimple::SF_JobStarted> onJobStarted(0);
    CIMJobChangeStateNotify<cimple::SF_JobProgress> onJobProgress(0);
    CIMJobChangeStateNotify<cimple::SF_JobError> onJobError(0);
    CIMJobChangeStateNotify<cimple::SF_JobSuccess> onJobSuccess(0);
#endif

    CIMAlertNotify<cimple::SF_PortLinkStateAlert> onPortLinkStateAlert;
    CIMAlertNotify<cimple::SF_PortSensorAlert> onPortSensorAlert;

    bool Action::process(SystemElement& se)
    {
        const CIMHelper *helper = se.cimDispatch(*sample->meta_class);
        if (helper == NULL)
            return true;

        unsigned n = helper->nObjects(se);

        for (unsigned i = 0; i < n; i++)
        {
            if (helper->match(se, *sample, i))
            {
                handler(se, i);
                return false;
            }
        }
        return true;
    }

    bool ActionForAll::process(SystemElement& se)
    {
        const CIMHelper *helper = se.cimDispatch(*sample->meta_class);
        if (helper == NULL)
            return true;

        unsigned n = helper->nObjects(se);

        for (unsigned i = 0; i < n; i++)
            if (helper->match(se, *sample, i))
                handler(se, i);

        return true;
    }

    bool Action::forThread()
    {
        if (System::target.forAllDiagnostics(*this))
            return !System::target.forAllSoftware(*this);
        else
            return true;
    }

    bool Action::forAny()
    {
        if (!process(System::target))
            return true;
        if (!System::target.forAllSoftware(*this))
            return true;
        if (!System::target.forAllNICs(*this))
            return true;
        if (!System::target.forAllInterfaces(*this))
            return true;
        if (!System::target.forAllPorts(*this))
            return true;
        if (!System::target.forAllDiagnostics(*this))
            return true;
        return false;
    }

    Instance *ConcreteJobAbstractHelper::reference(const SystemElement& obj, unsigned) const
    {
        SF_ConcreteJob *job = SF_ConcreteJob::create(true);
        job->InstanceID.set(instanceID(obj.name()));
        job->InstanceID.value.append(":");
        job->InstanceID.value.append(threadSuffix());
        return job;
    }

    Instance *ConcreteJobAbstractHelper::instance(const SystemElement& obj, unsigned idx) const
    {
        Thread *th = const_cast<SystemElement&>(obj).embeddedThread();
        SF_ConcreteJob *job = static_cast<SF_ConcreteJob *>(reference(obj, idx));
        Thread::State st;
        
        job->Name.set(job->InstanceID.value);
#if CIM_SCHEMA_VERSION_MINOR > 0
        job->OperationalStatus.null = false;
#endif
        job->JobState.null = false;

        switch (th->currentState())
        {
            case Thread::NotRun:
#if CIM_SCHEMA_VERSION_MINOR > 0
                job->OperationalStatus.value.append(SF_ConcreteJob::_OperationalStatus::enum_Dormant);
#endif
                job->JobState.value = SF_ConcreteJob::_JobState::enum_New;
                break;
            case Thread::Running:
#if CIM_SCHEMA_VERSION_MINOR > 0
                job->OperationalStatus.value.append(SF_ConcreteJob::_OperationalStatus::enum_OK);
#endif
                job->JobState.value = SF_ConcreteJob::_JobState::enum_Running;
                break;
            case Thread::Succeeded:
#if CIM_SCHEMA_VERSION_MINOR > 0
                job->OperationalStatus.value.append(SF_ConcreteJob::_OperationalStatus::enum_OK);
                job->OperationalStatus.value.append(SF_ConcreteJob::_OperationalStatus::enum_Completed);
#endif
                job->JobState.value = SF_ConcreteJob::_JobState::enum_Completed;
                break;
            case Thread::Failed:
#if CIM_SCHEMA_VERSION_MINOR > 0
                job->OperationalStatus.value.append(SF_ConcreteJob::_OperationalStatus::enum_Error);
                job->OperationalStatus.value.append(SF_ConcreteJob::_OperationalStatus::enum_Completed);
#endif
                job->JobState.value = SF_ConcreteJob::_JobState::enum_Exception;
                break;
            case Thread::Aborting:
#if CIM_SCHEMA_VERSION_MINOR > 0
                job->OperationalStatus.value.append(SF_ConcreteJob::_OperationalStatus::enum_Error);
#endif
                job->JobState.value = SF_ConcreteJob::_JobState::enum_Shutting_Down;
                break;
            case Thread::Aborted:
#if CIM_SCHEMA_VERSION_MINOR > 0
                job->OperationalStatus.value.append(SF_ConcreteJob::_OperationalStatus::enum_Error);
#endif
                job->JobState.value = SF_ConcreteJob::_JobState::enum_Killed;
                break;
        }
#if CIM_SCHEMA_VERSION_MINOR > 0
        job->PercentComplete.set(th->percentage());
        job->DeleteOnCompletion.set(false);
#endif
        job->StartTime.set(th->startTime());

        st = th->currentState();
        if (st == Thread::Running || st == Thread::Aborting)
            job->ElapsedTime.set(Datetime(Datetime::now().usec() -
                                          job->StartTime.value.usec()));
        else if (th->finishTime().usec() > job->StartTime.value.usec())
            job->ElapsedTime.set(Datetime(th->finishTime().usec() -
                                          job->StartTime.value.usec()));
        else
            job->ElapsedTime.set(Datetime((uint64)0));

#if CIM_SCHEMA_VERSION_MINOR > 0
        job->ErrorCode.null = false;
        job->ErrorCode.value = 0;
        job->ErrorDescription.null = false;
        job->ErrorDescription.value = String("");
#endif

        return job;
    }

    bool
    ConcreteJobAbstractHelper::match(const SystemElement& se, const Instance& inst, unsigned) const
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

    Instance *
    EnabledLogicalElementCapabilitiesHelper::reference(const SystemElement& el, unsigned) const
    {
        SF_EnabledLogicalElementCapabilities* caps = SF_EnabledLogicalElementCapabilities::create(true);

        caps->InstanceID.set(instanceID(el.name()));
        caps->InstanceID.value.append(" ");
        caps->InstanceID.value.append(suffix);
        caps->InstanceID.value.append(" Capabilities");
        return caps;
    }

    Instance *
    EnabledLogicalElementCapabilitiesHelper::instance(const SystemElement& el, unsigned idx) const
    {
        SF_EnabledLogicalElementCapabilities* caps =
        static_cast<SF_EnabledLogicalElementCapabilities*>(reference(el, idx));

        caps->ElementNameEditSupported.set(false);
        caps->RequestedStatesSupported.null = false;
        if (manageable)
        {
            static const cimple::uint16 states[] = {
                SF_EnabledLogicalElementCapabilities::_RequestedStatesSupported::enum_Disabled,
                SF_EnabledLogicalElementCapabilities::_RequestedStatesSupported::enum_Enabled,
                SF_EnabledLogicalElementCapabilities::_RequestedStatesSupported::enum_Reset,
            };
            caps->RequestedStatesSupported.value.append(states, sizeof(states) / sizeof(*states));
        }
        return caps;
    }

    Instance *
    ElementCapabilitiesHelper::instance(const SystemElement& el, unsigned idx) const
    {
        SF_ElementCapabilities *link = SF_ElementCapabilities::create(true);

        link->ManagedElement = cast<cimple::CIM_LogicalElement *>(el.cimReference(elementClass, idx));
        link->Capabilities = cast<cimple::CIM_Capabilities *>(el.cimReference(capsClass, idx));
        link->Characteristics.null = false;
        link->Characteristics.value.append(SF_ElementCapabilities::_Characteristics::enum_Default);
        link->Characteristics.value.append(SF_ElementCapabilities::_Characteristics::enum_Current);

        return link;
    }

    const DMTFProfileInfo DMTFProfileInfo::ProfileRegistrationProfile("Profile Registration", "1.0.0", NULL);
    const DMTFProfileInfo *const DMTFProfileInfo::genericPrpRef[] =
    {&ProfileRegistrationProfile, NULL};
    const DMTFProfileInfo DMTFProfileInfo::DiagnosticsProfile("Diagnostics", "2.0.0", genericPrpRef);
    const DMTFProfileInfo DMTFProfileInfo::RecordLogProfile("Record Log", "2.0.0", genericPrpRef);
    const DMTFProfileInfo DMTFProfileInfo::PhysicalAssetProfile("Physical Asset", "1.0.2", genericPrpRef);
    const DMTFProfileInfo *const DMTFProfileInfo::hostLanPortRef[] =
    {&ProfileRegistrationProfile, &PhysicalAssetProfile, NULL};
    const DMTFProfileInfo DMTFProfileInfo::HostLANNetworkPortProfile("Host LAN Network Port", "1.0.2", hostLanPortRef);
    const DMTFProfileInfo *const DMTFProfileInfo::ethernetPortRef[] =
    {&ProfileRegistrationProfile, &HostLANNetworkPortProfile, NULL};
    const DMTFProfileInfo DMTFProfileInfo::EthernetPortProfile("Ethernet Port", "1.0.1", ethernetPortRef);
    const DMTFProfileInfo DMTFProfileInfo::SoftwareInventoryProfile("Software Inventory", "1.0.1", genericPrpRef);
    const DMTFProfileInfo *const DMTFProfileInfo::softwareUpdateRef[] =
    {&ProfileRegistrationProfile, &SoftwareInventoryProfile, NULL};
    const DMTFProfileInfo DMTFProfileInfo::SoftwareUpdateProfile("Software Update", "1.0.0", softwareUpdateRef);
    const DMTFProfileInfo DMTFProfileInfo::JobControlProfile("Job Control", "1.0.0", genericPrpRef);

    const DMTFProfileInfo * const DMTFProfileInfo::knownDMTFProfiles[] =
    {
        &ProfileRegistrationProfile,
        &DiagnosticsProfile,
        &RecordLogProfile,
        &PhysicalAssetProfile,
        &EthernetPortProfile,
        &SoftwareInventoryProfile,
        &SoftwareUpdateProfile,
        &HostLANNetworkPortProfile,
#if !TARGET_CIM_SERVER_esxi
        // Disabled on ESXi due to an error in CIM PAT Provides
        // Tags Validation test.
        &JobControlProfile,
#endif
        NULL
    };

    String DMTFProfileInfo::profileId() const
    {
        Buffer buf;
        buf.appends(System::target.prefix().c_str());
        buf.append(':');
        buf.appends("DMTF+");
        buf.appends(name);
        buf.append('+');
        buf.appends(version);
        return buf.data();
    }

    SF_RegisteredProfile *DMTFProfileInfo::reference() const
    {
        SF_RegisteredProfile *newProf = SF_RegisteredProfile::create(true);

        newProf->InstanceID.set(profileId());
        return newProf;
    }

    SF_ElementConformsToProfile *DMTFProfileInfo::conformingElement(const Instance *obj) const
    {
        SF_ElementConformsToProfile *link = SF_ElementConformsToProfile::create(true);

        link->ConformantStandard = cast<cimple::CIM_RegisteredProfile *>(reference());
        link->ConformantStandard->__name_space = CIMHelper::interopNS;
        link->ManagedElement = cast<cimple::CIM_ManagedSystemElement *>(obj);
        link->ManagedElement->__name_space = CIMHelper::solarflareNS;

        return link;
    }

} // namespace
