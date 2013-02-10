/*NOCHKSRC*/
//==============================================================================
//
// PLEASE DO NOT EDIT; THIS FILE WAS AUTOMATICALLY GENERATED BY GENCLASS 2.0.24
//
//==============================================================================

#include <cimple/Meta_Class.h>
#include <cimple/Meta_Property.h>
#include <cimple/Meta_Reference.h>
#include "SF_LANEndpoint.h"

CIMPLE_NAMESPACE_BEGIN

using namespace cimple;

extern const Meta_Repository __meta_repository_52E780FCF95E157C8B11653275F6420A;

/*[1568]*/
extern const Meta_Property
_CIM_ManagedElement_InstanceID;

/*[1568]*/
extern const Meta_Property
_CIM_ManagedElement_Caption;

/*[1568]*/
extern const Meta_Property
_CIM_ProtocolEndpoint_Description;

/*[1568]*/
extern const Meta_Property
_CIM_ManagedElement_ElementName;

/*[1568]*/
extern const Meta_Property
_CIM_ManagedSystemElement_InstallDate;

/*[1568]*/
extern const Meta_Property
_CIM_ProtocolEndpoint_Name;

/*[1568]*/
extern const Meta_Property
_CIM_ProtocolEndpoint_OperationalStatus;

/*[1568]*/
extern const Meta_Property
_CIM_ManagedSystemElement_StatusDescriptions;

/*[1568]*/
extern const Meta_Property
_CIM_ManagedSystemElement_Status;

/*[1568]*/
extern const Meta_Property
_CIM_ManagedSystemElement_HealthState;

/*[1568]*/
extern const Meta_Property
_CIM_ManagedSystemElement_CommunicationStatus;

/*[1568]*/
extern const Meta_Property
_CIM_ManagedSystemElement_DetailedStatus;

/*[1568]*/
extern const Meta_Property
_CIM_ManagedSystemElement_OperatingStatus;

/*[1568]*/
extern const Meta_Property
_CIM_ManagedSystemElement_PrimaryStatus;

/*[1568]*/
extern const Meta_Property
_CIM_ProtocolEndpoint_EnabledState;

/*[1568]*/
extern const Meta_Property
_CIM_EnabledLogicalElement_OtherEnabledState;

/*[1568]*/
extern const Meta_Property
_CIM_EnabledLogicalElement_RequestedState;

/*[1568]*/
extern const Meta_Property
_CIM_EnabledLogicalElement_EnabledDefault;

/*[1568]*/
extern const Meta_Property
_CIM_ProtocolEndpoint_TimeOfLastStateChange;

/*[1568]*/
extern const Meta_Property
_CIM_EnabledLogicalElement_AvailableRequestedStates;

/*[1568]*/
extern const Meta_Property
_CIM_EnabledLogicalElement_TransitioningToState;

/*[1568]*/
extern const Meta_Property
_CIM_ServiceAccessPoint_SystemCreationClassName;

/*[1568]*/
extern const Meta_Property
_CIM_ServiceAccessPoint_SystemName;

/*[1568]*/
extern const Meta_Property
_CIM_ServiceAccessPoint_CreationClassName;

/*[1568]*/
extern const Meta_Property
_CIM_ProtocolEndpoint_NameFormat;

/*[1568]*/
extern const Meta_Property
_CIM_ProtocolEndpoint_ProtocolType;

/*[1568]*/
extern const Meta_Property
_CIM_ProtocolEndpoint_ProtocolIFType;

/*[1568]*/
extern const Meta_Property
_CIM_ProtocolEndpoint_OtherTypeDescription;

/*[1568]*/
extern const Meta_Property
_CIM_LANEndpoint_LANID;

/*[1568]*/
extern const Meta_Property
_CIM_LANEndpoint_LANType;

/*[1568]*/
extern const Meta_Property
_CIM_LANEndpoint_OtherLANType;

/*[1568]*/
extern const Meta_Property
_CIM_LANEndpoint_MACAddress;

/*[1568]*/
extern const Meta_Property
_CIM_LANEndpoint_AliasAddresses;

/*[1568]*/
extern const Meta_Property
_CIM_LANEndpoint_GroupAddresses;

/*[1568]*/
extern const Meta_Property
_CIM_LANEndpoint_MaxDataSize;

/*[2291]*/
static Meta_Feature* _SF_LANEndpoint_MFA[] =
{
    (Meta_Feature*)(void*)&_CIM_ManagedElement_InstanceID,
    (Meta_Feature*)(void*)&_CIM_ManagedElement_Caption,
    (Meta_Feature*)(void*)&_CIM_ProtocolEndpoint_Description,
    (Meta_Feature*)(void*)&_CIM_ManagedElement_ElementName,
    (Meta_Feature*)(void*)&_CIM_ManagedSystemElement_InstallDate,
    (Meta_Feature*)(void*)&_CIM_ProtocolEndpoint_Name,
    (Meta_Feature*)(void*)&_CIM_ProtocolEndpoint_OperationalStatus,
    (Meta_Feature*)(void*)&_CIM_ManagedSystemElement_StatusDescriptions,
    (Meta_Feature*)(void*)&_CIM_ManagedSystemElement_Status,
    (Meta_Feature*)(void*)&_CIM_ManagedSystemElement_HealthState,
    (Meta_Feature*)(void*)&_CIM_ManagedSystemElement_CommunicationStatus,
    (Meta_Feature*)(void*)&_CIM_ManagedSystemElement_DetailedStatus,
    (Meta_Feature*)(void*)&_CIM_ManagedSystemElement_OperatingStatus,
    (Meta_Feature*)(void*)&_CIM_ManagedSystemElement_PrimaryStatus,
    (Meta_Feature*)(void*)&_CIM_ProtocolEndpoint_EnabledState,
    (Meta_Feature*)(void*)&_CIM_EnabledLogicalElement_OtherEnabledState,
    (Meta_Feature*)(void*)&_CIM_EnabledLogicalElement_RequestedState,
    (Meta_Feature*)(void*)&_CIM_EnabledLogicalElement_EnabledDefault,
    (Meta_Feature*)(void*)&_CIM_ProtocolEndpoint_TimeOfLastStateChange,
    (Meta_Feature*)(void*)&_CIM_EnabledLogicalElement_AvailableRequestedStates,
    (Meta_Feature*)(void*)&_CIM_EnabledLogicalElement_TransitioningToState,
    (Meta_Feature*)(void*)&CIM_EnabledLogicalElement_RequestStateChange_method::static_meta_class,
    (Meta_Feature*)(void*)&_CIM_ServiceAccessPoint_SystemCreationClassName,
    (Meta_Feature*)(void*)&_CIM_ServiceAccessPoint_SystemName,
    (Meta_Feature*)(void*)&_CIM_ServiceAccessPoint_CreationClassName,
    (Meta_Feature*)(void*)&_CIM_ProtocolEndpoint_NameFormat,
    (Meta_Feature*)(void*)&_CIM_ProtocolEndpoint_ProtocolType,
    (Meta_Feature*)(void*)&_CIM_ProtocolEndpoint_ProtocolIFType,
    (Meta_Feature*)(void*)&_CIM_ProtocolEndpoint_OtherTypeDescription,
    (Meta_Feature*)(void*)&_CIM_LANEndpoint_LANID,
    (Meta_Feature*)(void*)&_CIM_LANEndpoint_LANType,
    (Meta_Feature*)(void*)&_CIM_LANEndpoint_OtherLANType,
    (Meta_Feature*)(void*)&_CIM_LANEndpoint_MACAddress,
    (Meta_Feature*)(void*)&_CIM_LANEndpoint_AliasAddresses,
    (Meta_Feature*)(void*)&_CIM_LANEndpoint_GroupAddresses,
    (Meta_Feature*)(void*)&_CIM_LANEndpoint_MaxDataSize,
};

/*[2338]*/
static const Meta_Feature_Local _locals[] =
{
    {0},
    {0},
    {0},
    {0},
    {0},
    {0},
    {0},
    {0},
    {0},
    {0},
    {0},
    {0},
    {0},
    {0},
    {0},
    {0},
    {0},
    {0},
    {0},
    {0},
    {0},
    {0},
    {0},
    {0},
    {0},
    {0},
    {0},
    {0},
    {0},
    {0},
    {0},
    {0},
    {0},
    {0},
    {0},
    {0},
};

/*[2358]*/
const Meta_Class SF_LANEndpoint::static_meta_class =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_CLASS,
    "SF_LANEndpoint",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    _SF_LANEndpoint_MFA,
    CIMPLE_ARRAY_SIZE(_SF_LANEndpoint_MFA),
    sizeof(SF_LANEndpoint),
    _locals,
    &CIM_LANEndpoint::static_meta_class,
    4, /* num_keys */
    &__meta_repository_52E780FCF95E157C8B11653275F6420A,
};

CIMPLE_NAMESPACE_END

