#include "sf_provider.h"
#include "SF_PhysicalConnector.h"

namespace solarflare 
{
    using cimple::Instance;
    using cimple::Meta_Class;
    using cimple::CIM_ComputerSystem;
    using cimple::Ref;
    using cimple::cast;
    using cimple::SF_PhysicalConnector;

    class PhysicalConnectorHelper : public CIMHelper {
        String tag(const Port& p) const;
    public:
        virtual Instance *reference(const SystemElement& obj) const;
        virtual Instance *instance(const SystemElement&) const;
        virtual bool match(const SystemElement& obj, const Instance& inst) const;
    };

    const CIMHelper* Port::cimDispatch(const Meta_Class& cls) const
    {
        static const PhysicalConnectorHelper physicalConnector;
        if (&cls == &SF_PhysicalConnector::static_meta_class)
            return &physicalConnector;
        return NULL;
    }

    String PhysicalConnectorHelper::tag(const Port& p) const
    {
        Buffer buf;
        buf.appends(p.nic()->vitalProductData().id().c_str());
        buf.append(':');
        buf.append_uint16(p.elementId());
        return buf.data();
    }

    Instance *PhysicalConnectorHelper::reference(const SystemElement& p) const
    {
        SF_PhysicalConnector *phc = SF_PhysicalConnector::create(true);
        
        phc->CreationClassName.set("SF_PhysicalConnector");
        phc->Tag.set(tag(static_cast<const Port&>(p)));
   
        return phc;
    }

    Instance *PhysicalConnectorHelper::instance (const solarflare::SystemElement& se) const
    {
        const solarflare::Port& p = static_cast<const solarflare::Port&>(se);
        SF_PhysicalConnector *phc = static_cast<SF_PhysicalConnector *>(reference(p));
        
        phc->InstanceID.set(instanceID(p.name()));
        phc->Name.set(p.name());
        phc->ElementName.set(p.name());
        phc->Description.set(p.description());
        
        phc->ConnectorType.null = false;
        phc->ConnectorLayout.null = false;
        switch (p.nic()->connector()) 
        {
            case solarflare::NIC::RJ45:
                phc->ConnectorType.value.append(SF_PhysicalConnector::_ConnectorType::enum_RJ45);
                phc->ConnectorLayout.value = SF_PhysicalConnector::_ConnectorLayout::enum_RJ45;
                break;
            case solarflare::NIC::SFPPlus:
                phc->ConnectorType.value.append(SF_PhysicalConnector::_ConnectorType::enum_Fibre_Channel__Optical_Fibre_);
                phc->ConnectorLayout.value = SF_PhysicalConnector::_ConnectorLayout::enum_Fiber_SC;
                break;
            case solarflare::NIC::Mezzanine:
                phc->ConnectorType.value.append(SF_PhysicalConnector::_ConnectorType::enum_PMC);
                phc->ConnectorLayout.value = SF_PhysicalConnector::_ConnectorLayout::enum_Slot;
                break;
            default:
                phc->ConnectorType.value.append(SF_PhysicalConnector::_ConnectorType::enum_Unknown);
                phc->ConnectorLayout.value = SF_PhysicalConnector::_ConnectorLayout::enum_Unknown;
                break;
        }
        return phc;
    }

    bool PhysicalConnectorHelper::match(const SystemElement& se, const Instance &obj) const
    {
        const cimple::CIM_PhysicalConnector *card = static_cast<const cimple::CIM_PhysicalConnector *>(&obj);
        if (card == NULL)
            return false;
        if (card->CreationClassName.null || card->Tag.null ||
            card->CreationClassName.value != "SF_PhysicalConnector")
            return false;
        return card->Tag.value == tag(static_cast<const Port&>(se));
    }

} // namespace
