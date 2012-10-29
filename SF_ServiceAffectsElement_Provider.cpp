// Generated by genprov 2.0.24
#include "SF_ServiceAffectsElement_Provider.h"
#include "SF_SoftwareInstallationService_Provider.h"
#include "SF_ComputerSystem_Provider.h"
#include "SF_SoftwareIdentity_Provider.h"
#include "SF_PortController_Provider.h"
#include "SF_NICCard_Provider.h"
#include "SF_DiagnosticTest_Provider.h"

CIMPLE_NAMESPACE_BEGIN

bool SF_ServiceAffectsElement_Provider::SWEnum::process(const solarflare::SystemElement& se)
{
    const solarflare::SWElement& sw = static_cast<const solarflare::SWElement&>(se);
    
    SF_ServiceAffectsElement *swlink = NULL;
    SF_ServiceAffectsElement *hwlink = NULL;
    switch (sw.classify())
    {
        case solarflare::SWElement::SWFirmware:
        {
            const solarflare::Firmware& fw = static_cast<const solarflare::Firmware&>(sw);
            
            hwlink = SF_ServiceAffectsElement::create(true);
            hwlink->AffectedElement = cast<CIM_ManagedElement *>(fw.nic()->cimReference(SF_NICCard::static_meta_class));
            hwlink->AffectingElement = cast<CIM_Service *>(SF_SoftwareInstallationService_Provider::makeReference(sw));
            handler->handle(hwlink);

            swlink = SF_ServiceAffectsElement::create(true);
            swlink->AffectedElement = cast<CIM_ManagedElement *>(SF_SoftwareIdentity_Provider::makeReference(sw));
            swlink->AffectingElement = cast<CIM_Service *>(SF_SoftwareInstallationService_Provider::makeReference(sw));
            handler->handle(swlink);

            break;
        }
        case solarflare::SWElement::SWPackage:
        {
            hwlink = SF_ServiceAffectsElement::create(true);
            hwlink->AffectedElement = cast<CIM_ManagedElement *>(SF_ComputerSystem_Provider::findSystem()->clone());
            hwlink->AffectingElement = cast<CIM_Service *>(SF_SoftwareInstallationService_Provider::makeReference(sw));
            handler->handle(hwlink);

            swlink = SF_ServiceAffectsElement::create(true);
            swlink->AffectedElement = cast<CIM_ManagedElement *>(SF_SoftwareIdentity_Provider::makeReference(sw));
            swlink->AffectingElement = cast<CIM_Service *>(SF_SoftwareInstallationService_Provider::makeReference(sw));
            handler->handle(swlink);

            break;
        }
        default:
        {
            if (sw.isHostSw())
            {
                const solarflare::HostSWElement& hsw = static_cast<const solarflare::HostSWElement&>(sw);

                swlink = SF_ServiceAffectsElement::create(true);
                swlink->AffectedElement = cast<CIM_ManagedElement *>(SF_SoftwareIdentity_Provider::makeReference(sw));
                swlink->AffectingElement = cast<CIM_Service *>(SF_SoftwareInstallationService_Provider::makeReference(*hsw.package()));
                handler->handle(swlink);
            }
            break;
        }
    }
    return true;
}

bool SF_ServiceAffectsElement_Provider::DiagEnum::process(const solarflare::SystemElement& se)
{
    const solarflare::Diagnostic& diag = static_cast<const solarflare::Diagnostic&>(se);
    SF_ServiceAffectsElement *link = SF_ServiceAffectsElement::create(true);

    link->AffectedElement = cast<CIM_ManagedElement *>(SF_PortController_Provider::makeReference(*diag.nic()));
    link->AffectingElement = cast<CIM_Service *>(SF_DiagnosticTest_Provider::makeReference(diag));

    handler->handle(link);

    return true;
}

SF_ServiceAffectsElement_Provider::SF_ServiceAffectsElement_Provider()
{
}

SF_ServiceAffectsElement_Provider::~SF_ServiceAffectsElement_Provider()
{
}

Load_Status SF_ServiceAffectsElement_Provider::load()
{
    solarflare::System::target.initialize();
    return LOAD_OK;
}

Unload_Status SF_ServiceAffectsElement_Provider::unload()
{
    return UNLOAD_OK;
}

Get_Instance_Status SF_ServiceAffectsElement_Provider::get_instance(
    const SF_ServiceAffectsElement* model,
    SF_ServiceAffectsElement*& instance)
{
    return GET_INSTANCE_UNSUPPORTED;
}

Enum_Instances_Status SF_ServiceAffectsElement_Provider::enum_instances(
    const SF_ServiceAffectsElement* model,
    Enum_Instances_Handler<SF_ServiceAffectsElement>* handler)
{
    SWEnum sweffects(handler);
    DiagEnum deffects(handler);
    solarflare::System::target.forAllSoftware(sweffects);
    solarflare::System::target.forAllDiagnostics(deffects);

    return ENUM_INSTANCES_OK;
}

Create_Instance_Status SF_ServiceAffectsElement_Provider::create_instance(
    SF_ServiceAffectsElement* instance)
{
    return CREATE_INSTANCE_UNSUPPORTED;
}

Delete_Instance_Status SF_ServiceAffectsElement_Provider::delete_instance(
    const SF_ServiceAffectsElement* instance)
{
    return DELETE_INSTANCE_UNSUPPORTED;
}

Modify_Instance_Status SF_ServiceAffectsElement_Provider::modify_instance(
    const SF_ServiceAffectsElement* model,
    const SF_ServiceAffectsElement* instance)
{
    return MODIFY_INSTANCE_UNSUPPORTED;
}

Enum_Associator_Names_Status SF_ServiceAffectsElement_Provider::enum_associator_names(
    const Instance* instance,
    const String& result_class,
    const String& role,
    const String& result_role,
    Enum_Associator_Names_Handler<Instance>* handler)
{
    return ENUM_ASSOCIATOR_NAMES_UNSUPPORTED;
}

Enum_Associators_Status SF_ServiceAffectsElement_Provider::enum_associators(
    const Instance* instance,
    const String& result_class,
    const String& role,
    const String& result_role,
    Enum_Associators_Handler<Instance>* handler)
{
    return ENUM_ASSOCIATORS_UNSUPPORTED;
}

Enum_References_Status SF_ServiceAffectsElement_Provider::enum_references(
    const Instance* instance,
    const SF_ServiceAffectsElement* model,
    const String& role,
    Enum_References_Handler<SF_ServiceAffectsElement>* handler)
{
    return ENUM_REFERENCES_UNSUPPORTED;
}

/*@END@*/

CIMPLE_NAMESPACE_END
