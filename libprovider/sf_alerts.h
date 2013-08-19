#ifndef SOLARFLARE_SF_ALERTS_H
#define SOLARFLARE_SF_ALERTS_H 1

#include "SF_Alert.h"
#include "SF_EthernetPort.h"
#include "sf_sensors.h"
#include "sf_provider.h"

namespace solarflare
{
    using cimple::Instance;
    using cimple::uint16;
    using cimple::Array;
    using cimple::String;
    using cimple::SF_EthernetPort;
    using cimple::Datetime;

    ///
    /// Properties of SF_Alert filled by platform-specific code.
    ///
    class AlertProps {
    public:
        uint16 alertType;           ///< Type of alert
        uint16 perceivedSeverity;   ///< Perceived severity
        String description;         ///< Description of alert indication
        String localId;             ///< Our ID of alert indication

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
        /// @param alerts       Descriptions of alert to be generated
        ///
        /// @return true is some alerts were detected, false otherwise
        virtual bool check(Array<AlertProps> &alerts) = 0;

        String instPath;              ///< Path to instance for which
                                      ///  alerts are checked
        String sysName;               ///< System name
        String sysCreationClassName;  ///< System creation class name

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

        virtual bool check(Array<AlertProps> &alerts);
    };

    /// Abstract class for sensors alert
    class SensorsAlertInfo : public AlertInfo {
    protected:
        Array<Sensor> sensorsPrev;            ///< Previous states of
                                              ///  sensors
        Array<Sensor> sensorsCur;             ///< Current states of
                                              ///  sensors
        bool          sensorsStateFirstTime;  ///< Whether sensors were
                                              ///  not checket before or
                                              ///  not

        int portFn;                           ///< PCI function in PCI
                                              ///  address of this port

        /// Update sensors states
        virtual int updateSensors() = 0;

    public:
        SensorsAlertInfo() : sensorsStateFirstTime(true),
                             portFn(0) {};
        virtual ~SensorsAlertInfo() {};

        virtual bool check(Array<AlertProps> &alerts);
    };

    ///
    /// Type of function used to register all the Ethernet port alerts
    /// to be checked by adding corresponding AlertInfo instances in
    /// the array.
    ///
    typedef int (*FillPortAlertsInfoFunc)(Array<AlertInfo *> &info,
                                          const Port *port);

    ///
    /// Handler type to be used in provider classes instances enumeration
    ///
    typedef void (*constProcessProvClsInst)(const SystemElement& obj,
                                            void *data);

    ///
    /// Enumerator for provider classes instances enumeration
    /// (i.e. classes like Port - descendants of SystemElement)
    ///
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

        ///
        /// For each given Port instance, obtain alert
        /// description instances.
        ///
        /// @param obj    Reference to SF_EthernetPort instance
        /// @param data   Pointer to CIMAlertNotify instance for which
        ///               list of alert descriptions is filled
        ///
        static inline void collectPortAlerts(const SystemElement& obj,
                                             void *data)
        {
            Array<AlertInfo *> alerts;
            String             portPath;
            String             portSysName;
            String             portSysCreationClassName;
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
            if (!port->SystemName.null)
                portSysName = port->SystemName.value;
            if (!port->SystemCreationClassName.null)
                portSysCreationClassName =
                          port->SystemCreationClassName.value;
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
                alerts[i]->sysName = portSysName;
                alerts[i]->sysCreationClassName = portSysCreationClassName;
                owner->instancesInfo.append(alerts[i]);
            }

            return;
        }

    protected:
        ///
        /// Create an indication object according to given properties
        /// 
        /// @param alertType            Type of alert indication
        /// @param perceivedSeverity    Perceived severity
        /// @param description          Description of alert indication
        /// @param instPath             Path to port instance to which
        ///                             generated alert indication is
        ///                             related
        /// @param sysName              Name of the scoping system of
        ///                             AlertingManagedElement
        /// @param sysCreationClassName The scoping system's
        ///                             CreationClassName
        ///
        /// @return Indication object pointer
        ///
        virtual CIMClass *makeIndication(uint16 alertType,
                                         uint16 perceivedSeverity,
                                         String description,
                                         String localId,
                                         String instPath,
                                         String sysName,
                                         String sysCreationClassName)
        {
            CIMClass *indication = CIMClass::create(true);
            Instance *instance = NULL;
            String    indicationId("Solarflare:");

            indicationId.append(localId);
#if !defined(TARGET_CIM_SERVER_wmi)
            indication->IndicationIdentifier.null = false;
            indication->IndicationIdentifier.value = indicationId;
            indication->IndicationTime.null = false;
            indication->IndicationTime.value = Datetime::now();
#endif
            indication->EventID.null = false;
            indication->EventID.value = indicationId;
            indication->ProviderName.null = false;
            indication->ProviderName.value = String("solarflare");
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
            indication->ProbableCause.null = false;
            indication->ProbableCause.value =
                  cimple::CIM_AlertIndication::_ProbableCause::enum_Unknown;
            indication->SystemName.null = false;
            indication->SystemName.value = sysName;
            indication->SystemCreationClassName.null = false;
            indication->SystemCreationClassName.value =
                                              sysCreationClassName;

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
                    Array<AlertProps> alertsProps;
                    if (owner->handler != NULL &&
                        owner->instancesInfo[i]->check(alertsProps))
                    {
                        for (j = 0; j < alertsProps.size(); j++)
                            owner->handler->handle(
                              owner->makeIndication(
                                alertsProps[j].alertType,
                                alertsProps[j].perceivedSeverity,
                                alertsProps[j].description,
                                alertsProps[j].localId,
                                owner->instancesInfo[i]->instPath,
                                owner->instancesInfo[i]->sysName,
                                owner->instancesInfo[i]->
                                            sysCreationClassName));
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
}

#endif // SOLARFLARE_SF_ALERTS_H
