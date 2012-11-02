#include "sf_provider.h"
#include "SF_EthernetPort.h"
#include "SF_LANEndpoint.h"

namespace solarflare 
{
    using cimple::Instance;
    using cimple::Meta_Class;
    using cimple::CIM_ComputerSystem;
    using cimple::Ref;
    using cimple::cast;
    using cimple::SF_EthernetPort;
    using cimple::SF_LANEndpoint;

    class EthernetPortHelper : public CIMHelper {
    public:
        virtual Instance *reference(const SystemElement& obj) const;
        virtual Instance *instance(const SystemElement&) const;
        virtual bool match(const SystemElement& obj, const Instance& inst) const;
    };

    class LANEndpointHelper : public CIMHelper {
    public:
        virtual Instance *reference(const SystemElement& obj) const;
        virtual Instance *instance(const SystemElement&) const;
        virtual bool match(const SystemElement& obj, const Instance& inst) const;
    };



    const CIMHelper* Interface::cimDispatch(const Meta_Class& cls) const
    {
        static const EthernetPortHelper ethernetPortHelper;
        static const LANEndpointHelper lanEndpointHelper;
        if (&cls == &SF_EthernetPort::static_meta_class)
            return &ethernetPortHelper;
        if (&cls == &SF_LANEndpoint::static_meta_class)
            return &lanEndpointHelper;
        return NULL;
    }

    Instance *EthernetPortHelper::reference(const SystemElement& se) const
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


    Instance *EthernetPortHelper::instance(const solarflare::SystemElement& se) const 
    {
        const solarflare::Interface& intf = static_cast<const solarflare::Interface&>(se);
    
        SF_EthernetPort *newPort = static_cast<SF_EthernetPort *>(reference(intf));

        newPort->Description.set(intf.description());
        newPort->ElementName.set(intf.ifName());
        newPort->Name.set(intf.name());
        newPort->InstanceID.set(instanceID(intf.name()));
        newPort->EnabledState.null = false;
        newPort->EnabledState.value = (intf.ifStatus() ?
                                       (intf.port()->linkStatus() ? 
                                        SF_EthernetPort::_EnabledState::enum_Enabled : 
                                        SF_EthernetPort::_EnabledState::enum_Enabled_but_Offline) :
                                       SF_EthernetPort::_EnabledState::enum_Disabled);
        newPort->RequestedState.null = false;
        newPort->RequestedState.value = SF_EthernetPort::_RequestedState::enum_Not_Applicable;
        newPort->StatusInfo.null = false;
        newPort->StatusInfo.value = (intf.ifStatus() ?
                                     SF_EthernetPort::_StatusInfo::enum_Enabled :
                                     SF_EthernetPort::_StatusInfo::enum_Disabled);
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
        
        return newPort;
    }

    bool EthernetPortHelper::match(const SystemElement& se, const Instance &obj) const
    {
        const cimple::CIM_EthernetPort *eth = cast<const cimple::CIM_EthernetPort *>(&obj);
        if (eth == NULL)
            return false;
        if (eth->CreationClassName.null || eth->DeviceID.null ||
            eth->CreationClassName.value != "SF_EthernetPort" ||
            eth->SystemCreationClassName.null || eth->SystemName.null)
            return false;

        if (!isOurSystem(eth->SystemCreationClassName.value, eth->SystemName.value))
            return false;
    
        return eth->DeviceID.value == static_cast<const Interface&>(se).ifName();
    }


    Instance *LANEndpointHelper::reference(const SystemElement& se) const
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


    Instance *LANEndpointHelper::instance(const solarflare::SystemElement& se) const
    {
        const solarflare::Interface& intf = static_cast<const solarflare::Interface&>(se);
        
        SF_LANEndpoint *newEP = static_cast<SF_LANEndpoint *>(reference(intf));

        newEP->Description.set(intf.description());
        newEP->ElementName.set(intf.ifName());
        newEP->NameFormat.set("Interface");
        newEP->InstanceID.set(instanceID(intf.name()));
        newEP->EnabledState.null = false;
        newEP->EnabledState.value = (intf.ifStatus() ?
                                     SF_LANEndpoint::_EnabledState::enum_Enabled : 
                                     SF_LANEndpoint::_EnabledState::enum_Disabled);
        newEP->RequestedState.null = false;
        newEP->RequestedState.value = SF_LANEndpoint::_RequestedState::enum_Not_Applicable;
        newEP->ProtocolIFType.null = false;
        newEP->ProtocolIFType.value = SF_LANEndpoint::_ProtocolIFType::enum_Gigabit_Ethernet;
        newEP->MACAddress.set(intf.currentMAC().string());
        
        return newEP;
    }

    bool LANEndpointHelper::match(const SystemElement& se, const Instance &obj) const
    {
        const cimple::CIM_LANEndpoint *lan = cast<const cimple::CIM_LANEndpoint *>(&obj);
        if (lan == NULL)
            return false;
        if (lan->CreationClassName.null || lan->Name.null ||
            lan->CreationClassName.value != "SF_LANEndpoint" ||
            lan->SystemCreationClassName.null || lan->SystemName.null)
            return false;

        if (!isOurSystem(lan->SystemCreationClassName.value, lan->SystemName.value))
            return false;
    
        return lan->Name.value == static_cast<const Interface&>(se).ifName();
    }



} // namespace