/*NOCHKSRC*/
//==============================================================================
//
// PLEASE DO NOT EDIT; THIS FILE WAS AUTOMATICALLY GENERATED BY GENCLASS 2.0.24
//
//==============================================================================

#include <cimple/Meta_Class.h>
#include <cimple/Meta_Property.h>
#include <cimple/Meta_Reference.h>
#include "CIM_ComputerSystem.h"

CIMPLE_NAMESPACE_BEGIN

using namespace cimple;

extern const Meta_Repository __meta_repository_480038F9449B17AC9E2210119CF38AF2;

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
_CIM_System_Name;

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
_CIM_System_CreationClassName;

/*[1568]*/
extern const Meta_Property
_CIM_ComputerSystem_NameFormat;

/*[1591]*/
const Meta_Property
_CIM_ComputerSystem_NameFormat =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "NameFormat",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    STRING,
    0, /* subscript */
    CIMPLE_OFF(CIM_ComputerSystem,NameFormat),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_System_PrimaryOwnerName;

/*[1568]*/
extern const Meta_Property
_CIM_System_PrimaryOwnerContact;

/*[1568]*/
extern const Meta_Property
_CIM_System_Roles;

/*[1568]*/
extern const Meta_Property
_CIM_System_OtherIdentifyingInfo;

/*[1568]*/
extern const Meta_Property
_CIM_System_IdentifyingDescriptions;

/*[1568]*/
extern const Meta_Property
_CIM_ComputerSystem_Dedicated;

/*[1591]*/
const Meta_Property
_CIM_ComputerSystem_Dedicated =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "Dedicated",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    UINT16,
    -1, /* subscript */
    CIMPLE_OFF(CIM_ComputerSystem,Dedicated),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_ComputerSystem_OtherDedicatedDescriptions;

/*[1591]*/
const Meta_Property
_CIM_ComputerSystem_OtherDedicatedDescriptions =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "OtherDedicatedDescriptions",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    STRING,
    -1, /* subscript */
    CIMPLE_OFF(CIM_ComputerSystem,OtherDedicatedDescriptions),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_ComputerSystem_ResetCapability;

/*[1591]*/
const Meta_Property
_CIM_ComputerSystem_ResetCapability =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "ResetCapability",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    UINT16,
    0, /* subscript */
    CIMPLE_OFF(CIM_ComputerSystem,ResetCapability),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_ComputerSystem_PowerManagementCapabilities;

/*[1591]*/
const Meta_Property
_CIM_ComputerSystem_PowerManagementCapabilities =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "PowerManagementCapabilities",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    UINT16,
    -1, /* subscript */
    CIMPLE_OFF(CIM_ComputerSystem,PowerManagementCapabilities),
    0, /* value */
};

/*[1782]*/
static const Meta_Property
_CIM_ComputerSystem_SetPowerState_PowerState =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_IN,
    "PowerState",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    UINT32,
    0,
    CIMPLE_OFF(CIM_ComputerSystem_SetPowerState_method,PowerState),
    0, /* value */
};

/*[1782]*/
static const Meta_Property
_CIM_ComputerSystem_SetPowerState_Time =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_IN,
    "Time",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    DATETIME,
    0,
    CIMPLE_OFF(CIM_ComputerSystem_SetPowerState_method,Time),
    0, /* value */
};

/*[1935]*/
static const Meta_Property
_CIM_ComputerSystem_SetPowerState_return_value =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_OUT,
    "return_value",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    UINT32,
    0,
    CIMPLE_OFF(CIM_ComputerSystem_SetPowerState_method,return_value),
    0, /* value */
};

/*[1911]*/
static Meta_Feature* _CIM_ComputerSystem_SetPowerState_MFA[] =
{
    (Meta_Feature*)(void*)&_CIM_ComputerSystem_SetPowerState_PowerState,
    (Meta_Feature*)(void*)&_CIM_ComputerSystem_SetPowerState_Time,
    (Meta_Feature*)(void*)&_CIM_ComputerSystem_SetPowerState_return_value
};

/*[2113]*/
const Meta_Method
CIM_ComputerSystem_SetPowerState_method::static_meta_class =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_METHOD,
    "SetPowerState",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    _CIM_ComputerSystem_SetPowerState_MFA,
    CIMPLE_ARRAY_SIZE(_CIM_ComputerSystem_SetPowerState_MFA),
    sizeof(CIM_ComputerSystem_SetPowerState_method),
    UINT32,
};

/*[2291]*/
static Meta_Feature* _CIM_ComputerSystem_MFA[] =
{
    (Meta_Feature*)(void*)&_CIM_ManagedElement_InstanceID,
    (Meta_Feature*)(void*)&_CIM_ManagedElement_Caption,
    (Meta_Feature*)(void*)&_CIM_ManagedElement_Description,
    (Meta_Feature*)(void*)&_CIM_ManagedElement_ElementName,
    (Meta_Feature*)(void*)&_CIM_ManagedSystemElement_InstallDate,
    (Meta_Feature*)(void*)&_CIM_System_Name,
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
    (Meta_Feature*)(void*)&_CIM_System_CreationClassName,
    (Meta_Feature*)(void*)&_CIM_ComputerSystem_NameFormat,
    (Meta_Feature*)(void*)&_CIM_System_PrimaryOwnerName,
    (Meta_Feature*)(void*)&_CIM_System_PrimaryOwnerContact,
    (Meta_Feature*)(void*)&_CIM_System_Roles,
    (Meta_Feature*)(void*)&_CIM_System_OtherIdentifyingInfo,
    (Meta_Feature*)(void*)&_CIM_System_IdentifyingDescriptions,
    (Meta_Feature*)(void*)&_CIM_ComputerSystem_Dedicated,
    (Meta_Feature*)(void*)&_CIM_ComputerSystem_OtherDedicatedDescriptions,
    (Meta_Feature*)(void*)&_CIM_ComputerSystem_ResetCapability,
    (Meta_Feature*)(void*)&_CIM_ComputerSystem_PowerManagementCapabilities,
    (Meta_Feature*)(void*)&CIM_ComputerSystem_SetPowerState_method::static_meta_class,
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
    {1},
    {0},
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
const Meta_Class CIM_ComputerSystem::static_meta_class =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_CLASS,
    "CIM_ComputerSystem",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    _CIM_ComputerSystem_MFA,
    CIMPLE_ARRAY_SIZE(_CIM_ComputerSystem_MFA),
    sizeof(CIM_ComputerSystem),
    _locals,
    &CIM_System::static_meta_class,
    2, /* num_keys */
    &__meta_repository_480038F9449B17AC9E2210119CF38AF2,
};

CIMPLE_NAMESPACE_END

