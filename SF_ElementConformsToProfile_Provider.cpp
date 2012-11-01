// Generated by genprov 2.0.24
#include "SF_ElementConformsToProfile_Provider.h"
#include "SF_SoftwareIdentity_Provider.h"
#include "SF_SoftwareInstallationService_Provider.h"
#include "SF_NICCard_Provider.h"
#include "SF_EthernetPort_Provider.h"
#include "SF_PhysicalConnector_Provider.h"
#include "SF_ConcreteJob_Provider.h"
#include "SF_DiagnosticTest_Provider.h"
#include "SF_RecordLog_Provider.h"
#include "SF_RegisteredProfile_Provider.h"

CIMPLE_NAMESPACE_BEGIN

const char * const SF_ElementConformsToProfile_Provider::implementationNamespace = "root/solarflare";
const char * const SF_ElementConformsToProfile_Provider::interopNamespace = "root/pg_interop";

SF_ElementConformsToProfile *SF_ElementConformsToProfile_Provider::makeLink(const SF_RegisteredProfile_Provider::DMTFProfileInfo &profile,
                                                                            Instance *obj)
{
    SF_ElementConformsToProfile *link = SF_ElementConformsToProfile::create(true);

    link->ConformantStandard = cast<CIM_RegisteredProfile *>(SF_RegisteredProfile_Provider::makeReference(profile));
    link->ConformantStandard->__name_space = interopNamespace;
    link->ManagedElement = cast<CIM_ManagedElement *>(obj);
    link->ManagedElement->__name_space = implementationNamespace;

    return link;
}

bool SF_ElementConformsToProfile_Provider::SWEnum::process(const solarflare::SystemElement &se)
{
    const solarflare::SWElement &sw = static_cast<const solarflare::SWElement &>(se);
    handler->handle(makeLink(SF_RegisteredProfile_Provider::SoftwareInventoryProfile, 
                             sw.cimReference(SF_SoftwareIdentity::static_meta_class)));
    
    switch (sw.classify())
    {
        case solarflare::SWElement::SWFirmware:
        case solarflare::SWElement::SWPackage:
            handler->handle(makeLink(SF_RegisteredProfile_Provider::SoftwareUpdateProfile, 
                                     sw.cimReference(SF_SoftwareInstallationService::static_meta_class)));
            if (const_cast<solarflare::SWElement&>(sw).installThread())
            {
                handler->handle(makeLink(SF_RegisteredProfile_Provider::JobControlProfile, 
                                         sw.cimReference(SF_ConcreteJob::static_meta_class)));
            }
            break;
        default:
            /* do nothing */
            break;
    }
    return true;
}

bool SF_ElementConformsToProfile_Provider::DiagEnum::process(const solarflare::SystemElement& se)
{
    const solarflare::Diagnostic &diag = static_cast<const solarflare::Diagnostic &>(se);
    handler->handle(makeLink(SF_RegisteredProfile_Provider::DiagnosticsProfile, 
                             diag.cimReference(SF_DiagnosticTest::static_meta_class)));
    if (const_cast<solarflare::Diagnostic&>(diag).asyncThread())
    {
        handler->handle(makeLink(SF_RegisteredProfile_Provider::JobControlProfile, 
                                 diag.cimReference(SF_DiagnosticTest::static_meta_class)));
    }
    return true;
}


bool SF_ElementConformsToProfile_Provider::NICEnum::process(const solarflare::SystemElement& se)
{
    const solarflare::NIC &nic = static_cast<const solarflare::NIC &>(se);
    handler->handle(makeLink(SF_RegisteredProfile_Provider::PhysicalAssetProfile, 
                             nic.cimReference(SF_NICCard::static_meta_class)));
    return true;
}


bool SF_ElementConformsToProfile_Provider::PortEnum::process(const solarflare::SystemElement& se)
{
    const solarflare::Port &port = static_cast<const solarflare::Port &>(se);
    handler->handle(makeLink(SF_RegisteredProfile_Provider::PhysicalAssetProfile, 
                             port.cimReference(SF_PhysicalConnector::static_meta_class)));
    return true;
}


bool SF_ElementConformsToProfile_Provider::IntfEnum::process(const solarflare::SystemElement& se)
{
    const solarflare::Interface &intf = static_cast<const solarflare::Interface &>(se);
    handler->handle(makeLink(SF_RegisteredProfile_Provider::EthernetPortProfile, 
                             intf.cimReference(SF_EthernetPort::static_meta_class)));
    handler->handle(makeLink(SF_RegisteredProfile_Provider::HostLANNetworkPortProfile, 
                             intf.cimReference(SF_EthernetPort::static_meta_class)));
    return true;
}


SF_ElementConformsToProfile_Provider::SF_ElementConformsToProfile_Provider()
{
}

SF_ElementConformsToProfile_Provider::~SF_ElementConformsToProfile_Provider()
{
}

Load_Status SF_ElementConformsToProfile_Provider::load()
{
    return LOAD_OK;
}

Unload_Status SF_ElementConformsToProfile_Provider::unload()
{
    return UNLOAD_OK;
}

Get_Instance_Status SF_ElementConformsToProfile_Provider::get_instance(
    const SF_ElementConformsToProfile* model,
    SF_ElementConformsToProfile*& instance)
{
    return GET_INSTANCE_UNSUPPORTED;
}

Enum_Instances_Status SF_ElementConformsToProfile_Provider::enum_instances(
    const SF_ElementConformsToProfile* model,
    Enum_Instances_Handler<SF_ElementConformsToProfile>* handler)
{
    SWEnum swenum(handler);
    NICEnum nicenum(handler);
    PortEnum portenum(handler);
    IntfEnum intfenum(handler);
    DiagEnum denum(handler);
    
    solarflare::System::target.forAllSoftware(swenum);
    solarflare::System::target.forAllNICs(nicenum);
    solarflare::System::target.forAllPorts(portenum);
    solarflare::System::target.forAllInterfaces(intfenum);
    solarflare::System::target.forAllDiagnostics(denum);

    for (unsigned i = 0; solarflare::Logger::knownLogs[i] != NULL; i++)
    {
        handler->handle(makeLink(SF_RegisteredProfile_Provider::RecordLogProfile,
                                 SF_RecordLog_Provider::makeReference(*solarflare::Logger::knownLogs[i])));
    }

    return ENUM_INSTANCES_OK;
}

Create_Instance_Status SF_ElementConformsToProfile_Provider::create_instance(
    SF_ElementConformsToProfile* instance)
{
    return CREATE_INSTANCE_UNSUPPORTED;
}

Delete_Instance_Status SF_ElementConformsToProfile_Provider::delete_instance(
    const SF_ElementConformsToProfile* instance)
{
    return DELETE_INSTANCE_UNSUPPORTED;
}

Modify_Instance_Status SF_ElementConformsToProfile_Provider::modify_instance(
    const SF_ElementConformsToProfile* model,
    const SF_ElementConformsToProfile* instance)
{
    return MODIFY_INSTANCE_UNSUPPORTED;
}

Enum_Associator_Names_Status SF_ElementConformsToProfile_Provider::enum_associator_names(
    const Instance* instance,
    const String& result_class,
    const String& role,
    const String& result_role,
    Enum_Associator_Names_Handler<Instance>* handler)
{
    return ENUM_ASSOCIATOR_NAMES_UNSUPPORTED;
}

Enum_Associators_Status SF_ElementConformsToProfile_Provider::enum_associators(
    const Instance* instance,
    const String& result_class,
    const String& role,
    const String& result_role,
    Enum_Associators_Handler<Instance>* handler)
{
    return ENUM_ASSOCIATORS_UNSUPPORTED;
}

Enum_References_Status SF_ElementConformsToProfile_Provider::enum_references(
    const Instance* instance,
    const SF_ElementConformsToProfile* model,
    const String& role,
    Enum_References_Handler<SF_ElementConformsToProfile>* handler)
{
    return ENUM_REFERENCES_UNSUPPORTED;
}

/*@END@*/

CIMPLE_NAMESPACE_END
