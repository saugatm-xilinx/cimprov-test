// Generated by genprov 2.0.24
#include "SF_ElementCapabilities_Provider.h"
#include "SF_SoftwareInstallationServiceCapabilities_Provider.h"
#include "SF_SoftwareInstallationService_Provider.h"
#include "SF_EthernetPort_Provider.h"
#include "SF_PortController_Provider.h"
#include "SF_LANEndpoint_Provider.h"
#include "SF_EnabledLogicalElementCapabilities_Provider.h"
#include "SF_DiagnosticServiceCapabilities_Provider.h"
#include "SF_DiagnosticTest_Provider.h"

CIMPLE_NAMESPACE_BEGIN

bool SF_ElementCapabilities_Provider::NICEnum::process(const solarflare::SystemElement& se)
{
    const solarflare::NIC& nic = static_cast<const solarflare::NIC&>(se);
    SF_PortController *pc = static_cast<SF_PortController *>(nic.cimReference(SF_PortController::static_meta_class));
    SF_EnabledLogicalElementCapabilities *caps = SF_EnabledLogicalElementCapabilities_Provider::makeReference(nic, "Controller");
    SF_ElementCapabilities *link = SF_ElementCapabilities::create(true);

    link->ManagedElement = cast<CIM_ManagedElement *>(pc);
    link->Capabilities = cast<CIM_Capabilities *>(caps);
    link->Characteristics.null = false;
    link->Characteristics.value.append(SF_ElementCapabilities::_Characteristics::enum_Default);
    link->Characteristics.value.append(SF_ElementCapabilities::_Characteristics::enum_Current);
    handler->handle(link);
    return true;
}

bool SF_ElementCapabilities_Provider::DiagEnum::process(const solarflare::SystemElement& se)
{
    const solarflare::Diagnostic& diag = static_cast<const solarflare::Diagnostic&>(se);
    SF_DiagnosticTest *test = static_cast<SF_DiagnosticTest *>(diag.cimReference(SF_DiagnosticTest::static_meta_class));
    SF_DiagnosticServiceCapabilities *caps = SF_DiagnosticServiceCapabilities_Provider::makeReference(diag);
    SF_ElementCapabilities *link = SF_ElementCapabilities::create(true);

    link->ManagedElement = cast<CIM_ManagedElement *>(test);
    link->Capabilities = cast<CIM_Capabilities *>(caps);
    link->Characteristics.null = false;
    link->Characteristics.value.append(SF_ElementCapabilities::_Characteristics::enum_Default);
    link->Characteristics.value.append(SF_ElementCapabilities::_Characteristics::enum_Current);
    handler->handle(link);
    return true;
}


bool SF_ElementCapabilities_Provider::IntfEnum::process(const solarflare::SystemElement& se)
{
    const solarflare::Interface& nic = static_cast<const solarflare::Interface&>(se);

    SF_EthernetPort *port = static_cast<SF_EthernetPort *>(nic.cimReference(SF_EthernetPort::static_meta_class));
    SF_LANEndpoint *endpoint = static_cast<SF_LANEndpoint *>(nic.cimReference(SF_LANEndpoint::static_meta_class));
    SF_EnabledLogicalElementCapabilities *pcaps = SF_EnabledLogicalElementCapabilities_Provider::makeReference(nic, "Port");
    SF_EnabledLogicalElementCapabilities *epcaps = SF_EnabledLogicalElementCapabilities_Provider::makeReference(nic, "Endpoint");
    SF_ElementCapabilities *plink = SF_ElementCapabilities::create(true);
    SF_ElementCapabilities *eplink = SF_ElementCapabilities::create(true);

    plink->ManagedElement = cast<CIM_ManagedElement *>(port);
    plink->Capabilities = cast<CIM_Capabilities *>(pcaps);
    plink->Characteristics.null = false;
    plink->Characteristics.value.append(SF_ElementCapabilities::_Characteristics::enum_Default);
    plink->Characteristics.value.append(SF_ElementCapabilities::_Characteristics::enum_Current);
    handler->handle(plink);
    eplink->ManagedElement = cast<CIM_ManagedElement *>(endpoint);
    eplink->Capabilities = cast<CIM_Capabilities *>(epcaps);
    eplink->Characteristics.null = false;
    eplink->Characteristics.value.append(SF_ElementCapabilities::_Characteristics::enum_Default);
    eplink->Characteristics.value.append(SF_ElementCapabilities::_Characteristics::enum_Current);
    handler->handle(eplink);
    return true;
}

bool SF_ElementCapabilities_Provider::SWEnum::process(const solarflare::SystemElement& se)
{
    const solarflare::SWElement& sw = static_cast<const solarflare::SWElement&>(se);
    switch (sw.classify())
    {
        case solarflare::SWElement::SWFirmware:
        case solarflare::SWElement::SWPackage:
            /* acceptable, do nothing here */
            break;
        default:
            return true;
    }
    SF_SoftwareInstallationService *svc = 
    static_cast<SF_SoftwareInstallationService *>(sw.cimReference(SF_SoftwareInstallationService::static_meta_class));
    SF_SoftwareInstallationServiceCapabilities *caps = SF_SoftwareInstallationServiceCapabilities_Provider::makeReference(sw);
    SF_ElementCapabilities *link = SF_ElementCapabilities::create(true);
    
    link->ManagedElement = cast<CIM_ManagedElement *>(svc);
    link->Capabilities = cast<CIM_Capabilities *>(caps);
    link->Characteristics.null = false;
    link->Characteristics.value.append(SF_ElementCapabilities::_Characteristics::enum_Default);
    link->Characteristics.value.append(SF_ElementCapabilities::_Characteristics::enum_Current);
    handler->handle(link);
    return true;
}

SF_ElementCapabilities_Provider::SF_ElementCapabilities_Provider()
{
}

SF_ElementCapabilities_Provider::~SF_ElementCapabilities_Provider()
{
}

Load_Status SF_ElementCapabilities_Provider::load()
{
    solarflare::System::target.initialize();
    return LOAD_OK;
}

Unload_Status SF_ElementCapabilities_Provider::unload()
{
    return UNLOAD_OK;
}

Get_Instance_Status SF_ElementCapabilities_Provider::get_instance(
    const SF_ElementCapabilities* model,
    SF_ElementCapabilities*& instance)
{
    return GET_INSTANCE_UNSUPPORTED;
}

Enum_Instances_Status SF_ElementCapabilities_Provider::enum_instances(
    const SF_ElementCapabilities* model,
    Enum_Instances_Handler<SF_ElementCapabilities>* handler)
{
    NICEnum niclinks(handler);
    IntfEnum intflinks(handler);
    SWEnum swlinks(handler);
    DiagEnum dlinks(handler);
    
    solarflare::System::target.forAllNICs(niclinks);
    solarflare::System::target.forAllInterfaces(intflinks);
    solarflare::System::target.forAllSoftware(swlinks);
    solarflare::System::target.forAllDiagnostics(dlinks);
    return ENUM_INSTANCES_OK;
}

Create_Instance_Status SF_ElementCapabilities_Provider::create_instance(
    SF_ElementCapabilities* instance)
{
    return CREATE_INSTANCE_UNSUPPORTED;
}

Delete_Instance_Status SF_ElementCapabilities_Provider::delete_instance(
    const SF_ElementCapabilities* instance)
{
    return DELETE_INSTANCE_UNSUPPORTED;
}

Modify_Instance_Status SF_ElementCapabilities_Provider::modify_instance(
    const SF_ElementCapabilities* model,
    const SF_ElementCapabilities* instance)
{
    return MODIFY_INSTANCE_UNSUPPORTED;
}

Enum_Associator_Names_Status SF_ElementCapabilities_Provider::enum_associator_names(
    const Instance* instance,
    const String& result_class,
    const String& role,
    const String& result_role,
    Enum_Associator_Names_Handler<Instance>* handler)
{
    return ENUM_ASSOCIATOR_NAMES_UNSUPPORTED;
}

Enum_Associators_Status SF_ElementCapabilities_Provider::enum_associators(
    const Instance* instance,
    const String& result_class,
    const String& role,
    const String& result_role,
    Enum_Associators_Handler<Instance>* handler)
{
    return ENUM_ASSOCIATORS_UNSUPPORTED;
}

Enum_References_Status SF_ElementCapabilities_Provider::enum_references(
    const Instance* instance,
    const SF_ElementCapabilities* model,
    const String& role,
    Enum_References_Handler<SF_ElementCapabilities>* handler)
{
    return ENUM_REFERENCES_UNSUPPORTED;
}

/*@END@*/

CIMPLE_NAMESPACE_END