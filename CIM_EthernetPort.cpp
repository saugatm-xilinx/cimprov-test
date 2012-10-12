/*NOCHKSRC*/
//==============================================================================
//
// PLEASE DO NOT EDIT; THIS FILE WAS AUTOMATICALLY GENERATED BY GENCLASS 2.0.24
//
//==============================================================================

#include <cimple/Meta_Class.h>
#include <cimple/Meta_Property.h>
#include <cimple/Meta_Reference.h>
#include "CIM_EthernetPort.h"

CIMPLE_NAMESPACE_BEGIN

using namespace cimple;

extern const Meta_Repository __meta_repository_CCA3FB4C49C51F2D868FF3E04426D671;

/*[1568]*/
extern const Meta_Property
_CIM_ManagedElement_InstanceID;

/*[1568]*/
extern const Meta_Property
_CIM_ManagedElement_Caption;

/*[1568]*/
extern const Meta_Property
_CIM_ManagedElement_Description;

/*[1568]*/
extern const Meta_Property
_CIM_ManagedElement_ElementName;

/*[1568]*/
extern const Meta_Property
_CIM_ManagedSystemElement_InstallDate;

/*[1568]*/
extern const Meta_Property
_CIM_ManagedSystemElement_Name;

/*[1568]*/
extern const Meta_Property
_CIM_ManagedSystemElement_OperationalStatus;

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
_CIM_EnabledLogicalElement_EnabledState;

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
_CIM_EnabledLogicalElement_TimeOfLastStateChange;

/*[1568]*/
extern const Meta_Property
_CIM_EnabledLogicalElement_AvailableRequestedStates;

/*[1568]*/
extern const Meta_Property
_CIM_EnabledLogicalElement_TransitioningToState;

/*[1568]*/
extern const Meta_Property
_CIM_LogicalDevice_SystemCreationClassName;

/*[1568]*/
extern const Meta_Property
_CIM_LogicalDevice_SystemName;

/*[1568]*/
extern const Meta_Property
_CIM_LogicalDevice_CreationClassName;

/*[1568]*/
extern const Meta_Property
_CIM_LogicalDevice_DeviceID;

/*[1568]*/
extern const Meta_Property
_CIM_LogicalDevice_PowerManagementSupported;

/*[1568]*/
extern const Meta_Property
_CIM_LogicalDevice_PowerManagementCapabilities;

/*[1568]*/
extern const Meta_Property
_CIM_LogicalDevice_Availability;

/*[1568]*/
extern const Meta_Property
_CIM_LogicalDevice_StatusInfo;

/*[1568]*/
extern const Meta_Property
_CIM_LogicalDevice_LastErrorCode;

/*[1568]*/
extern const Meta_Property
_CIM_LogicalDevice_ErrorDescription;

/*[1568]*/
extern const Meta_Property
_CIM_LogicalDevice_ErrorCleared;

/*[1568]*/
extern const Meta_Property
_CIM_LogicalDevice_OtherIdentifyingInfo;

/*[1568]*/
extern const Meta_Property
_CIM_LogicalDevice_PowerOnHours;

/*[1568]*/
extern const Meta_Property
_CIM_LogicalDevice_TotalPowerOnHours;

/*[1568]*/
extern const Meta_Property
_CIM_LogicalDevice_IdentifyingDescriptions;

/*[1568]*/
extern const Meta_Property
_CIM_LogicalDevice_AdditionalAvailability;

/*[1568]*/
extern const Meta_Property
_CIM_LogicalDevice_MaxQuiesceTime;

/*[1568]*/
extern const Meta_Property
_CIM_NetworkPort_Speed;

/*[1568]*/
extern const Meta_Property
_CIM_LogicalPort_MaxSpeed;

/*[1568]*/
extern const Meta_Property
_CIM_LogicalPort_RequestedSpeed;

/*[1568]*/
extern const Meta_Property
_CIM_LogicalPort_UsageRestriction;

/*[1568]*/
extern const Meta_Property
_CIM_EthernetPort_PortType;

/*[1591]*/
const Meta_Property
_CIM_EthernetPort_PortType =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "PortType",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    UINT16,
    0, /* subscript */
    CIMPLE_OFF(CIM_EthernetPort,PortType),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_LogicalPort_OtherPortType;

/*[1568]*/
extern const Meta_Property
_CIM_NetworkPort_OtherNetworkPortType;

/*[1568]*/
extern const Meta_Property
_CIM_NetworkPort_PortNumber;

/*[1568]*/
extern const Meta_Property
_CIM_NetworkPort_LinkTechnology;

/*[1568]*/
extern const Meta_Property
_CIM_NetworkPort_OtherLinkTechnology;

/*[1568]*/
extern const Meta_Property
_CIM_NetworkPort_PermanentAddress;

/*[1568]*/
extern const Meta_Property
_CIM_EthernetPort_NetworkAddresses;

/*[1591]*/
const Meta_Property
_CIM_EthernetPort_NetworkAddresses =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "NetworkAddresses",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    STRING,
    -1, /* subscript */
    CIMPLE_OFF(CIM_EthernetPort,NetworkAddresses),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_NetworkPort_FullDuplex;

/*[1568]*/
extern const Meta_Property
_CIM_NetworkPort_AutoSense;

/*[1568]*/
extern const Meta_Property
_CIM_NetworkPort_SupportedMaximumTransmissionUnit;

/*[1568]*/
extern const Meta_Property
_CIM_NetworkPort_ActiveMaximumTransmissionUnit;

/*[1568]*/
extern const Meta_Property
_CIM_EthernetPort_MaxDataSize;

/*[1591]*/
const Meta_Property
_CIM_EthernetPort_MaxDataSize =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "MaxDataSize",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    UINT32,
    0, /* subscript */
    CIMPLE_OFF(CIM_EthernetPort,MaxDataSize),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_EthernetPort_Capabilities;

/*[1591]*/
const Meta_Property
_CIM_EthernetPort_Capabilities =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "Capabilities",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    UINT16,
    -1, /* subscript */
    CIMPLE_OFF(CIM_EthernetPort,Capabilities),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_EthernetPort_CapabilityDescriptions;

/*[1591]*/
const Meta_Property
_CIM_EthernetPort_CapabilityDescriptions =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "CapabilityDescriptions",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    STRING,
    -1, /* subscript */
    CIMPLE_OFF(CIM_EthernetPort,CapabilityDescriptions),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_EthernetPort_EnabledCapabilities;

/*[1591]*/
const Meta_Property
_CIM_EthernetPort_EnabledCapabilities =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "EnabledCapabilities",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    UINT16,
    -1, /* subscript */
    CIMPLE_OFF(CIM_EthernetPort,EnabledCapabilities),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_EthernetPort_OtherEnabledCapabilities;

/*[1591]*/
const Meta_Property
_CIM_EthernetPort_OtherEnabledCapabilities =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "OtherEnabledCapabilities",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    STRING,
    -1, /* subscript */
    CIMPLE_OFF(CIM_EthernetPort,OtherEnabledCapabilities),
    0, /* value */
};

/*[2291]*/
static Meta_Feature* _CIM_EthernetPort_MFA[] =
{
    (Meta_Feature*)(void*)&_CIM_ManagedElement_InstanceID,
    (Meta_Feature*)(void*)&_CIM_ManagedElement_Caption,
    (Meta_Feature*)(void*)&_CIM_ManagedElement_Description,
    (Meta_Feature*)(void*)&_CIM_ManagedElement_ElementName,
    (Meta_Feature*)(void*)&_CIM_ManagedSystemElement_InstallDate,
    (Meta_Feature*)(void*)&_CIM_ManagedSystemElement_Name,
    (Meta_Feature*)(void*)&_CIM_ManagedSystemElement_OperationalStatus,
    (Meta_Feature*)(void*)&_CIM_ManagedSystemElement_StatusDescriptions,
    (Meta_Feature*)(void*)&_CIM_ManagedSystemElement_Status,
    (Meta_Feature*)(void*)&_CIM_ManagedSystemElement_HealthState,
    (Meta_Feature*)(void*)&_CIM_ManagedSystemElement_CommunicationStatus,
    (Meta_Feature*)(void*)&_CIM_ManagedSystemElement_DetailedStatus,
    (Meta_Feature*)(void*)&_CIM_ManagedSystemElement_OperatingStatus,
    (Meta_Feature*)(void*)&_CIM_ManagedSystemElement_PrimaryStatus,
    (Meta_Feature*)(void*)&_CIM_EnabledLogicalElement_EnabledState,
    (Meta_Feature*)(void*)&_CIM_EnabledLogicalElement_OtherEnabledState,
    (Meta_Feature*)(void*)&_CIM_EnabledLogicalElement_RequestedState,
    (Meta_Feature*)(void*)&_CIM_EnabledLogicalElement_EnabledDefault,
    (Meta_Feature*)(void*)&_CIM_EnabledLogicalElement_TimeOfLastStateChange,
    (Meta_Feature*)(void*)&_CIM_EnabledLogicalElement_AvailableRequestedStates,
    (Meta_Feature*)(void*)&_CIM_EnabledLogicalElement_TransitioningToState,
    (Meta_Feature*)(void*)&CIM_EnabledLogicalElement_RequestStateChange_method::static_meta_class,
    (Meta_Feature*)(void*)&_CIM_LogicalDevice_SystemCreationClassName,
    (Meta_Feature*)(void*)&_CIM_LogicalDevice_SystemName,
    (Meta_Feature*)(void*)&_CIM_LogicalDevice_CreationClassName,
    (Meta_Feature*)(void*)&_CIM_LogicalDevice_DeviceID,
    (Meta_Feature*)(void*)&_CIM_LogicalDevice_PowerManagementSupported,
    (Meta_Feature*)(void*)&_CIM_LogicalDevice_PowerManagementCapabilities,
    (Meta_Feature*)(void*)&_CIM_LogicalDevice_Availability,
    (Meta_Feature*)(void*)&_CIM_LogicalDevice_StatusInfo,
    (Meta_Feature*)(void*)&_CIM_LogicalDevice_LastErrorCode,
    (Meta_Feature*)(void*)&_CIM_LogicalDevice_ErrorDescription,
    (Meta_Feature*)(void*)&_CIM_LogicalDevice_ErrorCleared,
    (Meta_Feature*)(void*)&_CIM_LogicalDevice_OtherIdentifyingInfo,
    (Meta_Feature*)(void*)&_CIM_LogicalDevice_PowerOnHours,
    (Meta_Feature*)(void*)&_CIM_LogicalDevice_TotalPowerOnHours,
    (Meta_Feature*)(void*)&_CIM_LogicalDevice_IdentifyingDescriptions,
    (Meta_Feature*)(void*)&_CIM_LogicalDevice_AdditionalAvailability,
    (Meta_Feature*)(void*)&_CIM_LogicalDevice_MaxQuiesceTime,
    (Meta_Feature*)(void*)&CIM_LogicalDevice_SetPowerState_method::static_meta_class,
    (Meta_Feature*)(void*)&CIM_LogicalDevice_Reset_method::static_meta_class,
    (Meta_Feature*)(void*)&CIM_LogicalDevice_EnableDevice_method::static_meta_class,
    (Meta_Feature*)(void*)&CIM_LogicalDevice_OnlineDevice_method::static_meta_class,
    (Meta_Feature*)(void*)&CIM_LogicalDevice_QuiesceDevice_method::static_meta_class,
    (Meta_Feature*)(void*)&CIM_LogicalDevice_SaveProperties_method::static_meta_class,
    (Meta_Feature*)(void*)&CIM_LogicalDevice_RestoreProperties_method::static_meta_class,
    (Meta_Feature*)(void*)&_CIM_NetworkPort_Speed,
    (Meta_Feature*)(void*)&_CIM_LogicalPort_MaxSpeed,
    (Meta_Feature*)(void*)&_CIM_LogicalPort_RequestedSpeed,
    (Meta_Feature*)(void*)&_CIM_LogicalPort_UsageRestriction,
    (Meta_Feature*)(void*)&_CIM_EthernetPort_PortType,
    (Meta_Feature*)(void*)&_CIM_LogicalPort_OtherPortType,
    (Meta_Feature*)(void*)&_CIM_NetworkPort_OtherNetworkPortType,
    (Meta_Feature*)(void*)&_CIM_NetworkPort_PortNumber,
    (Meta_Feature*)(void*)&_CIM_NetworkPort_LinkTechnology,
    (Meta_Feature*)(void*)&_CIM_NetworkPort_OtherLinkTechnology,
    (Meta_Feature*)(void*)&_CIM_NetworkPort_PermanentAddress,
    (Meta_Feature*)(void*)&_CIM_EthernetPort_NetworkAddresses,
    (Meta_Feature*)(void*)&_CIM_NetworkPort_FullDuplex,
    (Meta_Feature*)(void*)&_CIM_NetworkPort_AutoSense,
    (Meta_Feature*)(void*)&_CIM_NetworkPort_SupportedMaximumTransmissionUnit,
    (Meta_Feature*)(void*)&_CIM_NetworkPort_ActiveMaximumTransmissionUnit,
    (Meta_Feature*)(void*)&_CIM_EthernetPort_MaxDataSize,
    (Meta_Feature*)(void*)&_CIM_EthernetPort_Capabilities,
    (Meta_Feature*)(void*)&_CIM_EthernetPort_CapabilityDescriptions,
    (Meta_Feature*)(void*)&_CIM_EthernetPort_EnabledCapabilities,
    (Meta_Feature*)(void*)&_CIM_EthernetPort_OtherEnabledCapabilities,
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
    {1},
    {0},
    {0},
    {0},
    {0},
    {0},
    {0},
    {1},
    {0},
    {0},
    {0},
    {0},
    {1},
    {1},
    {1},
    {1},
    {1},
};

/*[2358]*/
const Meta_Class CIM_EthernetPort::static_meta_class =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_CLASS,
    "CIM_EthernetPort",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    _CIM_EthernetPort_MFA,
    CIMPLE_ARRAY_SIZE(_CIM_EthernetPort_MFA),
    sizeof(CIM_EthernetPort),
    _locals,
    &CIM_NetworkPort::static_meta_class,
    4, /* num_keys */
    &__meta_repository_CCA3FB4C49C51F2D868FF3E04426D671,
};

CIMPLE_NAMESPACE_END

