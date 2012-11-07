#include "sf_provider.h"
#include "CIM_ComputerSystem.h"
#include "SF_ConcreteJob.h"
#include "SF_EnabledLogicalElementCapabilities.h"
#include "SF_RegisteredProfile.h"
#include "SF_ReferencedProfile.h"

namespace solarflare 
{
    using cimple::Instance;
    using cimple::Meta_Class;
    using cimple::CIM_ComputerSystem;
    using cimple::Ref;
    using cimple::cast;
    using cimple::SF_ConcreteJob;
    using cimple::SF_EnabledLogicalElementCapabilities;
    using cimple::SF_RegisteredProfile;
    using cimple::SF_ReferencedProfile;
    using cimple::SF_ElementConformsToProfile;

    cimple::Instance *SystemElement::cimReference(const cimple::Meta_Class& cls, unsigned idx) const
    {
        const CIMHelper *helper = cimDispatch(cls);
        return helper ? helper->reference(*this, idx) : NULL;
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

    bool Lookup::process(SystemElement& se)
    {
        const CIMHelper *helper = se.cimDispatch(*sample->meta_class);
        if (helper == NULL)
            return true;
        
        unsigned n = helper->nObjects(se);
            
        for (unsigned i = 0; i < n; i++)
        {
            if (helper->match(se, *sample, i))
            {
                obj = &se;
                idx = i;
                return false;
            }
        }
        return true;
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
        if (!finder.process(System::target))
            return finder.found();
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
        &JobControlProfile,
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
        link->ManagedElement = cast<cimple::CIM_ManagedElement *>(obj);
        link->ManagedElement->__name_space = CIMHelper::solarflareNS;

        return link;
    }
    
} // namespace
