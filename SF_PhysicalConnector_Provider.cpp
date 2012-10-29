// Generated by genprov 2.0.24
#include "SF_PhysicalConnector_Provider.h"
#include "sf_platform.h"

CIMPLE_NAMESPACE_BEGIN

SF_PhysicalConnector *SF_PhysicalConnector_Provider::makeReference(const solarflare::Port& p)
{
    SF_PhysicalConnector *phc = SF_PhysicalConnector::create(true);

    phc->CreationClassName.set("SF_NICCard");
    Buffer buf;
    buf.appends(p.nic()->vitalProductData().id().c_str());
    buf.append(':');
    buf.append_uint16(p.elementId());
    phc->Tag.set(buf.data());

    return phc;
}

bool SF_PhysicalConnector_Provider::ConstEnum::process (const solarflare::SystemElement& se)
{
    const solarflare::Port& p = static_cast<const solarflare::Port&>(se);
    SF_PhysicalConnector *phc = SF_PhysicalConnector::create(true);
    solarflare::VitalProductData vpd = p.nic()->vitalProductData();
    
    phc->InstanceID.set(solarflare::System::target.prefix());
    phc->InstanceID.value.append(":");
    phc->InstanceID.value.append(p.name());
    phc->CreationClassName.set("SF_NICCard");
    Buffer buf;
    buf.appends(vpd.id().c_str());
    buf.append(':');
    buf.append_uint16(p.elementId());
    phc->Tag.set(buf.data());
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
    handler->handle(phc);
    return false;
}

SF_PhysicalConnector_Provider::SF_PhysicalConnector_Provider()
{
}

SF_PhysicalConnector_Provider::~SF_PhysicalConnector_Provider()
{
}

Load_Status SF_PhysicalConnector_Provider::load()
{
    return LOAD_OK;
}

Unload_Status SF_PhysicalConnector_Provider::unload()
{
    return UNLOAD_OK;
}

Get_Instance_Status SF_PhysicalConnector_Provider::get_instance(
    const SF_PhysicalConnector* model,
    SF_PhysicalConnector*& instance)
{
    return GET_INSTANCE_UNSUPPORTED;
}

Enum_Instances_Status SF_PhysicalConnector_Provider::enum_instances(
    const SF_PhysicalConnector* model,
    Enum_Instances_Handler<SF_PhysicalConnector>* handler)
{
    ConstEnum connectors(handler);
    solarflare::System::target.forAllPorts(connectors);
    return ENUM_INSTANCES_OK;
}

Create_Instance_Status SF_PhysicalConnector_Provider::create_instance(
    SF_PhysicalConnector* instance)
{
    return CREATE_INSTANCE_UNSUPPORTED;
}

Delete_Instance_Status SF_PhysicalConnector_Provider::delete_instance(
    const SF_PhysicalConnector* instance)
{
    return DELETE_INSTANCE_UNSUPPORTED;
}

Modify_Instance_Status SF_PhysicalConnector_Provider::modify_instance(
    const SF_PhysicalConnector* model,
    const SF_PhysicalConnector* instance)
{
    return MODIFY_INSTANCE_UNSUPPORTED;
}

/*@END@*/

CIMPLE_NAMESPACE_END
