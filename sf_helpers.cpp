#include "sf_platform.h"
#include "SF_NICCard.h"

namespace solarflare 
{
    using cimple::Instance;
    using cimple::Meta_Class;
    
    String makeInstanceID(const String& name)
    {
        String result = System::target.prefix();
        result.append(":");
        result.append(name);
        return result;
    }

    Instance *NIC::cimReference(const Meta_Class& cls) const 
    {
        using cimple::SF_NICCard;
        
        if (&cls != &SF_NICCard::static_meta_class)
            return NULL;
        
        SF_NICCard *card = SF_NICCard::create(true);

        card->CreationClassName.set("SF_NICCard");
        card->Tag.set(vitalProductData().id());

        return card;
    }

    Instance* NIC::cimInstance(const Meta_Class &cls) const
    {
        using cimple::SF_NICCard;

        SF_NICCard *card = static_cast<SF_NICCard *>(cimReference(cls));

        if (card == NULL)
            return NULL;
        
        solarflare::VitalProductData vpd = vitalProductData();
    
        card->InstanceID.set(makeInstanceID(name()));
        card->Name.set(name());
        card->Description.set(description());
        card->Manufacturer.set(vpd.manufacturer());
        card->SerialNumber.set(vpd.serial());
        card->PartNumber.set(vpd.part());
        card->Model.set(vpd.model());
        card->SKU.set(vpd.fru());
        card->PackageType.null = false;
        card->PackageType.value = SF_NICCard::_PackageType::enum_Module_Card;
        card->HostingBoard.set(false);
        card->PoweredOn.set(true);
        card->ElementName.set(name());

        return card;
    }

    bool NIC::cimIsMe(const Instance *obj) const
    {
        const cimple::CIM_Card *card = static_cast<const cimple::CIM_Card *>(obj);
        if (card == NULL)
            return false;
        if (card->CreationClassName.null || card->Tag.null ||
            card->CreationClassName.value != "SF_NICCard")
            return false;
        return card->Tag.value == vitalProductData().id();
    }


}
