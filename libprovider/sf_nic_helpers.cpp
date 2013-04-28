#include "sf_provider.h"
#include "SF_NICCard.h"
#include "SF_PortController.h"
#include "SF_EnabledLogicalElementCapabilities.h"
#include "SF_ElementCapabilities.h"
#include "SF_ElementSoftwareIdentity.h"
#include "SF_ElementConformsToProfile.h"
#include "SF_SoftwareIdentity.h"
#include "SF_SystemDevice.h"
#include "SF_CardRealizesController.h"
#include "SF_ComputerSystemPackage.h"

namespace solarflare 
{
    using cimple::Instance;
    using cimple::Meta_Class;
    using cimple::CIM_ComputerSystem;
    using cimple::Ref;
    using cimple::cast;

    using cimple::SF_NICCard;
    using cimple::SF_PortController;
    using cimple::SF_EnabledLogicalElementCapabilities;
    using cimple::SF_ElementCapabilities;
    using cimple::SF_ElementSoftwareIdentity;
    using cimple::SF_ElementConformsToProfile;
    using cimple::SF_SoftwareIdentity;
    using cimple::SF_SystemDevice;
    using cimple::SF_CardRealizesController;
    using cimple::SF_ComputerSystemPackage;

    class NICCardHelper : public CIMHelper {
    public:
        virtual Instance *reference(const SystemElement& obj, unsigned) const;
        virtual Instance *instance(const SystemElement&, unsigned) const;
        virtual bool match(const SystemElement& obj, const Instance& inst, unsigned) const;
    };

    class PortControllerHelper : public CIMHelper {
    public:
        virtual Instance *reference(const SystemElement& obj, unsigned) const;
        virtual Instance *instance(const SystemElement&, unsigned) const;
        virtual bool match(const SystemElement& obj, const Instance& inst, unsigned) const;
    };

    class DriverElementSoftwareIdentityHelper : public CIMHelper {
    public:
        virtual Instance *instance(const SystemElement& se, unsigned idx) const;
    };

    class PortControllerSystemDeviceHelper : public CIMHelper {
    public:
        virtual Instance *instance(const SystemElement& se, unsigned idx) const;
    };

    class NICConformsToProfile : public CIMHelper {
        virtual Instance *instance(const SystemElement &se, unsigned) const;
    };

    class CardRealizesControllerHelper : public CIMHelper {
    public:
        virtual Instance *instance(const SystemElement&, unsigned) const;
    };

    class ComputerSystemPackageHelper : public CIMHelper {
    public:
        virtual Instance *instance(const SystemElement&, unsigned) const;
    };

    const CIMHelper *NIC::cimDispatch(const Meta_Class& cls) const
    {
        static const NICCardHelper nicCardHelper;
        static const PortControllerHelper ethernetPortHelper;
        static const EnabledLogicalElementCapabilitiesHelper capabilities("Controller", false);
        static const ElementCapabilitiesHelper capsLink(SF_PortController::static_meta_class,
                                                        SF_EnabledLogicalElementCapabilities::static_meta_class);
        static const DriverElementSoftwareIdentityHelper driverSoftwareIdentity;
        static const PortControllerSystemDeviceHelper systemDevice;
        static const NICConformsToProfile conforming;
        static const CardRealizesControllerHelper cardRealizesControllerHelper;
        static const ComputerSystemPackageHelper computerSystemPackageHelper;
        
        if (&cls == &SF_NICCard::static_meta_class)
            return &nicCardHelper;
        if (&cls == &SF_PortController::static_meta_class)
            return &ethernetPortHelper;
        if (&cls == &SF_EnabledLogicalElementCapabilities::static_meta_class)
            return &capabilities;
        if (&cls == &SF_ElementCapabilities::static_meta_class)
            return &capsLink;
        if (&cls == &SF_ElementSoftwareIdentity::static_meta_class)
            return &driverSoftwareIdentity;
        if (&cls == &SF_SystemDevice::static_meta_class)
            return &systemDevice;
        if (&cls == &SF_ElementConformsToProfile::static_meta_class)
            return &conforming;
        if (&cls == &SF_CardRealizesController::static_meta_class)
            return &cardRealizesControllerHelper;
        if (&cls == &SF_ComputerSystemPackage::static_meta_class)
            return &computerSystemPackageHelper;
        return NULL;
    }

    Instance *NICCardHelper::reference(const SystemElement& se, unsigned) const 
    {
        
        SF_NICCard *card = SF_NICCard::create(true);

        card->CreationClassName.set("SF_NICCard");
        card->Tag.set(static_cast<const NIC&>(se).vitalProductData().id());

        return card;
    }

    Instance* NICCardHelper::instance(const SystemElement& se, unsigned idx) const
    {

        SF_NICCard *card = static_cast<SF_NICCard *>(reference(se, idx));

        const NIC& nic = static_cast<const NIC&>(se);
        solarflare::VitalProductData vpd = nic.vitalProductData();
   
#if CIM_SCHEMA_VERSION_MINOR == 26 
        card->InstanceID.set(instanceID(nic.name()));
#endif
        card->Name.set(nic.name());
        card->Description.set(nic.description());
        card->Manufacturer.set(vpd.manufacturer());
        card->SerialNumber.set(vpd.serial());
        card->PartNumber.set(vpd.part());
        card->Model.set(vpd.model());
        card->SKU.set(vpd.fru());
#if CIM_SCHEMA_VERSION_MINOR > 0
        card->PackageType.null = false;
        card->PackageType.value = SF_NICCard::_PackageType::enum_Module_Card;
#endif
        card->HostingBoard.set(false);
        card->PoweredOn.set(true);
#if CIM_SCHEMA_VERSION_MINOR > 0
        card->ElementName.set(nic.name());
#endif

        return card;
    }

    bool NICCardHelper::match(const SystemElement& se, const Instance &obj, unsigned) const
    {
        const cimple::CIM_Card *card = static_cast<const cimple::CIM_Card *>(&obj);
        if (card == NULL)
            return false;
        if (card->CreationClassName.null || card->Tag.null ||
            card->CreationClassName.value != "SF_NICCard")
            return false;
        return card->Tag.value == static_cast<const NIC&>(se).vitalProductData().id();
    }


    Instance *PortControllerHelper::reference(const SystemElement& nic, unsigned) const 
    {
        const CIM_ComputerSystem *system = findSystem();
        SF_PortController *newPC = SF_PortController::create(true);
    
        newPC->CreationClassName.set("SF_PortController");
        newPC->DeviceID.set(static_cast<const NIC&>(nic).vitalProductData().id());
        newPC->SystemCreationClassName.set(system->CreationClassName.value);
        newPC->SystemName.set(system->Name.value);
        return newPC;
    }

    Instance *PortControllerHelper::instance(const SystemElement& se, unsigned idx) const
    {
        const solarflare::NIC& nic = static_cast<const solarflare::NIC&>(se);
        
        SF_PortController *newPC = static_cast<SF_PortController *>(reference(nic, idx));
        ObjectCount counter;
        
        newPC->Description.set(nic.description());
        newPC->Name.set(nic.name());
#if CIM_SCHEMA_VERSION_MINOR > 0
        newPC->ElementName.set(nic.name());
#endif
        
        newPC->ControllerType.null = false;
        newPC->ControllerType.value = SF_PortController::_ControllerType::enum_Ethernet;
#if CIM_SCHEMA_VERSION_MINOR > 0
        // fixme: I could not figure out whether 'protocol' here is to be meant
        // as system<->device protocol (PCI), as a networking protocol
        // (Ethernet), or as an internal protocol (like MII)
        // Emulex uses "Other" type with unclear description; IBM
        // uses null, even though the field is mandatory in the profile.
        // The value map of the field does not unfortunately include any values
        // for 'modern' Ethernet; nor there are things like 'MII'
        newPC->ProtocolSupported.null = false;
        newPC->ProtocolSupported.value = SF_PortController::_ProtocolSupported::enum_PCI;


        newPC->RequestedState.null = false;
        newPC->RequestedState.value = SF_PortController::_RequestedState::enum_Not_Applicable;
        newPC->EnabledState.null = false;
        newPC->EnabledState.value = SF_PortController::_EnabledState::enum_Enabled;
#endif
        
        nic.forAllPorts(counter);
        newPC->MaxNumberControlled.set(counter.count());
        
        return newPC;
    }

    bool PortControllerHelper::match(const SystemElement& se, const Instance &obj, unsigned) const
    {
        const cimple::CIM_PortController *card = static_cast<const cimple::CIM_PortController *>(&obj);
        if (card == NULL)
            return false;
        if (card->CreationClassName.null || card->DeviceID.null ||
            card->CreationClassName.value != "SF_PortController")
            return false;
        return card->DeviceID.value == static_cast<const NIC&>(se).vitalProductData().id();
    }

    Instance *
    DriverElementSoftwareIdentityHelper::instance(const SystemElement& se, unsigned idx) const
    {
        const NIC &nic = static_cast<const NIC &>(se);
        const Driver *drv = nic.driver();
        
        if (drv == NULL)
            return NULL;
        SF_ElementSoftwareIdentity *item = SF_ElementSoftwareIdentity::create(true);
    
        item->Antecedent = cast<cimple::CIM_SoftwareIdentity *>(drv->cimReference(SF_SoftwareIdentity::static_meta_class));
        item->Dependent = cast<cimple::CIM_ManagedElement *>(nic.cimReference(SF_PortController::static_meta_class));
    
        item->ElementSoftwareStatus.null = false;
        item->ElementSoftwareStatus.value.append(SF_ElementSoftwareIdentity::_ElementSoftwareStatus::enum_Current);
        item->ElementSoftwareStatus.value.append(SF_ElementSoftwareIdentity::_ElementSoftwareStatus::enum_Next);
        item->ElementSoftwareStatus.value.append(SF_ElementSoftwareIdentity::_ElementSoftwareStatus::enum_Default);

        return item;
    }

    Instance *
    PortControllerSystemDeviceHelper::instance(const solarflare::SystemElement& se, unsigned) const
    {
        const NIC& nic = static_cast<const NIC&>(se);
        SF_SystemDevice *dev = SF_SystemDevice::create(true);
        
        dev->GroupComponent = systemRef();
        dev->PartComponent = cast<cimple::CIM_LogicalDevice *>(nic.cimReference(SF_PortController::static_meta_class));

        return dev;
    }
    

    Instance *NICConformsToProfile::instance(const SystemElement &se, unsigned) const
    {
        return DMTFProfileInfo::PhysicalAssetProfile.                   \
        conformingElement(se.cimReference(SF_NICCard::static_meta_class));
    }

    Instance *CardRealizesControllerHelper::instance(const SystemElement& se, unsigned idx) const
    {
        const NIC& nic = static_cast<const NIC&>(se);
        SF_CardRealizesController* link = SF_CardRealizesController::create(true);

        link->Antecedent = cast<cimple::CIM_PhysicalElement *>(nic.cimReference(SF_NICCard::static_meta_class));
        link->Dependent =  cast<cimple::CIM_LogicalDevice *>(nic.cimReference(SF_PortController::static_meta_class));
        return link;
    }

    Instance *ComputerSystemPackageHelper::instance(const SystemElement& se, unsigned idx) const
    {
        const NIC& nic = static_cast<const NIC&>(se);
        SF_ComputerSystemPackage* link = SF_ComputerSystemPackage::create(true);

        link->Antecedent = cast<cimple::CIM_PhysicalPackage *>(nic.cimReference(SF_NICCard::static_meta_class));
#if CIM_SCHEMA_VERSION_MINOR > 0
        link->Dependent =  findSystem()->clone();
#else
        link->Dependent =  cast<cimple::CIM_UnitaryComputerSystem *>(findSystem()->clone());
#endif
        return link;
    }

} // namespace
