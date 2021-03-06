/***************************************************************************//*! \file liprovider/sf_port_helpers.cpp
** <L5_PRIVATE L5_SOURCE>
** \author  OktetLabs
**  \brief  CIM Provider
**   \date  2013/10/02
**    \cop  (c) Solarflare Communications Inc.
** </L5_PRIVATE>
*//*
\**************************************************************************/

#include "sf_provider.h"
#include "SF_PhysicalConnector.h"
#include "SF_NICCard.h"
#include "SF_ConnectorOnNIC.h"
#include "SF_ElementConformsToProfile.h"

namespace solarflare
{
    using cimple::Instance;
    using cimple::Meta_Class;
    using cimple::CIM_ComputerSystem;
    using cimple::Ref;
    using cimple::cast;
    using cimple::SF_PhysicalConnector;
    using cimple::SF_ConnectorOnNIC;
    using cimple::SF_NICCard;
    using cimple::SF_ElementConformsToProfile;

    class PhysicalConnectorHelper : public CIMHelper {
        String tag(const Port& p) const;
    public:
        virtual Instance *reference(const SystemElement& obj, unsigned) const;
        virtual Instance *instance(const SystemElement&, unsigned) const;
        virtual bool match(const SystemElement& obj, const Instance& inst, unsigned) const;
    };

    class ConnectorOnNICHelper : public CIMHelper {
    public:
        virtual Instance *instance(const SystemElement&, unsigned) const;
    };

    class ConnectorConformsToProfile : public CIMHelper {
        virtual Instance *instance(const SystemElement &se, unsigned) const;
    };


    const CIMHelper* Port::cimDispatch(const Meta_Class& cls) const
    {
        static const PhysicalConnectorHelper physicalConnector;
        static const ConnectorOnNICHelper connectorOnNIC;
        static const ConnectorConformsToProfile conforming;

        if (&cls == &SF_PhysicalConnector::static_meta_class)
            return &physicalConnector;
        if (&cls == &SF_ConnectorOnNIC::static_meta_class)
            return &connectorOnNIC;
        if (&cls == &SF_ElementConformsToProfile::static_meta_class)
            return &conforming;
        return NULL;
    }

    String PhysicalConnectorHelper::tag(const Port& p) const
    {
        Buffer buf;
        buf.appends("PhysicalConnector for ");
        buf.appends(p.permanentMAC().string().c_str());
        return buf.data();
    }

    Instance *PhysicalConnectorHelper::reference(const SystemElement& p, unsigned) const
    {
        SF_PhysicalConnector *phc = SF_PhysicalConnector::create(true);

        phc->CreationClassName.set("SF_PhysicalConnector");
        phc->Tag.set(tag(static_cast<const Port&>(p)));

        return phc;
    }

    Instance *PhysicalConnectorHelper::instance (const solarflare::SystemElement& se, unsigned idx) const
    {
        const solarflare::Port& p = static_cast<const solarflare::Port&>(se);
        SF_PhysicalConnector *phc = static_cast<SF_PhysicalConnector *>(reference(p, idx));
        Buffer buf;

        buf.appends("LocalPhysicalPort:");
        buf.append_uint16(p.elementId());
        buf.appends(",Transceiver:");
        /// TODO: fix it
        buf.appends("SFP+");
        phc->OtherIdentifyingInfo.set(buf.data());

#if  CIM_SCHEMA_VERSION_MINOR == 26
        phc->InstanceID.set(instanceID(p.name()));
#endif
        phc->Name.set(tag(p));
#if CIM_SCHEMA_VERSION_MINOR > 0
        phc->ElementName.set(p.name());
#endif
        phc->Description.set(p.description());

        phc->ConnectorType.null = false;
#if CIM_SCHEMA_VERSION_MINOR > 0
        phc->ConnectorLayout.null = false;
#endif
        switch (p.nic()->connector())
        {
            case solarflare::NIC::RJ45:
#if CIM_SCHEMA_VERSION_MINOR > 0
                phc->ConnectorType.value.append(SF_PhysicalConnector::_ConnectorType::enum_RJ45);
#else
                phc->ConnectorType.value.append(39);
#endif
#if CIM_SCHEMA_VERSION_MINOR > 0
                phc->ConnectorLayout.value = SF_PhysicalConnector::_ConnectorLayout::enum_RJ45;
#endif
                break;
            case solarflare::NIC::SFPPlus:
#if CIM_SCHEMA_VERSION_MINOR > 0
                phc->ConnectorType.value.append(SF_PhysicalConnector::_ConnectorType::enum_Fibre_Channel__Optical_Fibre_);
#else
                phc->ConnectorType.value.append(12);
#endif
#if CIM_SCHEMA_VERSION_MINOR > 0
                phc->ConnectorLayout.value = SF_PhysicalConnector::_ConnectorLayout::enum_Fiber_SC;
#endif
                break;
            case solarflare::NIC::Mezzanine:
#if CIM_SCHEMA_VERSION_MINOR > 0
                phc->ConnectorType.value.append(SF_PhysicalConnector::_ConnectorType::enum_PMC);
#else
                phc->ConnectorType.value.append(106);
#endif
#if CIM_SCHEMA_VERSION_MINOR > 0
                phc->ConnectorLayout.value = SF_PhysicalConnector::_ConnectorLayout::enum_Slot;
#endif
                break;
            default:
#if CIM_SCHEMA_VERSION_MINOR > 0
                phc->ConnectorType.value.append(SF_PhysicalConnector::_ConnectorType::enum_Unknown);
#else
                phc->ConnectorType.value.append(0);
#endif
#if CIM_SCHEMA_VERSION_MINOR > 0
                phc->ConnectorLayout.value = SF_PhysicalConnector::_ConnectorLayout::enum_Unknown;
#endif
                break;
        }

        return phc;
    }

    bool PhysicalConnectorHelper::match(const SystemElement& se, const Instance &obj, unsigned) const
    {
        const cimple::CIM_PhysicalConnector *card = cast<const cimple::CIM_PhysicalConnector *>(&obj);
        if (card == NULL)
            return false;
        if (card->CreationClassName.null || card->Tag.null ||
            card->CreationClassName.value != "SF_PhysicalConnector")
            return false;
        return card->Tag.value == tag(static_cast<const Port&>(se));
    }

    Instance *ConnectorOnNICHelper::instance(const solarflare::SystemElement& se, unsigned) const
    {
        const solarflare::Port& e = static_cast<const solarflare::Port&>(se);

        SF_ConnectorOnNIC *link = SF_ConnectorOnNIC::create(true);
        link->GroupComponent =
        cast<cimple::CIM_PhysicalPackage *>(e.nic()->cimReference(SF_NICCard::static_meta_class));
        link->PartComponent =
        cast<cimple::CIM_PhysicalConnector *>(e.cimReference(SF_PhysicalConnector::static_meta_class));

        return link;
    }

    Instance *ConnectorConformsToProfile::instance(const SystemElement &se, unsigned) const
    {
        return DMTFProfileInfo::PhysicalAssetProfile.                   \
        conformingElement(se.cimReference(SF_PhysicalConnector::static_meta_class));
    }

} // namespace
