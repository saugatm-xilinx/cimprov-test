/*NOCHKSRC*/
//==============================================================================
//
// PLEASE DO NOT EDIT; THIS FILE WAS AUTOMATICALLY GENERATED BY GENCLASS 2.0.24
//
//==============================================================================

#include <cimple/Meta_Class.h>
#include <cimple/Meta_Property.h>
#include <cimple/Meta_Reference.h>
#include "SF_ConcreteJob.h"

CIMPLE_NAMESPACE_BEGIN

using namespace cimple;

extern const Meta_Repository __meta_repository_CCA3FB4C49C51F2D868FF3E04426D671;

/*[1568]*/
extern const Meta_Property
_CIM_ConcreteJob_InstanceID;

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
_CIM_ConcreteJob_Name;

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

/*[1568]*/
extern const Meta_Property
_CIM_Job_TimeSubmitted;

/*[1568]*/
extern const Meta_Property
_CIM_Job_ScheduledStartTime;

/*[1568]*/
extern const Meta_Property
_CIM_Job_StartTime;

/*[1568]*/
extern const Meta_Property
_CIM_Job_ElapsedTime;

/*[1568]*/
extern const Meta_Property
_CIM_Job_JobRunTimes;

/*[1568]*/
extern const Meta_Property
_CIM_Job_RunMonth;

/*[1568]*/
extern const Meta_Property
_CIM_Job_RunDay;

/*[1568]*/
extern const Meta_Property
_CIM_Job_RunDayOfWeek;

/*[1568]*/
extern const Meta_Property
_CIM_Job_RunStartInterval;

/*[1568]*/
extern const Meta_Property
_CIM_Job_LocalOrUtcTime;

/*[1568]*/
extern const Meta_Property
_CIM_Job_UntilTime;

/*[1568]*/
extern const Meta_Property
_CIM_Job_Notify;

/*[1568]*/
extern const Meta_Property
_CIM_Job_Owner;

/*[1568]*/
extern const Meta_Property
_CIM_Job_Priority;

/*[1568]*/
extern const Meta_Property
_CIM_Job_PercentComplete;

/*[1568]*/
extern const Meta_Property
_CIM_Job_DeleteOnCompletion;

/*[1568]*/
extern const Meta_Property
_CIM_Job_ErrorCode;

/*[1568]*/
extern const Meta_Property
_CIM_Job_ErrorDescription;

/*[1568]*/
extern const Meta_Property
_CIM_Job_RecoveryAction;

/*[1568]*/
extern const Meta_Property
_CIM_Job_OtherRecoveryAction;

/*[1568]*/
extern const Meta_Property
_CIM_ConcreteJob_JobState;

/*[1568]*/
extern const Meta_Property
_CIM_ConcreteJob_TimeOfLastStateChange;

/*[1568]*/
extern const Meta_Property
_CIM_ConcreteJob_TimeBeforeRemoval;

/*[2291]*/
static Meta_Feature* _SF_ConcreteJob_MFA[] =
{
    (Meta_Feature*)(void*)&_CIM_ConcreteJob_InstanceID,
    (Meta_Feature*)(void*)&_CIM_ManagedElement_Caption,
    (Meta_Feature*)(void*)&_CIM_ManagedElement_Description,
    (Meta_Feature*)(void*)&_CIM_ManagedElement_ElementName,
    (Meta_Feature*)(void*)&_CIM_ManagedSystemElement_InstallDate,
    (Meta_Feature*)(void*)&_CIM_ConcreteJob_Name,
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
    (Meta_Feature*)(void*)&_CIM_ConcreteJob_JobState,
    (Meta_Feature*)(void*)&_CIM_ConcreteJob_TimeOfLastStateChange,
    (Meta_Feature*)(void*)&_CIM_ConcreteJob_TimeBeforeRemoval,
    (Meta_Feature*)(void*)&CIM_ConcreteJob_RequestStateChange_method::static_meta_class,
    (Meta_Feature*)(void*)&CIM_ConcreteJob_GetError_method::static_meta_class,
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
};

/*[2358]*/
const Meta_Class SF_ConcreteJob::static_meta_class =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_CLASS,
    "SF_ConcreteJob",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    _SF_ConcreteJob_MFA,
    CIMPLE_ARRAY_SIZE(_SF_ConcreteJob_MFA),
    sizeof(SF_ConcreteJob),
    _locals,
    &CIM_ConcreteJob::static_meta_class,
    1, /* num_keys */
    &__meta_repository_CCA3FB4C49C51F2D868FF3E04426D671,
};

CIMPLE_NAMESPACE_END

