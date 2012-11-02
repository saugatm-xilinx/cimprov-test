/*NOCHKSRC*/
//==============================================================================
//
// PLEASE DO NOT EDIT; THIS FILE WAS AUTOMATICALLY GENERATED BY GENCLASS 2.0.24
//
//==============================================================================

#include <cimple/Meta_Class.h>
#include <cimple/Meta_Property.h>
#include <cimple/Meta_Reference.h>
#include "CIM_DiagnosticService.h"

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

/*[1721]*/
static const Meta_Reference
_CIM_DiagnosticService_RunDiagnostic_ManagedElement =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_REFERENCE|CIMPLE_FLAG_IN,
    "ManagedElement",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    0, /* subscript */
    &CIM_ManagedElement::static_meta_class,
    CIMPLE_OFF(CIM_DiagnosticService_RunDiagnostic_method,ManagedElement)
};

/*[1721]*/
static const Meta_Reference
_CIM_DiagnosticService_RunDiagnostic_DiagSetting =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_REFERENCE|CIMPLE_FLAG_IN,
    "DiagSetting",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    0, /* subscript */
    &CIM_DiagnosticSetting::static_meta_class,
    CIMPLE_OFF(CIM_DiagnosticService_RunDiagnostic_method,DiagSetting)
};

/*[1721]*/
static const Meta_Reference
_CIM_DiagnosticService_RunDiagnostic_JobSetting =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_REFERENCE|CIMPLE_FLAG_IN,
    "JobSetting",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    0, /* subscript */
    &CIM_JobSettingData::static_meta_class,
    CIMPLE_OFF(CIM_DiagnosticService_RunDiagnostic_method,JobSetting)
};

/*[1721]*/
static const Meta_Reference
_CIM_DiagnosticService_RunDiagnostic_Job =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_REFERENCE|CIMPLE_FLAG_OUT,
    "Job",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    0, /* subscript */
    &CIM_ConcreteJob::static_meta_class,
    CIMPLE_OFF(CIM_DiagnosticService_RunDiagnostic_method,Job)
};

/*[1935]*/
static const Meta_Property
_CIM_DiagnosticService_RunDiagnostic_return_value =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_OUT,
    "return_value",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    UINT32,
    0,
    CIMPLE_OFF(CIM_DiagnosticService_RunDiagnostic_method,return_value),
    0, /* value */
};

/*[1911]*/
static Meta_Feature* _CIM_DiagnosticService_RunDiagnostic_MFA[] =
{
    (Meta_Feature*)(void*)&_CIM_DiagnosticService_RunDiagnostic_ManagedElement,
    (Meta_Feature*)(void*)&_CIM_DiagnosticService_RunDiagnostic_DiagSetting,
    (Meta_Feature*)(void*)&_CIM_DiagnosticService_RunDiagnostic_JobSetting,
    (Meta_Feature*)(void*)&_CIM_DiagnosticService_RunDiagnostic_Job,
    (Meta_Feature*)(void*)&_CIM_DiagnosticService_RunDiagnostic_return_value
};

/*[2113]*/
const Meta_Method
CIM_DiagnosticService_RunDiagnostic_method::static_meta_class =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_METHOD,
    "RunDiagnostic",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    _CIM_DiagnosticService_RunDiagnostic_MFA,
    CIMPLE_ARRAY_SIZE(_CIM_DiagnosticService_RunDiagnostic_MFA),
    sizeof(CIM_DiagnosticService_RunDiagnostic_method),
    UINT32,
};

/*[1721]*/
static const Meta_Reference
_CIM_DiagnosticService_RunDiagnosticService_ManagedElement =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_REFERENCE|CIMPLE_FLAG_IN,
    "ManagedElement",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    0, /* subscript */
    &CIM_ManagedElement::static_meta_class,
    CIMPLE_OFF(CIM_DiagnosticService_RunDiagnosticService_method,ManagedElement)
};

/*[1846]*/
static const Meta_Reference
_CIM_DiagnosticService_RunDiagnosticService_DiagnosticSettings =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_REFERENCE|CIMPLE_FLAG_EMBEDDED_INSTANCE|CIMPLE_FLAG_IN,
    "DiagnosticSettings",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    0, /* subscript */
    &CIM_DiagnosticSettingData::static_meta_class,
    CIMPLE_OFF(CIM_DiagnosticService_RunDiagnosticService_method,DiagnosticSettings)
};

/*[1846]*/
static const Meta_Reference
_CIM_DiagnosticService_RunDiagnosticService_JobSettings =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_REFERENCE|CIMPLE_FLAG_EMBEDDED_INSTANCE|CIMPLE_FLAG_IN,
    "JobSettings",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    0, /* subscript */
    &CIM_JobSettingData::static_meta_class,
    CIMPLE_OFF(CIM_DiagnosticService_RunDiagnosticService_method,JobSettings)
};

/*[1721]*/
static const Meta_Reference
_CIM_DiagnosticService_RunDiagnosticService_Job =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_REFERENCE|CIMPLE_FLAG_OUT,
    "Job",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    0, /* subscript */
    &CIM_ConcreteJob::static_meta_class,
    CIMPLE_OFF(CIM_DiagnosticService_RunDiagnosticService_method,Job)
};

/*[1935]*/
static const Meta_Property
_CIM_DiagnosticService_RunDiagnosticService_return_value =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_OUT,
    "return_value",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    UINT32,
    0,
    CIMPLE_OFF(CIM_DiagnosticService_RunDiagnosticService_method,return_value),
    0, /* value */
};

/*[1911]*/
static Meta_Feature* _CIM_DiagnosticService_RunDiagnosticService_MFA[] =
{
    (Meta_Feature*)(void*)&_CIM_DiagnosticService_RunDiagnosticService_ManagedElement,
    (Meta_Feature*)(void*)&_CIM_DiagnosticService_RunDiagnosticService_DiagnosticSettings,
    (Meta_Feature*)(void*)&_CIM_DiagnosticService_RunDiagnosticService_JobSettings,
    (Meta_Feature*)(void*)&_CIM_DiagnosticService_RunDiagnosticService_Job,
    (Meta_Feature*)(void*)&_CIM_DiagnosticService_RunDiagnosticService_return_value
};

/*[2113]*/
const Meta_Method
CIM_DiagnosticService_RunDiagnosticService_method::static_meta_class =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_METHOD,
    "RunDiagnosticService",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    _CIM_DiagnosticService_RunDiagnosticService_MFA,
    CIMPLE_ARRAY_SIZE(_CIM_DiagnosticService_RunDiagnosticService_MFA),
    sizeof(CIM_DiagnosticService_RunDiagnosticService_method),
    UINT32,
};

/*[2291]*/
static Meta_Feature* _CIM_DiagnosticService_MFA[] =
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
    (Meta_Feature*)(void*)&CIM_DiagnosticService_RunDiagnostic_method::static_meta_class,
    (Meta_Feature*)(void*)&CIM_DiagnosticService_RunDiagnosticService_method::static_meta_class,
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
    {1},
    {1},
};

/*[2358]*/
const Meta_Class CIM_DiagnosticService::static_meta_class =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_CLASS|CIMPLE_FLAG_ABSTRACT,
    "CIM_DiagnosticService",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    _CIM_DiagnosticService_MFA,
    CIMPLE_ARRAY_SIZE(_CIM_DiagnosticService_MFA),
    sizeof(CIM_DiagnosticService),
    _locals,
    &CIM_Service::static_meta_class,
    4, /* num_keys */
    &__meta_repository_52E780FCF95E157C8B11653275F6420A,
};

CIMPLE_NAMESPACE_END
