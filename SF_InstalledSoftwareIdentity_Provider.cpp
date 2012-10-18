// Generated by genprov 2.0.24
#include "SF_InstalledSoftwareIdentity_Provider.h"
#include "SF_SoftwareIdentity.h"
#include "SF_SoftwareIdentity_Provider.h"
#include "SF_ComputerSystem_Provider.h"

CIMPLE_NAMESPACE_BEGIN

SF_InstalledSoftwareIdentity_Provider::SF_InstalledSoftwareIdentity_Provider()
{
}

SF_InstalledSoftwareIdentity_Provider::~SF_InstalledSoftwareIdentity_Provider()
{
}

Load_Status SF_InstalledSoftwareIdentity_Provider::load()
{
    return LOAD_OK;
}

Unload_Status SF_InstalledSoftwareIdentity_Provider::unload()
{
    return UNLOAD_OK;
}

Get_Instance_Status SF_InstalledSoftwareIdentity_Provider::get_instance(
    const SF_InstalledSoftwareIdentity* model,
    SF_InstalledSoftwareIdentity*& instance)
{
    return GET_INSTANCE_UNSUPPORTED;
}

bool SF_InstalledSoftwareIdentity_Provider::ConstEnum::process(const solarflare::SWElement& sw)
{
    if (sw.version().isUnknown())
        return true;
    
    SF_InstalledSoftwareIdentity *link = SF_InstalledSoftwareIdentity::create(true);

    link->System = cast<CIM_System *>(from->clone());
    SF_SoftwareIdentity *sfid = SF_SoftwareIdentity_Provider::makeReference(sw);
    link->InstalledSoftware = cast<CIM_SoftwareIdentity *>(sfid);

    handler->handle(link);

    return true;
}

Enum_Instances_Status SF_InstalledSoftwareIdentity_Provider::enum_instances(
    const SF_InstalledSoftwareIdentity* model,
    Enum_Instances_Handler<SF_InstalledSoftwareIdentity>* handler)
{
    const CIM_ComputerSystem *cs = SF_ComputerSystem_Provider::findSystem();
    
    if (cs == NULL)
    {
        CIMPLE_ERR(("No ComputerSystem instance found"));
        return ENUM_INSTANCES_FAILED;
    }

    ConstEnum installed(cs, handler);
    solarflare::System::target.forAllSoftware(installed);

    return ENUM_INSTANCES_OK;
}

Create_Instance_Status SF_InstalledSoftwareIdentity_Provider::create_instance(
    SF_InstalledSoftwareIdentity* instance)
{
    return CREATE_INSTANCE_UNSUPPORTED;
}

Delete_Instance_Status SF_InstalledSoftwareIdentity_Provider::delete_instance(
    const SF_InstalledSoftwareIdentity* instance)
{
    return DELETE_INSTANCE_UNSUPPORTED;
}

Modify_Instance_Status SF_InstalledSoftwareIdentity_Provider::modify_instance(
    const SF_InstalledSoftwareIdentity* model,
    const SF_InstalledSoftwareIdentity* instance)
{
    return MODIFY_INSTANCE_UNSUPPORTED;
}

Enum_Associator_Names_Status SF_InstalledSoftwareIdentity_Provider::enum_associator_names(
    const Instance* instance,
    const String& result_class,
    const String& role,
    const String& result_role,
    Enum_Associator_Names_Handler<Instance>* handler)
{
    return ENUM_ASSOCIATOR_NAMES_UNSUPPORTED;
}

Enum_Associators_Status SF_InstalledSoftwareIdentity_Provider::enum_associators(
    const Instance* instance,
    const String& result_class,
    const String& role,
    const String& result_role,
    Enum_Associators_Handler<Instance>* handler)
{
    return ENUM_ASSOCIATORS_UNSUPPORTED;
}

Enum_References_Status SF_InstalledSoftwareIdentity_Provider::enum_references(
    const Instance* instance,
    const SF_InstalledSoftwareIdentity* model,
    const String& role,
    Enum_References_Handler<SF_InstalledSoftwareIdentity>* handler)
{
    return ENUM_REFERENCES_UNSUPPORTED;
}

/*@END@*/

CIMPLE_NAMESPACE_END
