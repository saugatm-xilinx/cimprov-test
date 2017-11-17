/***************************************************************************//*! \file liprovider/sf_intf_helpers.cpp
** <L5_PRIVATE L5_SOURCE>
** \author  OktetLabs
**  \brief  CIM Provider
**   \date  2013/10/02
**    \cop  (c) Solarflare Communications Inc.
** </L5_PRIVATE>
*//*
\**************************************************************************/

#include "sf_provider.h"
#include "SF_EthernetPort.h"
#include "SF_LANEndpoint.h"
#include "SF_ConnectorRealizesPort.h"
#include "SF_ConnectorRealizesController.h"
#include "SF_PhysicalConnector.h"
#include "SF_PortController.h"
#include "SF_ControlledBy.h"
#include "SF_EnabledLogicalElementCapabilities.h"
#include "SF_ElementCapabilities.h"
#include "SF_HostedAccessPoint.h"
#include "SF_NICSAPImplementation.h"
#include "SF_ElementConformsToProfile.h"
#include "SF_SystemDevice.h"

namespace solarflare 
{
    using cimple::Instance;
    using cimple::Meta_Class;
    using cimple::CIM_ComputerSystem;
    using cimple::Ref;
    using cimple::cast;
    using cimple::SF_EthernetPort;
    using cimple::SF_LANEndpoint;
    using cimple::SF_ConnectorRealizesPort;
    using cimple::SF_ConnectorRealizesController;
    using cimple::SF_PhysicalConnector;
    using cimple::SF_PortController;
    using cimple::SF_ControlledBy;
    using cimple::SF_EnabledLogicalElementCapabilities;
    using cimple::SF_ElementCapabilities;
    using cimple::SF_ElementConformsToProfile;
    using cimple::SF_HostedAccessPoint;
    using cimple::SF_NICSAPImplementation;
    using cimple::SF_SystemDevice;

    class EthernetPortHelper : public CIMHelper {
    public:
        virtual Instance *reference(const SystemElement& obj, unsigned) const;
        virtual Instance *instance(const SystemElement&, unsigned) const;
        virtual bool match(const SystemElement& obj, const Instance& inst, unsigned) const;
    };

    class LANEndpointHelper : public CIMHelper {
    public:
        virtual Instance *reference(const SystemElement& obj, unsigned) const;
        virtual Instance *instance(const SystemElement&, unsigned) const;
        virtual bool match(const SystemElement& obj, const Instance& inst, unsigned) const;
    };

    class ConnectorRealizesPortHelper : public CIMHelper {
    public:
        virtual Instance *instance(const SystemElement&, unsigned) const;
    };

    class ConnectorRealizesControllerHelper : public CIMHelper {
    public:
        virtual Instance *instance(const SystemElement&, unsigned) const;
    };

    class ControlledByHelper : public CIMHelper {
    public:
        virtual Instance *instance(const SystemElement&, unsigned) const;
    };

    class PortConformsToProfile : public CIMHelper {
        // Host LAN Port + Ethernet Port
        virtual unsigned nObjects(const SystemElement&) const { return 2; }
        virtual Instance *instance(const SystemElement &se, unsigned) const;
    };

    class PortAndEndpointCapabilitiesHelper : public CIMHelper {
        EnabledLogicalElementCapabilitiesHelper portCaps;
        EnabledLogicalElementCapabilitiesHelper endpointCaps;
    public:
        PortAndEndpointCapabilitiesHelper() :
            portCaps("Port", true), endpointCaps("Endpoint", false) {}
        virtual unsigned nObjects(const SystemElement&) const { return 2; }
        virtual Instance *reference(const SystemElement& obj, unsigned) const;
        virtual Instance *instance(const SystemElement& obj, unsigned) const;
    };

    class PortAndEndpointCapsLinkHelper : public CIMHelper {
        ElementCapabilitiesHelper portCaps;
        ElementCapabilitiesHelper endpointCaps;
    public:
        PortAndEndpointCapsLinkHelper() :
            portCaps(SF_EthernetPort::static_meta_class,
                     SF_EnabledLogicalElementCapabilities::static_meta_class), 
            endpointCaps(SF_LANEndpoint::static_meta_class, 
                         SF_EnabledLogicalElementCapabilities::static_meta_class) {}
        virtual unsigned nObjects(const SystemElement&) const { return 2; }
        virtual Instance *instance(const SystemElement& obj, unsigned) const;
    };

    class HostedAccessPointHelper : public CIMHelper {
    public:
        virtual Instance *instance(const SystemElement& obj, unsigned) const;
    };

    class EthernetPortSystemDeviceHelper : public CIMHelper {
    public:
        virtual Instance *instance(const SystemElement& obj, unsigned) const;
    };

    class NICSAPImplementationHelper : public CIMHelper {
    public:
        virtual Instance *instance(const SystemElement& obj, unsigned) const;
    };

    const CIMHelper* Interface::cimDispatch(const Meta_Class& cls) const
    {
        static const EthernetPortHelper ethernetPortHelper;
        static const LANEndpointHelper lanEndpointHelper;
        static const ConnectorRealizesPortHelper connectorRealizesPortHelper;
        static const ConnectorRealizesControllerHelper
                                            connectorRealizesControllerHelper;
        static const ControlledByHelper controlledByHelper;
        static const PortAndEndpointCapabilitiesHelper capabilities;
        static const PortAndEndpointCapsLinkHelper capsLink;
        static const HostedAccessPointHelper hostedAccessPoint;
        static const NICSAPImplementationHelper nicSAPImplementation;
        static const EthernetPortSystemDeviceHelper systemDevice;
        static const PortConformsToProfile conforming;
        
        if (&cls == &SF_EthernetPort::static_meta_class)
            return &ethernetPortHelper;
        if (&cls == &SF_LANEndpoint::static_meta_class)
            return &lanEndpointHelper;
        if (&cls == &SF_ConnectorRealizesPort::static_meta_class)
            return &connectorRealizesPortHelper;
        if (&cls == &SF_ConnectorRealizesController::static_meta_class)
            return &connectorRealizesControllerHelper;
        if (&cls == &SF_ControlledBy::static_meta_class)
            return &controlledByHelper;
        if (&cls == &SF_SystemDevice::static_meta_class)
            return &systemDevice;
        if (&cls == &SF_EnabledLogicalElementCapabilities::static_meta_class)
            return &capabilities;
        if (&cls == &SF_ElementCapabilities::static_meta_class)
            return &capsLink;
        if (&cls == &SF_HostedAccessPoint::static_meta_class)
            return &hostedAccessPoint;
        if (&cls == &SF_NICSAPImplementation::static_meta_class)
            return &nicSAPImplementation;
        if (&cls == &SF_ElementConformsToProfile::static_meta_class)
            return &conforming;
        
        return NULL;
    }

    Instance *EthernetPortHelper::reference(const SystemElement& se, unsigned) const
    {
        const Interface& intf = static_cast<const Interface&>(se);
        const CIM_ComputerSystem *system = findSystem();
        SF_EthernetPort *newPort = SF_EthernetPort::create(true);
        
        newPort->CreationClassName.set("SF_EthernetPort");
        newPort->DeviceID.set(intf.ifName());
        newPort->SystemCreationClassName.set(system->CreationClassName.value);
        newPort->SystemName.set(system->Name.value);
        return newPort;
    }


    Instance *EthernetPortHelper::instance(const solarflare::SystemElement& se, unsigned idx) const 
    {
        const solarflare::Interface& intf = static_cast<const solarflare::Interface&>(se);
    
        SF_EthernetPort *newPort = static_cast<SF_EthernetPort *>(reference(intf, idx));

        bool ifStatus;

        ASSIGN_IGNORE_EXCEPTION(ifStatus, intf.ifStatus(), false);

        newPort->Description.set(intf.description());
#if CIM_SCHEMA_VERSION_MINOR > 0
        newPort->ElementName.set(intf.ifName());
#endif
        newPort->Name.set(intf.name());
#if  CIM_SCHEMA_VERSION_MINOR == 26
        newPort->InstanceID.set(instanceID(intf.name()));
#endif
#if CIM_SCHEMA_VERSION_MINOR > 0
        newPort->EnabledState.null = false;
#if (TARGET_CIM_SERVER_esxi || TARGET_CIM_SERVER_esxi_native)
        newPort->EnabledState.value =
          (ifStatus ? 
           SF_EthernetPort::_EnabledState::enum_Enabled :
           SF_EthernetPort::_EnabledState::enum_Disabled);

#else
        newPort->EnabledState.value = (ifStatus ?
                                       (intf.port()->linkStatus() ? 
                                        SF_EthernetPort::_EnabledState::enum_Enabled : 
                                        SF_EthernetPort::_EnabledState::enum_Enabled_but_Offline) :
                                       SF_EthernetPort::_EnabledState::enum_Disabled);
#endif
        newPort->RequestedState.null = false;
        if (intf.port() != NULL)
            newPort->RequestedState.value =
                              System::getPortReqState(intf.port()->name());
        else
            newPort->RequestedState.value =
                            SF_EthernetPort::_RequestedState::enum_Unknown;
        newPort->StatusInfo.null = false;
        newPort->StatusInfo.value = (ifStatus ?
                                     SF_EthernetPort::_StatusInfo::enum_Enabled :
                                     SF_EthernetPort::_StatusInfo::enum_Disabled);
#endif
        // The following is a hack. The point is that PortType variable is
        // defined in one CIM class (CIM_LogicalPort) but the set of useful
        // values is defined in another class (CIM_EthernetPort). CIMPLE tools
        // are unable to handle this properly, so do not generate the correct
        // enumeration even with -e option
#define quasi_enum_10GBaseT 55
        newPort->PortType.null = false;
        switch (intf.nic()->connector())
        {
            case NIC::RJ45:
                newPort->PortType.value = quasi_enum_10GBaseT;
                break;
            case NIC::SFPPlus:
                newPort->PortType.value = SF_EthernetPort::_PortType::enum_Other;
                newPort->OtherNetworkPortType.set("10GBase-CR (SFP+)");
                break;
            case NIC::Mezzanine:
                newPort->PortType.value = SF_EthernetPort::_PortType::enum_Other;
                newPort->OtherNetworkPortType.set("10GBase-KR");
                break;
            default:
                newPort->PortType.value = SF_EthernetPort::_PortType::enum_Unknown;
                break;
        }
        newPort->PortNumber.set(intf.elementId());
        newPort->LinkTechnology.null = false;
        newPort->LinkTechnology.value = SF_EthernetPort::_LinkTechnology::enum_Ethernet;
        newPort->PermanentAddress.set(intf.port()->permanentMAC().string());
        newPort->NetworkAddresses.null = false;
        newPort->NetworkAddresses.value.append(intf.currentMAC().string());
        newPort->FullDuplex.set(intf.port()->fullDuplex());
        newPort->AutoSense.set(intf.port()->autoneg());
        newPort->SupportedMaximumTransmissionUnit.set(intf.nic()->supportedMTU());
        newPort->ActiveMaximumTransmissionUnit.set(intf.mtu());
        newPort->Speed.set(Port::speedBPS(intf.port()->linkSpeed()));
        newPort->MaxSpeed.set(Port::speedBPS(intf.nic()->maxLinkSpeed()));
        newPort->Capabilities.null = false;
        newPort->Capabilities.value.append(SF_EthernetPort::_Capabilities::enum_Unknown);
        newPort->EnabledCapabilities.null = false;
        newPort->EnabledCapabilities.value.append(SF_EthernetPort::_EnabledCapabilities::enum_Unknown);
        
        return newPort;
    }

    bool EthernetPortHelper::match(const SystemElement& se, const Instance &obj, unsigned) const
    {
        const cimple::CIM_EthernetPort *eth = cast<const cimple::CIM_EthernetPort *>(&obj);
        if (eth == NULL)
            return false;
        if (eth->CreationClassName.null || eth->DeviceID.null ||
            eth->CreationClassName.value != "SF_EthernetPort" ||
            eth->SystemCreationClassName.null)
            return false;

        if (!isOurSystem(eth->SystemCreationClassName.value,
                         eth->SystemName.null ?
                                String("") : eth->SystemName.value))
            return false;
    
        return eth->DeviceID.value == static_cast<const Interface&>(se).ifName();
    }


    Instance *LANEndpointHelper::reference(const SystemElement& se, unsigned) const
    {
        const Interface& intf = static_cast<const Interface&>(se);
        const CIM_ComputerSystem *system = findSystem();
        SF_LANEndpoint *newEP = SF_LANEndpoint::create(true);
        
        newEP->CreationClassName.set("SF_LANEndpoint");
        newEP->Name.set(intf.ifName());
        newEP->SystemCreationClassName.set(system->CreationClassName.value);
        newEP->SystemName.set(system->Name.value);
        return newEP;
    }


    Instance *LANEndpointHelper::instance(const solarflare::SystemElement& se, unsigned idx) const
    {
        const solarflare::Interface& intf = static_cast<const solarflare::Interface&>(se);
        
        SF_LANEndpoint *newEP = static_cast<SF_LANEndpoint *>(reference(intf, idx));

        bool ifStatus;

        ASSIGN_IGNORE_EXCEPTION(ifStatus, intf.ifStatus(), false);

        newEP->Description.set(intf.description());
#if CIM_SCHEMA_VERSION_MINOR > 0
        newEP->ElementName.set(intf.ifName());
#endif
        newEP->NameFormat.set("Interface");
#if CIM_SCHEMA_VERSION_MINOR == 26
        newEP->InstanceID.set(instanceID(intf.name()));
#endif
#if CIM_SCHEMA_VERSION_MINOR > 0
        newEP->EnabledState.null = false;
        newEP->EnabledState.value = (ifStatus ?
                                     SF_LANEndpoint::_EnabledState::enum_Enabled : 
                                     SF_LANEndpoint::_EnabledState::enum_Disabled);
        newEP->RequestedState.null = false;
        newEP->RequestedState.value = SF_LANEndpoint::_RequestedState::enum_Not_Applicable;
#endif
        newEP->ProtocolIFType.null = false;
        newEP->ProtocolIFType.value = SF_LANEndpoint::_ProtocolIFType::enum_Gigabit_Ethernet;
        newEP->MACAddress.set(intf.currentMAC().string());
        
        return newEP;
    }

    bool LANEndpointHelper::match(const SystemElement& se, const Instance &obj, unsigned) const
    {
        const cimple::CIM_LANEndpoint *lan = cast<const cimple::CIM_LANEndpoint *>(&obj);
        if (lan == NULL)
            return false;
        if (lan->CreationClassName.null || lan->Name.null ||
            lan->CreationClassName.value != "SF_LANEndpoint" ||
            lan->SystemCreationClassName.null)
            return false;

        if (!isOurSystem(lan->SystemCreationClassName.value,
                         lan->SystemName.null ?
                                      String("") : lan->SystemName.value))
            return false;
    
        return lan->Name.value == static_cast<const Interface&>(se).ifName();
    }


    Instance *ConnectorRealizesPortHelper::instance(const SystemElement& se, unsigned idx) const
    {
        const solarflare::Interface& intf = static_cast<const solarflare::Interface&>(se);
    
        const solarflare::Port *port = intf.port();
        if (port != NULL)
        {
            SF_ConnectorRealizesPort* link = SF_ConnectorRealizesPort::create(true);
            
            link->Antecedent = cast<cimple::CIM_PhysicalElement *>(port->cimReference(SF_PhysicalConnector::static_meta_class));
            link->Dependent = cast<cimple::CIM_LogicalDevice *>(intf.cimReference(SF_EthernetPort::static_meta_class));
            return link;
        }
        else
        {
            return NULL;
        }
    }

    Instance *ConnectorRealizesControllerHelper::instance(const SystemElement& se,
                                                          unsigned idx) const
    {
        const solarflare::Interface& intf = static_cast<const solarflare::Interface&>(se);
    
        const solarflare::Port *port = intf.port();
        const solarflare::NIC *nic = intf.nic();
        if (port != NULL && nic != NULL)
        {
            SF_ConnectorRealizesController* link =
                              SF_ConnectorRealizesController::create(true);
            
            link->Antecedent = cast<cimple::CIM_PhysicalElement *>(port->cimReference(SF_PhysicalConnector::static_meta_class));
            link->Dependent = cast<cimple::CIM_LogicalDevice *>(nic->cimReference(SF_PortController::static_meta_class));
            return link;
        }
        else
            return NULL;
    }

    Instance *ControlledByHelper::instance(const SystemElement& se, unsigned) const
    {
        const solarflare::Interface& intf = static_cast<const Interface&>(se);
        SF_ControlledBy *link = SF_ControlledBy::create(true);
        
        link->Dependent = cast<cimple::CIM_EthernetPort *>(intf.cimReference(SF_EthernetPort::static_meta_class));
        link->Antecedent = cast<cimple::CIM_PortController *>(intf.nic()->cimReference(SF_PortController::static_meta_class));
        return link;
    }

    Instance *PortAndEndpointCapabilitiesHelper::reference(const SystemElement& se, unsigned idx) const 
    {
        switch (idx)
        {
            case 0:
                return portCaps.reference(se, 0);
            case 1:
                return endpointCaps.reference(se, 1);
            default:
                return NULL;
        }
    }

    Instance *PortAndEndpointCapabilitiesHelper::instance(const SystemElement& se, unsigned idx) const 
    {
        switch (idx)
        {
            case 0:
                return portCaps.instance(se, 0);
            case 1:
                return endpointCaps.instance(se, 1);
            default:
                return NULL;
        }
    }

    Instance *PortAndEndpointCapsLinkHelper::instance(const SystemElement& se, unsigned idx) const 
    {
        switch (idx)
        {
            case 0:
                return portCaps.instance(se, 0);
            case 1:
                return endpointCaps.instance(se, 1);
            default:
                return NULL;
        }
    }


    Instance * 
    HostedAccessPointHelper::instance(const solarflare::SystemElement& se, unsigned) const
    {
        SF_HostedAccessPoint *link = SF_HostedAccessPoint::create(true);
        link->Antecedent = solarflare::CIMHelper::systemRef();
        link->Dependent = cast<cimple::CIM_ServiceAccessPoint *>(se.cimReference(SF_LANEndpoint::static_meta_class));
#if NEED_ASSOC_IN_ROOT_CIMV2
        link->Dependent->__name_space = CIMHelper::solarflareNS;
#endif
        return link;
    }

    Instance * 
    NICSAPImplementationHelper::instance(const solarflare::SystemElement& se, unsigned) const
    {
        const solarflare::Interface& intf = static_cast<const solarflare::Interface&>(se);
    
        SF_NICSAPImplementation *link = SF_NICSAPImplementation::create(true);
        link->Antecedent = static_cast<cimple::CIM_LogicalDevice *>(intf.cimReference(SF_EthernetPort::static_meta_class));
        link->Dependent = cast<cimple::CIM_ServiceAccessPoint *>(intf.cimReference(SF_LANEndpoint::static_meta_class));

        return link;
    }


    Instance *EthernetPortSystemDeviceHelper::instance(const SystemElement& se, unsigned) const
    {
        SF_SystemDevice *dev = SF_SystemDevice::create(true);

        dev->GroupComponent = systemRef();
        dev->PartComponent = static_cast<cimple::CIM_LogicalDevice *>(se.cimReference(SF_EthernetPort::static_meta_class));
#if NEED_ASSOC_IN_ROOT_CIMV2
        dev->PartComponent->__name_space = CIMHelper::solarflareNS;
#endif
        
        return dev;
    }

    Instance *PortConformsToProfile::instance(const SystemElement &se, unsigned idx) const
    {
        switch (idx)
        {
            case 0:
                return DMTFProfileInfo::HostLANNetworkPortProfile.           \
                conformingElement(se.cimReference(SF_EthernetPort::static_meta_class));
            case 1:
                return DMTFProfileInfo::EthernetPortProfile.           \
                conformingElement(se.cimReference(SF_EthernetPort::static_meta_class));
            default:
                return NULL;
        }
    }


} // namespace
