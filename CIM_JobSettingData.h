/*NOCHKSRC*/
//==============================================================================
//
// PLEASE DO NOT EDIT; THIS FILE WAS AUTOMATICALLY GENERATED BY GENCLASS 2.0.24
//
//==============================================================================

#ifndef _cimple_CIM_JobSettingData_h
#define _cimple_CIM_JobSettingData_h

#include <cimple/cimple.h>
#include "CIM_SettingData.h"

CIMPLE_NAMESPACE_BEGIN

// CIM_JobSettingData keys:
//     InstanceID

/*[1194]*/
class CIM_JobSettingData : public Instance
{
public:
    // CIM_ManagedElement features:
    Property<String> InstanceID;
    Property<String> Caption;
    Property<String> Description;
    Property<String> ElementName;
    Property<uint64> Generation;

    // CIM_SettingData features:
    Property<String> ConfigurationName;
    struct _ChangeableType
    {
        enum
        {
            enum_Not_Changeable___Persistent = 0,
            enum_Changeable___Transient = 1,
            enum_Changeable___Persistent = 2,
            enum_Not_Changeable___Transient = 3,
        };
        uint16 value;
        uint8 null;
    }
    ChangeableType;

    // CIM_JobSettingData features:
    Property<uint32> JobRunTimes;
    struct _RunMonth
    {
        enum
        {
            enum_January = 0,
            enum_February = 1,
            enum_March = 2,
            enum_April = 3,
            enum_May = 4,
            enum_June = 5,
            enum_July = 6,
            enum_August = 7,
            enum_September = 8,
            enum_October = 9,
            enum_November = 10,
            enum_December = 11,
        };
        uint8 value;
        uint8 null;
    }
    RunMonth;
    Property<sint8> RunDay;
    struct _RunDayOfWeek
    {
        enum
        {
            enum__Saturday = -7,
            enum__Friday = -6,
            enum__Thursday = -5,
            enum__Wednesday = -4,
            enum__Tuesday = -3,
            enum__Monday = -2,
            enum__Sunday = -1,
            enum_ExactDayOfMonth = 0,
            enum_Sunday = 1,
            enum_Monday = 2,
            enum_Tuesday = 3,
            enum_Wednesday = 4,
            enum_Thursday = 5,
            enum_Friday = 6,
            enum_Saturday = 7,
        };
        sint8 value;
        uint8 null;
    }
    RunDayOfWeek;
    Property<Datetime> RunStartInterval;
    struct _LocalOrUtcTime
    {
        enum
        {
            enum_Local_Time = 1,
            enum_UTC_Time = 2,
        };
        uint16 value;
        uint8 null;
    }
    LocalOrUtcTime;
    Property<Datetime> UntilTime;
    Property<String> Notify;
    Property<String> Owner;
    Property<uint32> Priority;
    Property<boolean> DeleteOnCompletion;
    struct _RecoveryAction
    {
        enum
        {
            enum_Unknown = 0,
            enum_Other = 1,
            enum_Do_Not_Continue = 2,
            enum_Continue_With_Next_Job = 3,
            enum_Re_run_Job = 4,
            enum_Run_Recovery_Job = 5,
        };
        uint16 value;
        uint8 null;
    }
    RecoveryAction;
    Property<String> OtherRecoveryAction;

    CIMPLE_CLASS(CIM_JobSettingData)
};

CIMPLE_NAMESPACE_END

#endif /* _cimple_CIM_JobSettingData_h */
