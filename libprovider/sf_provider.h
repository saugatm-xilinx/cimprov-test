#ifndef SOLARFLARE_SF_PROVIDER_H
#define SOLARFLARE_SF_PROVIDER_H 1

#include "sf_platform.h"
#include "CIM_ComputerSystem.h"
#include "SF_RegisteredProfile.h"
#include "SF_ReferencedProfile.h"
#include "SF_ElementConformsToProfile.h"
#include "CIM_InstIndication.h"
#include "CIM_InstModification.h"
#include "SF_ConcreteJob.h"
#include "SF_JobCreated.h"
#include "SF_JobStarted.h"
#include "SF_JobError.h"
#include "SF_JobSuccess.h"
#include "SF_Alert.h"

#if CIM_SCHEMA_VERSION_MINOR == 0
namespace cimple
{
    typedef CIM_ManagedSystemElement CIM_ManagedElement;
};
#endif


/// This file contains some helpers and utilities
/// for writing CIMPLE providers. None of the code is platform-dependent

namespace solarflare
{
    using cimple::Instance;

    /// @brief Abstract class for ties between CIM classes and #SystemElement.
    /// This class is subclassed internally for descendants of #SystemElement.
    /// @sa SystemElement::cimDispatch()
    class CIMHelper {
        static cimple::Ref<cimple::CIM_ComputerSystem> cimSystem;
    public:
        virtual ~CIMHelper() {}
        virtual unsigned nObjects(const SystemElement&) const { return 1; }
        /// @return A CIM instance matching @p obj
        virtual cimple::Instance *instance(const SystemElement&,
                                           unsigned idx = 0) const = 0;
        /// @return A CIM reference matching @p obj
        virtual cimple::Instance *reference(const SystemElement& obj,
                                            unsigned idx = 0) const
        {
            return instance(obj, idx);
        }

        /// @return true iff @p obj corresponds to CIM instance @p inst
        virtual bool match(const SystemElement& obj,
                           const cimple::Instance& inst, unsigned idx = 0) const
        {
            return false;
        }

        static const char solarflareNS[];
        static const char ibmseNS[];
        static const char ibmsdNS[];
        static const char interopNS[];
        static const char baseNS[];
        static void initialize()
        {
            System::target.initialize();
        }

        static String instanceID(const String& name);
        static const cimple::CIM_ComputerSystem *findSystem();
        static cimple::CIM_System *systemRef()
        {
            return reinterpret_cast<cimple::CIM_System *>(findSystem()->clone());
        }
        static bool isOurSystem(const String& sysclass, const String& sysname);
        static bool isOurSystem(const cimple::CIM_ComputerSystem* sys)
        {
            return cimple::key_eq(findSystem(), sys);
        }
    };

    
    template <class CIMClass>
    class CIMNotify {
        cimple::Indication_Handler<CIMClass> *handler;

    protected:
        virtual void send(CIMClass *obj)
        {
            using namespace cimple;
            if (handler != NULL)
                handler->handle(obj);
            else
                CIMClass::destroy(obj);
        }
        CIMNotify() : handler(NULL) {}
        virtual ~CIMNotify()
        {
            delete handler;
        }
    public:
        void enable(cimple::Indication_Handler<CIMClass> *hnd)
        {
            delete handler;
            handler = hnd;
        }
        void disable()
        {
            enable(NULL);
        }
    };

    template <class CIMClass>
    class CIMInstanceNotify : public CIMNotify<CIMClass> {
        const cimple::Meta_Class& sourceMC;
    protected:
        virtual CIMClass *makeIndication(const SystemElement& se)
        {
            const CIMHelper *helper = se.cimDispatch(sourceMC);
            CIMClass *indication = CIMClass::create(true);

            if (helper == NULL)
                return NULL;
            
            indication->SourceInstance = helper->instance(se, 0);
            indication->SourceInstanceModelPath.null = false;
            cimple::instance_to_model_path(indication->SourceInstance, 
                                           indication->SourceInstanceModelPath.value);
            return indication;
        }
    public:
        CIMInstanceNotify(const cimple::Meta_Class& mc) :
            sourceMC(mc) {}
        ~CIMInstanceNotify() {}
        
        void notify(const SystemElement& se)
        {
            send(makeIndication(se));
        }
    };

    template <class CIMClass>
    class CIMJobChangeStateNotify : public CIMInstanceNotify<CIMClass> {
        unsigned prevState;
    protected:
        virtual CIMClass *makeIndication(const SystemElement& se)
        {
            CIMClass *indication = CIMInstanceNotify<CIMClass>::makeIndication(se);
            indication->PreviousInstance = cimple::clone(indication->SourceInstance);
            cimple::CIM_ConcreteJob *pi = cimple::cast<cimple::CIM_ConcreteJob *>(indication->PreviousInstance);
#if CIM_SCHEMA_VERSION_MINOR > 0
            pi->OperationalStatus.null = false;
            pi->OperationalStatus.value.append(prevState);
#endif
            return indication;
        }
    public:
        CIMJobChangeStateNotify(unsigned ps) :
            CIMInstanceNotify<CIMClass>(cimple::SF_ConcreteJob::static_meta_class),
            prevState(ps) {}
        
    };

    extern CIMInstanceNotify<cimple::SF_JobCreated> onJobCreated;
    extern CIMJobChangeStateNotify<cimple::SF_JobStarted> onJobStarted;
    extern CIMJobChangeStateNotify<cimple::SF_JobError> onJobError;
    extern CIMJobChangeStateNotify<cimple::SF_JobSuccess> onJobSuccess;

    template <class CIMClass>
    class CIMAlertNotify : public CIMNotify<CIMClass> {
        const cimple::Meta_Class& alertMC;
    public:
        CIMAlertNotify(const cimple::Meta_Class amc) : alertMC(amc) {}
#if 0
        virtual void alert(const SystemElement& se)
        {
            const CIMHelper *helper = se.cimDispatch(sourceMC);
            CIMClass *indication = CIMClass::create(true);

            if (helper == NULL)
                return;
            
            indication->SourceInstance = helper->instance(se, 0);
            indication->SourceInstanceModelPath.null = false;
            cimple::instance_to_model_path(indication->SourceInstance, 
                                           indication->SourceInstanceModelPath.value);
            return indication;
        }
#endif
    };

    extern CIMAlertNotify<cimple::SF_Alert> onAlert;

    template <class CIMClass>
    class EnumInstances : public ConstElementEnumerator {
        cimple::Enum_Instances_Handler<CIMClass> *handler;
    public:
        EnumInstances(cimple::Enum_Instances_Handler<CIMClass> *h) :
            handler(h) {}
        virtual bool process(const SystemElement& obj)
        {
            const CIMHelper *helper = obj.cimDispatch(CIMClass::static_meta_class);
            if (helper == NULL)
                return true;

            unsigned n = helper->nObjects(obj);

            for (unsigned i = 0; i < n; i++)
            {
                Instance *cimobj = helper->instance(obj, i);
                if (cimobj != NULL)
                    handler->handle(static_cast<CIMClass *>(cimobj));
            }
            return true;
        }
        static void allSoftware(cimple::Enum_Instances_Handler<CIMClass> *handler)
        {
            EnumInstances<CIMClass> iter(handler);
            System::target.forAllSoftware(iter);
        }
        static void allNICs(cimple::Enum_Instances_Handler<CIMClass> *handler)
        {
            EnumInstances<CIMClass> iter(handler);
            System::target.forAllNICs(iter);
        }
        static void allInterfaces(cimple::Enum_Instances_Handler<CIMClass> *handler)
        {
            EnumInstances<CIMClass> iter(handler);
            System::target.forAllInterfaces(iter);
        }
        static void allPorts(cimple::Enum_Instances_Handler<CIMClass> *handler)
        {
            EnumInstances<CIMClass> iter(handler);
            System::target.forAllPorts(iter);
        }
        static void allDiagnostics(cimple::Enum_Instances_Handler<CIMClass> *handler)
        {
            EnumInstances<CIMClass> iter(handler);
            System::target.forAllDiagnostics(iter);
        }
        static void topmost(cimple::Enum_Instances_Handler<CIMClass> *handler)
        {
            EnumInstances<CIMClass> iter(handler);
            iter.process(System::target);
        }
        static void allObjects(cimple::Enum_Instances_Handler<CIMClass> *handler)
        {
            EnumInstances<CIMClass> iter(handler);
            iter.process(System::target);
            System::target.forAllSoftware(iter);
            System::target.forAllNICs(iter);
            System::target.forAllInterfaces(iter);
            System::target.forAllPorts(iter);
            System::target.forAllDiagnostics(iter);
        }
    };

    class Action : public ElementEnumerator
    {
        const Instance *sample;
    protected:
        virtual void handler(SystemElement&, unsigned) = 0;
    public:
        Action(const Instance *s) :
            sample(s) {}
        virtual bool process(SystemElement& el);
        bool forSoftware(SoftwareContainer& scope = System::target)
        {
            return !scope.forAllSoftware(*this);
        }
        bool forNIC()
        {
            return !System::target.forAllNICs(*this);
        }
        bool forInterface()
        {
            return !System::target.forAllInterfaces(*this);
        }
        bool forPort()
        {
            return !System::target.forAllPorts(*this);
        }
        bool forDiagnostic()
        {
            return !System::target.forAllDiagnostics(*this);
        }
        bool forThread();
        bool forAny();
        bool forSystem()
        {
            return !process(System::target);
        }
    };

    class AsyncRunner : public Action {
    protected:
        virtual void handler(SystemElement& se, unsigned);
    public:
        AsyncRunner(const cimple::Instance *inst) : Action(inst) {}
    };

    class ObjectCount : public ConstElementEnumerator
    {
        unsigned cnt;
    public:
        ObjectCount() : cnt(0) {};
        virtual bool process(const SystemElement&) { cnt++; return true; }
        unsigned count() const { return cnt; }
    };

    class ConcreteJobAbstractHelper : public CIMHelper {
    protected:
        virtual const char* threadSuffix() const = 0;
    public:
        virtual cimple::Instance *reference(const SystemElement& obj, unsigned idx) const;
        virtual cimple::Instance *instance(const SystemElement&, unsigned idx) const;
        virtual bool match(const SystemElement& obj, const cimple::Instance& inst, unsigned idx) const;
    };


    class EnabledLogicalElementCapabilitiesHelper : public CIMHelper {
        const char *suffix;
        bool manageable;
    public:
        EnabledLogicalElementCapabilitiesHelper(const char *suf, bool man) :
            suffix(suf), manageable(man) {}
        virtual cimple::Instance *reference(const SystemElement& obj, unsigned idx) const;
        virtual cimple::Instance *instance(const SystemElement&, unsigned idx) const;
    };

    class ElementCapabilitiesHelper : public CIMHelper {
        const cimple::Meta_Class& elementClass;
        const cimple::Meta_Class& capsClass;
    public:
        ElementCapabilitiesHelper(const cimple::Meta_Class& ec,
                                  const cimple::Meta_Class& cc) :
            elementClass(ec), capsClass(cc) {}
        virtual cimple::Instance *instance(const SystemElement&, unsigned idx) const;
    };

    class DMTFProfileInfo {
        const char *name;
        const char *version;
        const DMTFProfileInfo * const *referenced;
        static const DMTFProfileInfo *const genericPrpRef[];
        static const DMTFProfileInfo *const softwareUpdateRef[];
        static const DMTFProfileInfo *const ethernetPortRef[];
        static const DMTFProfileInfo *const hostLanPortRef[];
    public:
        DMTFProfileInfo(const char *n, const char *v,
                        const DMTFProfileInfo * const deps[]) :
            name(n), version(v), referenced(deps) {}

        static const DMTFProfileInfo ProfileRegistrationProfile;
        static const DMTFProfileInfo DiagnosticsProfile;
        static const DMTFProfileInfo RecordLogProfile;
        static const DMTFProfileInfo PhysicalAssetProfile;
        static const DMTFProfileInfo EthernetPortProfile;
        static const DMTFProfileInfo SoftwareInventoryProfile;
        static const DMTFProfileInfo SoftwareUpdateProfile;
        static const DMTFProfileInfo HostLANNetworkPortProfile;
        static const DMTFProfileInfo JobControlProfile;
        static const DMTFProfileInfo * const knownDMTFProfiles[];
        String profileId() const;
        cimple::SF_RegisteredProfile *reference() const;
        const DMTFProfileInfo * const *referencedProfiles() const
        {
            return referenced;
        }
        const char *profileName() const { return name; }
        const char *profileVersion() const { return version; }

        cimple::SF_ElementConformsToProfile *conformingElement(const Instance *obj) const;
    };


} // namespace

#endif // SOLARFLARE_SF_PROVIDER_H
