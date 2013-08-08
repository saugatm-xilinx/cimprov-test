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
#include "SF_JobProgress.h"
#include "SF_JobError.h"
#include "SF_JobSuccess.h"
#include "SF_Alert.h"
#include "SF_EthernetPort.h"

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
    using cimple::uint16;
    using cimple::Array;
    using cimple::String;
    using cimple::SF_EthernetPort;

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
        cimple::Atomic_Counter thread_send;
        cimple::Thread provider_thread;
        cimple::Mutex send_lock;
        cimple::Mutex thread_lock;
        bool thread_not_joined;
        CIMClass *obj_to_send;

    protected:
        cimple::Indication_Handler<CIMClass> *handler;
        cimple::Atomic_Counter thread_continue;
        cimple::Thread_Proc threadProc;

        virtual void send(CIMClass *obj)
        {
            using namespace cimple;

            Auto_Mutex auto_mutex(send_lock);

            if (handler != NULL && thread_continue.get() > 0)
            {
                obj_to_send = obj;
                thread_send.inc();

                while (thread_send.get() > 0 && thread_continue.get() > 0)
                    Time::sleep(10 * Time::MSEC);

                if (thread_send.get() > 0)
                {
                    thread_send.dec();
                    CIMClass::destroy(obj);
                }
            }
            else
                CIMClass::destroy(obj);
        }

        CIMNotify() : send_lock(false),
                      thread_lock(false), thread_not_joined(false),
                      handler(NULL), 
                      threadProc(threadFunc) {}

        virtual ~CIMNotify()
        {
            cimple::Auto_Mutex auto_mutex1(thread_lock);
            cimple::Auto_Mutex auto_mutex2(send_lock);

            if (thread_continue.get() || thread_not_joined)
            {
                void *value;

                if (thread_continue.get() > 0)
                    thread_continue.dec();
                Thread::join(provider_thread, value);
                thread_not_joined = false;
            }

            delete handler;
            handler = NULL;
        }

        static void *threadFunc(void *arg)
        {
            using namespace cimple;

            CIMNotify<CIMClass> *owner = (CIMNotify<CIMClass> *)arg;

            while (1)
            {
                if (!owner->thread_continue.get())
                    break;

                if (owner->thread_send.get())
                {
                    if (owner->handler != NULL)
                        owner->handler->handle(owner->obj_to_send);
                    else
                        CIMClass::destroy(owner->obj_to_send);
                    owner->thread_send.dec();
                }

                Time::sleep(10 * Time::MSEC);
            }

            return NULL;
        }

    public:
        virtual void enable(cimple::Indication_Handler<CIMClass> *hnd)
        {
            using namespace cimple;

            Auto_Mutex auto_mutex1(thread_lock);
            Auto_Mutex auto_mutex2(send_lock);

            if (thread_not_joined)
            {
                void *value;

                Thread::join(provider_thread, value);
                thread_not_joined = false;
            }

            delete handler;
            handler = hnd;

            if (hnd != NULL && !thread_continue.get())
            {
                thread_continue.inc();
                Thread::create_joinable(provider_thread,
                                        (Thread_Proc)threadProc,
                                        this);
            }
        }
        virtual void disable()
        {
            cimple::Auto_Mutex auto_mutex(thread_lock);

            if (thread_continue.get())
            {
                // With CMPI, after CIMPLE attempts to disable indications,
                // thread calling an indication handler just after it hangs
                // because this handler tries to call _indication_proc from
                // cimple/cmpi/CMPI_Adapter.cpp which uses the same mutex as
                // disableIndications() from the same file. To prevent
                // deadlock, we should not wait for anything here but
                // return immediately, so that disableIndications() will
                // unlock the mutex and _indication_proc will be able to
                // proceed unlocking our thread and send().
                // The same is true for pegasus provider interface.
                // 
                // Neither should we delete handler to avoid possible
                // segmentation fault in the thread.
                //
                // So the only thing we can do here is to ask thread to
                // terminate after unblocking and remember to join it later.

                thread_continue.dec();
                thread_not_joined = true;
            }
            else
            {
                delete handler;
                handler = NULL;
            }
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
            CIMNotify<CIMClass>(), sourceMC(mc) {}
        ~CIMInstanceNotify() {}
        
        void notify(const SystemElement& se)
        {
            this->send(makeIndication(se));
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
    extern CIMJobChangeStateNotify<cimple::SF_JobProgress> onJobProgress;
    extern CIMJobChangeStateNotify<cimple::SF_JobError> onJobError;
    extern CIMJobChangeStateNotify<cimple::SF_JobSuccess> onJobSuccess;

    ///
    /// Properties of SF_Alert filled by platform-specific code.
    ///
    class AlertProps {
    public:
        uint16 alertType;           ///< Type of alert
        uint16 perceivedSeverity;   ///< Perceived severity
        String description;         ///< Description of alert indication

        /// Comparison operator is required by cimple::Array
        inline bool operator== (const AlertProps &rhs)
        {
            if (alertType == rhs.alertType &&
                perceivedSeverity == rhs.perceivedSeverity &&
                description == rhs.description)
                return true;

            return false;
        }
    };

    ///
    /// An abstract class containing a platform specific function
    /// to check for alert(s), arguments to be passed to it and
    /// auxiliary data as well. It should be subclassed in platform
    /// specific implementation.
    ///
    class AlertInfo {
    public:
        AlertInfo() {};
        virtual ~AlertInfo() {}

        /// Function to check for alerts
        ///
        /// @param alert        Descriptions of alert to be generated
        ///
        /// @return true is some alerts were detected, false otherwise
        virtual bool check(AlertProps &alert) = 0;

        String instPath; ///< Path to instance for which alerts are checked

        /// Comparison operator is required by cimple::Array
        inline bool operator== (const AlertInfo &rhs)
        {
            if (instPath == rhs.instPath)
                return true;

            return false;
        }
    };

    /// Abstract class for link state alert
    class LinkStateAlertInfo : public AlertInfo {
    protected:
        bool prevLinkState;         ///< Previously determined link state
        bool curLinkState;          ///< Current link state
        bool linkStateFirstTime;    ///< Whether link state is checked
                                    ///  first time (in this case
                                    ///  prevLinkState is not defined yet)

        ///
        /// Function used to obtain current link state
        ///
        /// @return 0 on success, error code otherwise
        ///
        virtual int updateLinkState() = 0;
    public:
        LinkStateAlertInfo() : prevLinkState(false), curLinkState(false),
                               linkStateFirstTime(true) {};
        virtual ~LinkStateAlertInfo() {};

        virtual bool check(AlertProps &alert)
        {
            bool         result = false;

            if (updateLinkState() != 0)
                return false;

            if (!curLinkState && prevLinkState && !linkStateFirstTime)
            {
                alert.alertType = cimple::CIM_AlertIndication::
                                         _AlertType::enum_Device_Alert;
                alert.perceivedSeverity = cimple::CIM_AlertIndication::
                                         _PerceivedSeverity::enum_Major;
                alert.description = "Link went down";

                result = true;
            }
            linkStateFirstTime = false;
            prevLinkState = curLinkState;
            return result;
        }
    };

    ///
    /// Type of function used to register all the Ethernet port alerts
    /// to be checked by adding corresponding AlertInfo instances in
    /// the array.
    ///
    typedef int (*FillPortAlertsInfoFunc)(Array<AlertInfo *> &info,
                                          const Port *port);

    typedef void (*constProcessProvClsInst)(const SystemElement& obj,
                                            void *data);

    class ConstEnumProvClsInsts : public ConstElementEnumerator {
        constProcessProvClsInst    handler;
        void                      *handlerData;
    public:
        ConstEnumProvClsInsts(constProcessProvClsInst f,
                              void *data) :
            handler(f), handlerData(data) {};
 
        virtual bool process(const SystemElement& obj)
        {
            handler(obj, handlerData);
            return true;
        }
    };

    ///
    /// Class template for provider specific alert indications.
    ///
    template <class CIMClass>
    class CIMAlertNotify : public CIMNotify<CIMClass> {

        Array<AlertInfo *> instancesInfo;       ///< Registered alerts to be
                                                ///  checked and reported

        FillPortAlertsInfoFunc
                           fillPortAlertsInfo;  ///< Pointer to the platform
                                                ///  specific function used
                                                ///  to fill instancesInfo

        static inline void collectPortAlerts(const SystemElement& obj,
                                             void *data)
        {
            Array<AlertInfo *> alerts;
            String             portPath;
            SF_EthernetPort   *port;
            unsigned int       i;

            CIMAlertNotify *owner =
                        reinterpret_cast<CIMAlertNotify *>(data);

            const CIMHelper *helper =
                  obj.cimDispatch(SF_EthernetPort::static_meta_class);

            if (owner->fillPortAlertsInfo == NULL)
                return;

            if (helper == NULL)
            {
                CIMPLE_ERR(("%s(): failed to find helper "
                            "for SF_EthernetPort", __FUNCTION__));
                return;
            }

            port = static_cast<SF_EthernetPort *>(helper->instance(obj, 0));
            if (port == NULL)
            {
                CIMPLE_ERR(("%s(): failed to find SF_EthernetPort instance",
                            __FUNCTION__));
                return;
            }

            if (cimple::instance_to_model_path(port, portPath) != 0)
            {
                CIMPLE_ERR(("%s(): failed to determine path to "
                            "SF_EthernetPort instance",
                            __FUNCTION__));
                SF_EthernetPort::destroy(port);
                return;
            }
            SF_EthernetPort::destroy(port);

            if (owner->fillPortAlertsInfo(
                        alerts,
                        // For some strange reason just using reference
                        // directly breaks the build on VMware Workbench,
                        // so I use pointer here.
                        dynamic_cast<const Interface *>(&obj)->port()) != 0)
                return;

            for (i = 0; i < alerts.size(); i++)
            {
                alerts[i]->instPath = portPath;
                owner->instancesInfo.append(alerts[i]);
            }

            return;
        }

    protected:
        ///
        /// Create an indication object according to given properties
        /// 
        /// @return Indication object pointer
        ///
        virtual CIMClass *makeIndication(uint16 alertType,
                                         uint16 perceivedSeverity,
                                         String description,
                                         String instPath)
        {
            CIMClass *indication = CIMClass::create(true);
            Instance *instance = NULL;

            indication->Description.null = false;
            indication->Description.value = description;
            indication->AlertingManagedElement.null = false;
            indication->AlertingManagedElement.value = instPath;
            indication->AlertingElementFormat.null = false;
            indication->AlertingElementFormat.value =
                    cimple::CIM_AlertIndication::_AlertingElementFormat::
                                                        enum_CIMObjectPath;
            indication->AlertType.null = false;
            indication->AlertType.value = alertType;
            indication->PerceivedSeverity.null = false;
            indication->PerceivedSeverity.value = perceivedSeverity;

            return indication;
        }

        ///
        /// Main function for thread used to check for
        /// alerts and report them.
        ///
        /// @param arg  Pointer to alert indication class instance
        ///             containing information about alerts to
        ///             be checked
        ///
        /// @return NULL
        ///
        static void *alertThreadFunc(void *arg)
        {
            using namespace cimple;

            unsigned int i;
            unsigned int j;
            uint16       alertType;
            uint16       perceivedSeverity;
            String       description;

            CIMAlertNotify<CIMClass> *owner =
                                (CIMAlertNotify<CIMClass> *)arg;
            while (1)
            {
                if (!owner->thread_continue.get())
                    break;

                for (i = 0; i < owner->instancesInfo.size(); i++)
                {
                    AlertProps alertProps;
                    if (owner->handler != NULL &&
                        owner->instancesInfo[i]->check(alertProps))
                    {
                        owner->handler->handle(
                            owner->makeIndication(
                                    alertProps.alertType,
                                    alertProps.perceivedSeverity,
                                    alertProps.description,
                                    owner->instancesInfo[i]->instPath));
                    }
                }

                Time::sleep(500 * Time::MSEC);
            }

            return NULL;
        }

    public:
        CIMAlertNotify() :
          CIMNotify<CIMClass>(),
          fillPortAlertsInfo(NULL)
        {
            this->threadProc = alertThreadFunc;
        }

        /// Clear array of alerts to be checked
        void instsInfoClear()
        {
            unsigned int i;

            for (i = 0; i < instancesInfo.size(); i++)
            {
                delete instancesInfo[i];
                instancesInfo[i] = NULL;
            }
            instancesInfo.clear();
        }

        /// Enable alerts checking and reporting
        virtual void enable(cimple::Indication_Handler<CIMClass> *hnd)
        {
            ConstEnumProvClsInsts en(collectPortAlerts,
                                     this);

            if (fillPortAlertsInfo == NULL)
                return;
            instsInfoClear();

            System::target.forAllInterfaces(en);
            
            CIMNotify<CIMClass>::enable(hnd);
        }

        /// Disable alerts checking and reporting
        virtual void disable()
        {
            CIMNotify<CIMClass>::disable();
            instsInfoClear();
        }

        /// Set function to obtain information about alerts to
        /// be checked on current platform
        virtual void setFillPortAlertsInfo(FillPortAlertsInfoFunc f)
        {
            fillPortAlertsInfo = f;
        }
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
