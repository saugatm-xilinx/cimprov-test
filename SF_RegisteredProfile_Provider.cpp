// Generated by genprov 2.0.24
#include "SF_RegisteredProfile_Provider.h"
#include "sf_platform.h"

CIMPLE_NAMESPACE_BEGIN

const SF_RegisteredProfile_Provider::DMTFProfileInfo SF_RegisteredProfile_Provider::ProfileRegistrationProfile =
{"Profile Registration", "1.0.0", NULL};
const SF_RegisteredProfile_Provider::DMTFProfileInfo *const SF_RegisteredProfile_Provider::genericPrpRef[] = 
{&ProfileRegistrationProfile, NULL};
const SF_RegisteredProfile_Provider::DMTFProfileInfo SF_RegisteredProfile_Provider::DiagnosticsProfile =
{"Diagnostics", "2.0.0", genericPrpRef};
const SF_RegisteredProfile_Provider::DMTFProfileInfo SF_RegisteredProfile_Provider::RecordLogProfile =
{"Record Log", "2.0.0", genericPrpRef};
const SF_RegisteredProfile_Provider::DMTFProfileInfo SF_RegisteredProfile_Provider::PhysicalAssetProfile =
{"Physical Asset", "1.0.2", genericPrpRef};
const SF_RegisteredProfile_Provider::DMTFProfileInfo *const SF_RegisteredProfile_Provider::hostLanPortRef[] = 
{&ProfileRegistrationProfile, &PhysicalAssetProfile, NULL};
const SF_RegisteredProfile_Provider::DMTFProfileInfo SF_RegisteredProfile_Provider::HostLANNetworkPortProfile =
{"Host LAN Network Port", "1.0.2", hostLanPortRef};
const SF_RegisteredProfile_Provider::DMTFProfileInfo *const SF_RegisteredProfile_Provider::ethernetPortRef[] = 
{&ProfileRegistrationProfile, &HostLANNetworkPortProfile, NULL};
const SF_RegisteredProfile_Provider::DMTFProfileInfo SF_RegisteredProfile_Provider::EthernetPortProfile = 
{"Ethernet Port", "1.0.1", ethernetPortRef};
const SF_RegisteredProfile_Provider::DMTFProfileInfo SF_RegisteredProfile_Provider::SoftwareInventoryProfile =
{"Software Inventory", "1.0.1", genericPrpRef};
const SF_RegisteredProfile_Provider::DMTFProfileInfo *const SF_RegisteredProfile_Provider::softwareUpdateRef[] = 
{&ProfileRegistrationProfile, &SoftwareInventoryProfile, NULL};
const SF_RegisteredProfile_Provider::DMTFProfileInfo SF_RegisteredProfile_Provider::SoftwareUpdateProfile =
{"Software Update", "1.0.0", softwareUpdateRef};
const SF_RegisteredProfile_Provider::DMTFProfileInfo SF_RegisteredProfile_Provider::JobControlProfile = 
{"Job Control", "1.0.0", genericPrpRef};

const SF_RegisteredProfile_Provider::DMTFProfileInfo * const
SF_RegisteredProfile_Provider::knownDMTFProfiles[] = 
{
    &ProfileRegistrationProfile,
    &DiagnosticsProfile,
    &RecordLogProfile,
    &PhysicalAssetProfile,
    &EthernetPortProfile,
    &SoftwareInventoryProfile,
    &SoftwareUpdateProfile,
    &HostLANNetworkPortProfile,
    &JobControlProfile,
    NULL
};

SF_RegisteredProfile *SF_RegisteredProfile_Provider::makeReference(const DMTFProfileInfo& profile)
{
    SF_RegisteredProfile *newProf = SF_RegisteredProfile::create(true);
    
    newProf->InstanceID.set(solarflare::System::target.prefix());
    newProf->InstanceID.value.append(":");
    newProf->InstanceID.value.append("DMTF+");
    newProf->InstanceID.value.append(profile.name);
    newProf->InstanceID.value.append("+");
    newProf->InstanceID.value.append(profile.version);
    return newProf;
}

SF_RegisteredProfile_Provider::SF_RegisteredProfile_Provider()
{
}

SF_RegisteredProfile_Provider::~SF_RegisteredProfile_Provider()
{
}

Load_Status SF_RegisteredProfile_Provider::load()
{
    return LOAD_OK;
}

Unload_Status SF_RegisteredProfile_Provider::unload()
{
    return UNLOAD_OK;
}

Get_Instance_Status SF_RegisteredProfile_Provider::get_instance(
    const SF_RegisteredProfile* model,
    SF_RegisteredProfile*& instance)
{
    return GET_INSTANCE_UNSUPPORTED;
}

Enum_Instances_Status SF_RegisteredProfile_Provider::enum_instances(
    const SF_RegisteredProfile* model,
    Enum_Instances_Handler<SF_RegisteredProfile>* handler)
{
    for (unsigned i = 0; knownDMTFProfiles[i] != NULL; i++)
    {
        SF_RegisteredProfile *prof = makeReference(*knownDMTFProfiles[i]);
        prof->RegisteredOrganization.null = false;
        prof->RegisteredOrganization.value = SF_RegisteredProfile::_RegisteredOrganization::enum_DMTF;
        prof->RegisteredName.set(knownDMTFProfiles[i]->name);
        prof->RegisteredVersion.set(knownDMTFProfiles[i]->version);
        prof->AdvertiseTypes.null = false;
        prof->AdvertiseTypes.value.append(SF_RegisteredProfile::_AdvertiseTypes::enum_SLP);
        handler->handle(prof);
    }
    return ENUM_INSTANCES_OK;
}

Create_Instance_Status SF_RegisteredProfile_Provider::create_instance(
    SF_RegisteredProfile* instance)
{
    return CREATE_INSTANCE_UNSUPPORTED;
}

Delete_Instance_Status SF_RegisteredProfile_Provider::delete_instance(
    const SF_RegisteredProfile* instance)
{
    return DELETE_INSTANCE_UNSUPPORTED;
}

Modify_Instance_Status SF_RegisteredProfile_Provider::modify_instance(
    const SF_RegisteredProfile* model,
    const SF_RegisteredProfile* instance)
{
    return MODIFY_INSTANCE_UNSUPPORTED;
}

Invoke_Method_Status SF_RegisteredProfile_Provider::CloseConformantInstances(
    const SF_RegisteredProfile* self,
    const Property<String>& EnumerationContext,
    Property<uint32>& return_value)
{
    return INVOKE_METHOD_UNSUPPORTED;
}

Invoke_Method_Status SF_RegisteredProfile_Provider::OpenConformantInstances(
    const SF_RegisteredProfile* self,
    const Property<String>& ResultClass,
    const Property<Array_String>& IncludedPropertyList,
    const Property<uint32>& OperationTimeout,
    const Property<boolean>& ContinueOnError,
    const Property<uint32>& MaxObjectCount,
    Property<String>& EnumerationContext,
    Property<boolean>& EndOfSequence,
    Property<Array_uint16>& InstanceType,
    Property< Array<CIM_ManagedElement*> >& InstanceWithPathList,
    Property<uint32>& return_value)
{
    return INVOKE_METHOD_UNSUPPORTED;
}

Invoke_Method_Status SF_RegisteredProfile_Provider::PullConformantInstances(
    const SF_RegisteredProfile* self,
    const Property<uint32>& MaxObjectCount,
    Property<String>& EnumerationContext,
    Property<boolean>& EndOfSequence,
    Property<Array_uint16>& InstanceType,
    Property< Array<CIM_ManagedElement*> >& InstanceWithPathList,
    Property<uint32>& return_value)
{
    return INVOKE_METHOD_UNSUPPORTED;
}

/*@END@*/

CIMPLE_NAMESPACE_END
