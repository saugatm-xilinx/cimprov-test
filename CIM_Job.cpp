/*NOCHKSRC*/
//==============================================================================
//
// PLEASE DO NOT EDIT; THIS FILE WAS AUTOMATICALLY GENERATED BY GENCLASS 2.0.24
//
//==============================================================================

#include <cimple/Meta_Class.h>
#include <cimple/Meta_Property.h>
#include <cimple/Meta_Reference.h>
#include "CIM_Job.h"

CIMPLE_NAMESPACE_BEGIN

using namespace cimple;

extern const Meta_Repository __meta_repository_25890684DE9F1ED99AF8E12287919123;

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
_CIM_Job_JobStatus;

/*[1591]*/
const Meta_Property
_CIM_Job_JobStatus =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "JobStatus",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    STRING,
    0, /* subscript */
    CIMPLE_OFF(CIM_Job,JobStatus),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_Job_TimeSubmitted;

/*[1591]*/
const Meta_Property
_CIM_Job_TimeSubmitted =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "TimeSubmitted",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    DATETIME,
    0, /* subscript */
    CIMPLE_OFF(CIM_Job,TimeSubmitted),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_Job_ScheduledStartTime;

/*[1591]*/
const Meta_Property
_CIM_Job_ScheduledStartTime =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ|CIMPLE_FLAG_WRITE,
    "ScheduledStartTime",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    DATETIME,
    0, /* subscript */
    CIMPLE_OFF(CIM_Job,ScheduledStartTime),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_Job_StartTime;

/*[1591]*/
const Meta_Property
_CIM_Job_StartTime =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "StartTime",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    DATETIME,
    0, /* subscript */
    CIMPLE_OFF(CIM_Job,StartTime),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_Job_ElapsedTime;

/*[1591]*/
const Meta_Property
_CIM_Job_ElapsedTime =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "ElapsedTime",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    DATETIME,
    0, /* subscript */
    CIMPLE_OFF(CIM_Job,ElapsedTime),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_Job_JobRunTimes;

/*[540]*/
static const Meta_Value_Scalar<uint32>
_CIM_Job_JobRunTimes_MV =
{
    CIMPLE_UINT32_LITERAL(1)
};

/*[1591]*/
const Meta_Property
_CIM_Job_JobRunTimes =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ|CIMPLE_FLAG_WRITE,
    "JobRunTimes",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    UINT32,
    0, /* subscript */
    CIMPLE_OFF(CIM_Job,JobRunTimes),
    (const Meta_Value*)(void*)&_CIM_Job_JobRunTimes_MV,
};

/*[1568]*/
extern const Meta_Property
_CIM_Job_RunMonth;

/*[1591]*/
const Meta_Property
_CIM_Job_RunMonth =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ|CIMPLE_FLAG_WRITE,
    "RunMonth",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    UINT8,
    0, /* subscript */
    CIMPLE_OFF(CIM_Job,RunMonth),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_Job_RunDay;

/*[1591]*/
const Meta_Property
_CIM_Job_RunDay =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ|CIMPLE_FLAG_WRITE,
    "RunDay",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    SINT8,
    0, /* subscript */
    CIMPLE_OFF(CIM_Job,RunDay),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_Job_RunDayOfWeek;

/*[1591]*/
const Meta_Property
_CIM_Job_RunDayOfWeek =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ|CIMPLE_FLAG_WRITE,
    "RunDayOfWeek",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    SINT8,
    0, /* subscript */
    CIMPLE_OFF(CIM_Job,RunDayOfWeek),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_Job_RunStartInterval;

/*[1591]*/
const Meta_Property
_CIM_Job_RunStartInterval =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ|CIMPLE_FLAG_WRITE,
    "RunStartInterval",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    DATETIME,
    0, /* subscript */
    CIMPLE_OFF(CIM_Job,RunStartInterval),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_Job_LocalOrUtcTime;

/*[1591]*/
const Meta_Property
_CIM_Job_LocalOrUtcTime =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ|CIMPLE_FLAG_WRITE,
    "LocalOrUtcTime",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    UINT16,
    0, /* subscript */
    CIMPLE_OFF(CIM_Job,LocalOrUtcTime),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_Job_UntilTime;

/*[1591]*/
const Meta_Property
_CIM_Job_UntilTime =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ|CIMPLE_FLAG_WRITE,
    "UntilTime",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    DATETIME,
    0, /* subscript */
    CIMPLE_OFF(CIM_Job,UntilTime),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_Job_Notify;

/*[1591]*/
const Meta_Property
_CIM_Job_Notify =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ|CIMPLE_FLAG_WRITE,
    "Notify",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    STRING,
    0, /* subscript */
    CIMPLE_OFF(CIM_Job,Notify),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_Job_Owner;

/*[1591]*/
const Meta_Property
_CIM_Job_Owner =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "Owner",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    STRING,
    0, /* subscript */
    CIMPLE_OFF(CIM_Job,Owner),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_Job_Priority;

/*[1591]*/
const Meta_Property
_CIM_Job_Priority =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ|CIMPLE_FLAG_WRITE,
    "Priority",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    UINT32,
    0, /* subscript */
    CIMPLE_OFF(CIM_Job,Priority),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_Job_PercentComplete;

/*[1591]*/
const Meta_Property
_CIM_Job_PercentComplete =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "PercentComplete",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    UINT16,
    0, /* subscript */
    CIMPLE_OFF(CIM_Job,PercentComplete),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_Job_DeleteOnCompletion;

/*[1591]*/
const Meta_Property
_CIM_Job_DeleteOnCompletion =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ|CIMPLE_FLAG_WRITE,
    "DeleteOnCompletion",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    BOOLEAN,
    0, /* subscript */
    CIMPLE_OFF(CIM_Job,DeleteOnCompletion),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_Job_ErrorCode;

/*[1591]*/
const Meta_Property
_CIM_Job_ErrorCode =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "ErrorCode",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    UINT16,
    0, /* subscript */
    CIMPLE_OFF(CIM_Job,ErrorCode),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_Job_ErrorDescription;

/*[1591]*/
const Meta_Property
_CIM_Job_ErrorDescription =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "ErrorDescription",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    STRING,
    0, /* subscript */
    CIMPLE_OFF(CIM_Job,ErrorDescription),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_Job_RecoveryAction;

/*[1591]*/
const Meta_Property
_CIM_Job_RecoveryAction =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "RecoveryAction",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    UINT16,
    0, /* subscript */
    CIMPLE_OFF(CIM_Job,RecoveryAction),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_Job_OtherRecoveryAction;

/*[1591]*/
const Meta_Property
_CIM_Job_OtherRecoveryAction =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "OtherRecoveryAction",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    STRING,
    0, /* subscript */
    CIMPLE_OFF(CIM_Job,OtherRecoveryAction),
    0, /* value */
};

/*[1782]*/
static const Meta_Property
_CIM_Job_KillJob_DeleteOnKill =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_IN,
    "DeleteOnKill",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    BOOLEAN,
    0,
    CIMPLE_OFF(CIM_Job_KillJob_method,DeleteOnKill),
    0, /* value */
};

/*[1935]*/
static const Meta_Property
_CIM_Job_KillJob_return_value =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_OUT,
    "return_value",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    UINT32,
    0,
    CIMPLE_OFF(CIM_Job_KillJob_method,return_value),
    0, /* value */
};

/*[1911]*/
static Meta_Feature* _CIM_Job_KillJob_MFA[] =
{
    (Meta_Feature*)(void*)&_CIM_Job_KillJob_DeleteOnKill,
    (Meta_Feature*)(void*)&_CIM_Job_KillJob_return_value
};

/*[2113]*/
const Meta_Method
CIM_Job_KillJob_method::static_meta_class =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_METHOD,
    "KillJob",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    _CIM_Job_KillJob_MFA,
    CIMPLE_ARRAY_SIZE(_CIM_Job_KillJob_MFA),
    sizeof(CIM_Job_KillJob_method),
    UINT32,
};

/*[2291]*/
static Meta_Feature* _CIM_Job_MFA[] =
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
    (Meta_Feature*)(void*)&_CIM_Job_JobStatus,
    (Meta_Feature*)(void*)&_CIM_Job_TimeSubmitted,
    (Meta_Feature*)(void*)&_CIM_Job_ScheduledStartTime,
    (Meta_Feature*)(void*)&_CIM_Job_StartTime,
    (Meta_Feature*)(void*)&_CIM_Job_ElapsedTime,
    (Meta_Feature*)(void*)&_CIM_Job_JobRunTimes,
    (Meta_Feature*)(void*)&_CIM_Job_RunMonth,
    (Meta_Feature*)(void*)&_CIM_Job_RunDay,
    (Meta_Feature*)(void*)&_CIM_Job_RunDayOfWeek,
    (Meta_Feature*)(void*)&_CIM_Job_RunStartInterval,
    (Meta_Feature*)(void*)&_CIM_Job_LocalOrUtcTime,
    (Meta_Feature*)(void*)&_CIM_Job_UntilTime,
    (Meta_Feature*)(void*)&_CIM_Job_Notify,
    (Meta_Feature*)(void*)&_CIM_Job_Owner,
    (Meta_Feature*)(void*)&_CIM_Job_Priority,
    (Meta_Feature*)(void*)&_CIM_Job_PercentComplete,
    (Meta_Feature*)(void*)&_CIM_Job_DeleteOnCompletion,
    (Meta_Feature*)(void*)&_CIM_Job_ErrorCode,
    (Meta_Feature*)(void*)&_CIM_Job_ErrorDescription,
    (Meta_Feature*)(void*)&_CIM_Job_RecoveryAction,
    (Meta_Feature*)(void*)&_CIM_Job_OtherRecoveryAction,
    (Meta_Feature*)(void*)&CIM_Job_KillJob_method::static_meta_class,
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
    {1},
    {1},
    {1},
    {1},
    {1},
    {1},
    {1},
    {1},
    {1},
    {1},
    {1},
    {1},
    {1},
    {1},
    {1},
    {1},
    {1},
    {1},
    {1},
    {1},
    {1},
    {1},
};

/*[2358]*/
const Meta_Class CIM_Job::static_meta_class =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_CLASS|CIMPLE_FLAG_ABSTRACT,
    "CIM_Job",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    _CIM_Job_MFA,
    CIMPLE_ARRAY_SIZE(_CIM_Job_MFA),
    sizeof(CIM_Job),
    _locals,
    &CIM_LogicalElement::static_meta_class,
    0, /* num_keys */
    &__meta_repository_25890684DE9F1ED99AF8E12287919123,
};

CIMPLE_NAMESPACE_END

