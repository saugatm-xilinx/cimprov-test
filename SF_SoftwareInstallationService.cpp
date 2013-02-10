/*NOCHKSRC*/
//==============================================================================
//
// PLEASE DO NOT EDIT; THIS FILE WAS AUTOMATICALLY GENERATED BY GENCLASS 2.0.24
//
//==============================================================================

#include <cimple/Meta_Class.h>
#include <cimple/Meta_Property.h>
#include <cimple/Meta_Reference.h>
#include "SF_SoftwareInstallationService.h"

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
_CIM_ManagedElement_Description;

/*[1568]*/
extern const Meta_Property
_CIM_ManagedElement_ElementName;

/*[1568]*/
extern const Meta_Property
_CIM_ManagedSystemElement_InstallDate;

/*[1568]*/
extern const Meta_Property
_CIM_Service_Name;

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
_CIM_Service_SystemCreationClassName;

/*[1568]*/
extern const Meta_Property
_CIM_Service_SystemName;

/*[1568]*/
extern const Meta_Property
_CIM_Service_CreationClassName;

/*[1568]*/
extern const Meta_Property
_CIM_Service_PrimaryOwnerName;

/*[1568]*/
extern const Meta_Property
_CIM_Service_PrimaryOwnerContact;

/*[1568]*/
extern const Meta_Property
_CIM_Service_StartMode;

/*[1568]*/
extern const Meta_Property
_CIM_Service_Started;

/*[2291]*/
static Meta_Feature* _SF_SoftwareInstallationService_MFA[] =
{
    (Meta_Feature*)(void*)&_CIM_ManagedElement_InstanceID,
    (Meta_Feature*)(void*)&_CIM_ManagedElement_Caption,
    (Meta_Feature*)(void*)&_CIM_ManagedElement_Description,
    (Meta_Feature*)(void*)&_CIM_ManagedElement_ElementName,
    (Meta_Feature*)(void*)&_CIM_ManagedSystemElement_InstallDate,
    (Meta_Feature*)(void*)&_CIM_Service_Name,
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
    (Meta_Feature*)(void*)&_CIM_Service_SystemCreationClassName,
    (Meta_Feature*)(void*)&_CIM_Service_SystemName,
    (Meta_Feature*)(void*)&_CIM_Service_CreationClassName,
    (Meta_Feature*)(void*)&_CIM_Service_PrimaryOwnerName,
    (Meta_Feature*)(void*)&_CIM_Service_PrimaryOwnerContact,
    (Meta_Feature*)(void*)&_CIM_Service_StartMode,
    (Meta_Feature*)(void*)&_CIM_Service_Started,
    (Meta_Feature*)(void*)&CIM_Service_StartService_method::static_meta_class,
    (Meta_Feature*)(void*)&CIM_Service_StopService_method::static_meta_class,
    (Meta_Feature*)(void*)&CIM_SoftwareInstallationService_CheckSoftwareIdentity_method::static_meta_class,
    (Meta_Feature*)(void*)&CIM_SoftwareInstallationService_InstallFromSoftwareIdentity_method::static_meta_class,
    (Meta_Feature*)(void*)&CIM_SoftwareInstallationService_InstallFromURI_method::static_meta_class,
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
};

/*[2358]*/
const Meta_Class SF_SoftwareInstallationService::static_meta_class =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_CLASS,
    "SF_SoftwareInstallationService",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    _SF_SoftwareInstallationService_MFA,
    CIMPLE_ARRAY_SIZE(_SF_SoftwareInstallationService_MFA),
    sizeof(SF_SoftwareInstallationService),
    _locals,
    &CIM_SoftwareInstallationService::static_meta_class,
    4, /* num_keys */
    &__meta_repository_52E780FCF95E157C8B11653275F6420A,
};

CIMPLE_NAMESPACE_END

