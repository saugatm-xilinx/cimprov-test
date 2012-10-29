// Generated by genprov 2.0.24
#include "SF_ElementSoftwareIdentity_Provider.h"
#include "SF_SoftwareIdentity_Provider.h"
#include "SF_NICCard_Provider.h"
#include "SF_PortController_Provider.h"
#include "SF_DiagnosticTest_Provider.h"
#include "sf_platform.h"

CIMPLE_NAMESPACE_BEGIN

bool SF_ElementSoftwareIdentity_Provider::NICBinder::process(const solarflare::SystemElement& se)
{
    const solarflare::NIC &nic = static_cast<const solarflare::NIC &>(se);
    SF_ElementSoftwareIdentity *item = SF_ElementSoftwareIdentity::create(true);
    
    item->Antecedent = cast<CIM_SoftwareIdentity *>(SF_SoftwareIdentity_Provider::makeReference(*softItem));
    item->Dependent = cast<CIM_ManagedElement *>(SF_PortController_Provider::makeReference(nic));
    item->ElementSoftwareStatus.null = false;
    item->ElementSoftwareStatus.value.append(SF_ElementSoftwareIdentity::_ElementSoftwareStatus::enum_Current);
    item->ElementSoftwareStatus.value.append(SF_ElementSoftwareIdentity::_ElementSoftwareStatus::enum_Next);
    item->ElementSoftwareStatus.value.append(SF_ElementSoftwareIdentity::_ElementSoftwareStatus::enum_Default);

    handler->handle(item);
    return true;
}

bool SF_ElementSoftwareIdentity_Provider::DiagEnum::process(const solarflare::SystemElement& se)
{
    const solarflare::Diagnostic& diag = static_cast<const solarflare::Diagnostic&>(se);
    const solarflare::SWElement *tool = diag.diagnosticTool();
    
    if (tool != NULL)
    {
        SF_ElementSoftwareIdentity *item = SF_ElementSoftwareIdentity::create(true);
    
        item->Antecedent = cast<CIM_SoftwareIdentity *>(SF_SoftwareIdentity_Provider::makeReference(*tool));
        item->Dependent = cast<CIM_ManagedElement *>(SF_DiagnosticTest_Provider::makeReference(diag));
        item->ElementSoftwareStatus.null = false;
        item->ElementSoftwareStatus.value.append(SF_ElementSoftwareIdentity::_ElementSoftwareStatus::enum_Current);
        item->ElementSoftwareStatus.value.append(SF_ElementSoftwareIdentity::_ElementSoftwareStatus::enum_Next);
        item->ElementSoftwareStatus.value.append(SF_ElementSoftwareIdentity::_ElementSoftwareStatus::enum_Default);

        handler->handle(item);
    }
    return true;
}

bool SF_ElementSoftwareIdentity_Provider::SWEnum::process(const solarflare::SystemElement& el)
{
    const solarflare::SWElement& se = static_cast<const solarflare::SWElement &>(el);
    switch (se.classify())
    {
        case solarflare::SWElement::SWFirmware:
        {
            const solarflare::Firmware& fw = static_cast<const solarflare::Firmware&>(se);
            SF_ElementSoftwareIdentity *item = SF_ElementSoftwareIdentity::create(true);
            
            item->Antecedent = cast<CIM_SoftwareIdentity *>(SF_SoftwareIdentity_Provider::makeReference(fw));
            item->Dependent = cast<CIM_ManagedElement *>(fw.nic()->cimReference(SF_NICCard::static_meta_class));
            item->ElementSoftwareStatus.null = false;
            item->ElementSoftwareStatus.value.append(SF_ElementSoftwareIdentity::_ElementSoftwareStatus::enum_Current);
            item->ElementSoftwareStatus.value.append(SF_ElementSoftwareIdentity::_ElementSoftwareStatus::enum_Next);
            item->ElementSoftwareStatus.value.append(SF_ElementSoftwareIdentity::_ElementSoftwareStatus::enum_Default);
            item->ElementSoftwareStatus.value.append(SF_ElementSoftwareIdentity::_ElementSoftwareStatus::enum_Installed);
            handler->handle(item);
            break;
        }
        case solarflare::SWElement::SWDriver:
        {
            NICBinder bindToNIC(handler, &se);
            solarflare::System::target.forAllNICs(bindToNIC);
            break;
        }
        default:
            /* do nothing */
            break;
    }
    return true;
}

SF_ElementSoftwareIdentity_Provider::SF_ElementSoftwareIdentity_Provider()
{
}

SF_ElementSoftwareIdentity_Provider::~SF_ElementSoftwareIdentity_Provider()
{
}

Load_Status SF_ElementSoftwareIdentity_Provider::load()
{
    return LOAD_OK;
}

Unload_Status SF_ElementSoftwareIdentity_Provider::unload()
{
    return UNLOAD_OK;
}

Get_Instance_Status SF_ElementSoftwareIdentity_Provider::get_instance(
    const SF_ElementSoftwareIdentity* model,
    SF_ElementSoftwareIdentity*& instance)
{
    return GET_INSTANCE_UNSUPPORTED;
}

Enum_Instances_Status SF_ElementSoftwareIdentity_Provider::enum_instances(
    const SF_ElementSoftwareIdentity* model,
    Enum_Instances_Handler<SF_ElementSoftwareIdentity>* handler)
{
    SWEnum swinstances(handler);
    DiagEnum dinstances(handler);
    solarflare::System::target.forAllSoftware(swinstances);
    solarflare::System::target.forAllDiagnostics(dinstances);
    return ENUM_INSTANCES_OK;
}

Create_Instance_Status SF_ElementSoftwareIdentity_Provider::create_instance(
    SF_ElementSoftwareIdentity* instance)
{
    return CREATE_INSTANCE_UNSUPPORTED;
}

Delete_Instance_Status SF_ElementSoftwareIdentity_Provider::delete_instance(
    const SF_ElementSoftwareIdentity* instance)
{
    return DELETE_INSTANCE_UNSUPPORTED;
}

Modify_Instance_Status SF_ElementSoftwareIdentity_Provider::modify_instance(
    const SF_ElementSoftwareIdentity* model,
    const SF_ElementSoftwareIdentity* instance)
{
    return MODIFY_INSTANCE_UNSUPPORTED;
}

Enum_Associator_Names_Status SF_ElementSoftwareIdentity_Provider::enum_associator_names(
    const Instance* instance,
    const String& result_class,
    const String& role,
    const String& result_role,
    Enum_Associator_Names_Handler<Instance>* handler)
{
    return ENUM_ASSOCIATOR_NAMES_UNSUPPORTED;
}

Enum_Associators_Status SF_ElementSoftwareIdentity_Provider::enum_associators(
    const Instance* instance,
    const String& result_class,
    const String& role,
    const String& result_role,
    Enum_Associators_Handler<Instance>* handler)
{
    return ENUM_ASSOCIATORS_UNSUPPORTED;
}

Enum_References_Status SF_ElementSoftwareIdentity_Provider::enum_references(
    const Instance* instance,
    const SF_ElementSoftwareIdentity* model,
    const String& role,
    Enum_References_Handler<SF_ElementSoftwareIdentity>* handler)
{
    return ENUM_REFERENCES_UNSUPPORTED;
}

/*@END@*/

CIMPLE_NAMESPACE_END
