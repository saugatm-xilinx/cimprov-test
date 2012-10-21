/*NOCHKSRC*/
//==============================================================================
//
// PLEASE DO NOT EDIT; THIS FILE WAS AUTOMATICALLY GENERATED BY GENCLASS 2.0.24
//
//==============================================================================

#include <cimple/Meta_Class.h>
#include <cimple/Meta_Property.h>
#include <cimple/Meta_Reference.h>
#include "SF_PortController.h"

CIMPLE_NAMESPACE_BEGIN

using namespace cimple;

extern const Meta_Repository __meta_repository_56785F289C641B39983CABAFB114CB61;

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
_CIM_Controller_TimeOfLastReset;

/*[1568]*/
extern const Meta_Property
_CIM_Controller_ProtocolSupported;

/*[1568]*/
extern const Meta_Property
_CIM_Controller_MaxNumberControlled;

/*[1568]*/
extern const Meta_Property
_CIM_Controller_ProtocolDescription;

/*[1568]*/
extern const Meta_Property
_CIM_PortController_ControllerType;

/*[1568]*/
extern const Meta_Property
_CIM_PortController_OtherControllerType;

/*[1568]*/
extern const Meta_Property
_CIM_PortController_ControllerVersion;

/*[2291]*/
static Meta_Feature* _SF_PortController_MFA[] =
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
    (Meta_Feature*)(void*)&_CIM_Controller_TimeOfLastReset,
    (Meta_Feature*)(void*)&_CIM_Controller_ProtocolSupported,
    (Meta_Feature*)(void*)&_CIM_Controller_MaxNumberControlled,
    (Meta_Feature*)(void*)&_CIM_Controller_ProtocolDescription,
    (Meta_Feature*)(void*)&_CIM_PortController_ControllerType,
    (Meta_Feature*)(void*)&_CIM_PortController_OtherControllerType,
    (Meta_Feature*)(void*)&_CIM_PortController_ControllerVersion,
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
    {0},
    {0},
    {0},
};

/*[2358]*/
const Meta_Class SF_PortController::static_meta_class =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_CLASS,
    "SF_PortController",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    _SF_PortController_MFA,
    CIMPLE_ARRAY_SIZE(_SF_PortController_MFA),
    sizeof(SF_PortController),
    _locals,
    &CIM_PortController::static_meta_class,
    4, /* num_keys */
    &__meta_repository_56785F289C641B39983CABAFB114CB61,
};

CIMPLE_NAMESPACE_END

