/*NOCHKSRC*/
//==============================================================================
//
// PLEASE DO NOT EDIT; THIS FILE WAS AUTOMATICALLY GENERATED BY GENCLASS 2.0.24
//
//==============================================================================

#include <cimple/Meta_Class.h>
#include <cimple/Meta_Property.h>
#include <cimple/Meta_Reference.h>
#include "CIM_RecordLog.h"

CIMPLE_NAMESPACE_BEGIN

using namespace cimple;

extern const Meta_Repository __meta_repository_7D6DA94023C0157F82D3A7310346C853;

/*[1568]*/
extern const Meta_Property
_CIM_RecordLog_InstanceID;

/*[1591]*/
const Meta_Property
_CIM_RecordLog_InstanceID =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_KEY|CIMPLE_FLAG_READ,
    "InstanceID",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    STRING,
    0, /* subscript */
    CIMPLE_OFF(CIM_RecordLog,InstanceID),
    0, /* value */
};

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
_CIM_Log_MaxNumberOfRecords;

/*[1568]*/
extern const Meta_Property
_CIM_Log_CurrentNumberOfRecords;

/*[1568]*/
extern const Meta_Property
_CIM_Log_OverwritePolicy;

/*[1568]*/
extern const Meta_Property
_CIM_Log_LogState;

/*[2291]*/
static Meta_Feature* _CIM_RecordLog_MFA[] =
{
    (Meta_Feature*)(void*)&_CIM_RecordLog_InstanceID,
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
    (Meta_Feature*)(void*)&_CIM_Log_MaxNumberOfRecords,
    (Meta_Feature*)(void*)&_CIM_Log_CurrentNumberOfRecords,
    (Meta_Feature*)(void*)&_CIM_Log_OverwritePolicy,
    (Meta_Feature*)(void*)&_CIM_Log_LogState,
    (Meta_Feature*)(void*)&CIM_Log_ClearLog_method::static_meta_class,
};

/*[2338]*/
static const Meta_Feature_Local _locals[] =
{
    {1},
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
const Meta_Class CIM_RecordLog::static_meta_class =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_CLASS,
    "CIM_RecordLog",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    _CIM_RecordLog_MFA,
    CIMPLE_ARRAY_SIZE(_CIM_RecordLog_MFA),
    sizeof(CIM_RecordLog),
    _locals,
    &CIM_Log::static_meta_class,
    1, /* num_keys */
    &__meta_repository_7D6DA94023C0157F82D3A7310346C853,
};

CIMPLE_NAMESPACE_END

