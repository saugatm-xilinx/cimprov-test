// Generated by genprov 2.0.24
#include "SF_NICCard_Provider.h"
#include "SF_Sysinfo.h"

CIMPLE_NAMESPACE_BEGIN

SF_NICCard *SF_NICCard_Provider::makeReference(const solarflare::NIC& nic)
{
    SF_NICCard *card = SF_NICCard::create(true);

    card->CreationClassName.set("SF_NICCard");
    card->Tag.set(nic.vitalProductData().id());

    return card;
}


bool SF_NICCard_Provider::NICEnum::process(const solarflare::NIC& nic)
{
    SF_NICCard *card = SF_NICCard::create(true);
    solarflare::VitalProductData vpd = nic.vitalProductData();
    
    card->InstanceID.set(solarflare::System::target.prefix());
    card->InstanceID.value.append(":");
    card->InstanceID.value.append(nic.name());
    card->CreationClassName.set("SF_NICCard");
    card->Tag.set(vpd.id());
    card->Name.set(nic.name());
    card->Description.set(nic.description());
    card->Manufacturer.set(vpd.manufacturer());
    card->SerialNumber.set(vpd.serial());
    card->PartNumber.set(vpd.part());
    card->Model.set(vpd.model());
    card->SKU.set(vpd.fru());
    card->PackageType.null = false;
    card->PackageType.value = SF_NICCard::_PackageType::enum_Module_Card;
    card->HostingBoard.set(false);
    card->PoweredOn.set(true);
    card->ElementName.set(nic.name());
    

    handler->handle(card);

    return false;
}

SF_NICCard_Provider::SF_NICCard_Provider()
{
}

SF_NICCard_Provider::~SF_NICCard_Provider()
{
}

Load_Status SF_NICCard_Provider::load()
{
    return LOAD_OK;
}

Unload_Status SF_NICCard_Provider::unload()
{
    return UNLOAD_OK;
}

Get_Instance_Status SF_NICCard_Provider::get_instance(
    const SF_NICCard* model,
    SF_NICCard*& instance)
{
    return GET_INSTANCE_UNSUPPORTED;
}

Enum_Instances_Status SF_NICCard_Provider::enum_instances(
    const SF_NICCard* model,
    Enum_Instances_Handler<SF_NICCard>* handler)
{
    NICEnum nics(handler);
    solarflare::System::target.forAllNICs(nics);
    return ENUM_INSTANCES_OK;
}

Create_Instance_Status SF_NICCard_Provider::create_instance(
    SF_NICCard* instance)
{
    return CREATE_INSTANCE_UNSUPPORTED;
}

Delete_Instance_Status SF_NICCard_Provider::delete_instance(
    const SF_NICCard* instance)
{
    return DELETE_INSTANCE_UNSUPPORTED;
}

Modify_Instance_Status SF_NICCard_Provider::modify_instance(
    const SF_NICCard* model,
    const SF_NICCard* instance)
{
    return MODIFY_INSTANCE_UNSUPPORTED;
}

Invoke_Method_Status SF_NICCard_Provider::IsCompatible(
    const SF_NICCard* self,
    const CIM_PhysicalElement* ElementToCheck,
    Property<uint32>& return_value)
{
    return INVOKE_METHOD_UNSUPPORTED;
}

Invoke_Method_Status SF_NICCard_Provider::ConnectorPower(
    const SF_NICCard* self,
    const CIM_PhysicalConnector* Connector,
    const Property<boolean>& PoweredOn,
    Property<uint32>& return_value)
{
    return INVOKE_METHOD_UNSUPPORTED;
}

/*@END@*/

CIMPLE_NAMESPACE_END
