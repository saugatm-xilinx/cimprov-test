// Generated by genprov 2.0.24
#include "SF_BundleComponent_Provider.h"
#include "SF_SoftwareIdentity_Provider.h"
#include "sf_platform.h"

CIMPLE_NAMESPACE_BEGIN

bool SF_BundleComponent_Provider::ConstEnum::process(const solarflare::SWElement& e)
{
    if (e.isHostSw())
    {
        const solarflare::HostSWElement& he = static_cast<const solarflare::HostSWElement&>(e);
        
        SF_BundleComponent *link = SF_BundleComponent::create(true);
        link->GroupComponent = 
        cast<CIM_ManagedElement *>(SF_SoftwareIdentity_Provider::makeReference(*he.package()));
        link->PartComponent =
        cast<CIM_ManagedElement *>(SF_SoftwareIdentity_Provider::makeReference(e));
        if (he.package() != master)
        {
            master = he.package();
            idx = 0;
        }
        else
        {
            idx++;
        }
        link->AssignedSequence.set(idx);
        handler->handle(link);
    }
    return true;
}

SF_BundleComponent_Provider::SF_BundleComponent_Provider()
{
}

SF_BundleComponent_Provider::~SF_BundleComponent_Provider()
{
}

Load_Status SF_BundleComponent_Provider::load()
{
    return LOAD_OK;
}

Unload_Status SF_BundleComponent_Provider::unload()
{
    return UNLOAD_OK;
}

Get_Instance_Status SF_BundleComponent_Provider::get_instance(
    const SF_BundleComponent* model,
    SF_BundleComponent*& instance)
{
    return GET_INSTANCE_UNSUPPORTED;
}

Enum_Instances_Status SF_BundleComponent_Provider::enum_instances(
    const SF_BundleComponent* model,
    Enum_Instances_Handler<SF_BundleComponent>* handler)
{
    ConstEnum processor(handler);
    
    solarflare::System::target.forAllSoftware(processor);
    
    return ENUM_INSTANCES_OK;
}

Create_Instance_Status SF_BundleComponent_Provider::create_instance(
    SF_BundleComponent* instance)
{
    return CREATE_INSTANCE_UNSUPPORTED;
}

Delete_Instance_Status SF_BundleComponent_Provider::delete_instance(
    const SF_BundleComponent* instance)
{
    return DELETE_INSTANCE_UNSUPPORTED;
}

Modify_Instance_Status SF_BundleComponent_Provider::modify_instance(
    const SF_BundleComponent* model,
    const SF_BundleComponent* instance)
{
    return MODIFY_INSTANCE_UNSUPPORTED;
}

Enum_Associator_Names_Status SF_BundleComponent_Provider::enum_associator_names(
    const Instance* instance,
    const String& result_class,
    const String& role,
    const String& result_role,
    Enum_Associator_Names_Handler<Instance>* handler)
{
    return ENUM_ASSOCIATOR_NAMES_UNSUPPORTED;
}

Enum_Associators_Status SF_BundleComponent_Provider::enum_associators(
    const Instance* instance,
    const String& result_class,
    const String& role,
    const String& result_role,
    Enum_Associators_Handler<Instance>* handler)
{
    return ENUM_ASSOCIATORS_UNSUPPORTED;
}

Enum_References_Status SF_BundleComponent_Provider::enum_references(
    const Instance* instance,
    const SF_BundleComponent* model,
    const String& role,
    Enum_References_Handler<SF_BundleComponent>* handler)
{
    return ENUM_REFERENCES_UNSUPPORTED;
}

/*@END@*/

CIMPLE_NAMESPACE_END
