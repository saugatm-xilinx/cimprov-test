/*NOCHKSRC*/
//==============================================================================
//
// PLEASE DO NOT EDIT; THIS FILE WAS AUTOMATICALLY GENERATED BY GENCLASS 2.0.24
//
//==============================================================================

#ifndef _cimple_CIM_DiagnosticLog_h
#define _cimple_CIM_DiagnosticLog_h

#include <cimple/cimple.h>
#include "CIM_RecordLog.h"

CIMPLE_NAMESPACE_BEGIN

// CIM_DiagnosticLog keys:
//     InstanceID

/*[1194]*/
class CIM_DiagnosticLog : public Instance
{
public:
    // CIM_ManagedElement features:
    Property<String> InstanceID;
    Property<String> Caption;
    Property<String> Description;
    Property<String> ElementName;

    // CIM_ManagedSystemElement features:
    Property<Datetime> InstallDate;
    Property<String> Name;
    struct _OperationalStatus
    {
        enum
        {
            enum_Unknown = 0,
            enum_Other = 1,
            enum_OK = 2,
            enum_Degraded = 3,
            enum_Stressed = 4,
            enum_Predictive_Failure = 5,
            enum_Error = 6,
            enum_Non_Recoverable_Error = 7,
            enum_Starting = 8,
            enum_Stopping = 9,
            enum_Stopped = 10,
            enum_In_Service = 11,
            enum_No_Contact = 12,
            enum_Lost_Communication = 13,
            enum_Aborted = 14,
            enum_Dormant = 15,
            enum_Supporting_Entity_in_Error = 16,
            enum_Completed = 17,
            enum_Power_Mode = 18,
            enum_Relocating = 19,
            enum_DMTF_Reserved = 0,
            enum_Vendor_Reserved = 0,
        };
        Array_uint16 value;
        uint8 null;
    }
    OperationalStatus;
    Property<Array_String> StatusDescriptions;
    Property<String> Status;
    struct _HealthState
    {
        enum
        {
            enum_Unknown = 0,
            enum_OK = 5,
            enum_Degraded_Warning = 10,
            enum_Minor_failure = 15,
            enum_Major_failure = 20,
            enum_Critical_failure = 25,
            enum_Non_recoverable_error = 30,
            enum_DMTF_Reserved = 0,
            enum_Vendor_Specific = 32768,
        };
        uint16 value;
        uint8 null;
    }
    HealthState;
    struct _CommunicationStatus
    {
        enum
        {
            enum_Unknown = 0,
            enum_Not_Available = 1,
            enum_Communication_OK = 2,
            enum_Lost_Communication = 3,
            enum_No_Contact = 4,
            enum_DMTF_Reserved = 0,
            enum_Vendor_Reserved = 0,
        };
        uint16 value;
        uint8 null;
    }
    CommunicationStatus;
    struct _DetailedStatus
    {
        enum
        {
            enum_Not_Available = 0,
            enum_No_Additional_Information = 1,
            enum_Stressed = 2,
            enum_Predictive_Failure = 3,
            enum_Non_Recoverable_Error = 4,
            enum_Supporting_Entity_in_Error = 5,
            enum_DMTF_Reserved = 0,
            enum_Vendor_Reserved = 0,
        };
        uint16 value;
        uint8 null;
    }
    DetailedStatus;
    struct _OperatingStatus
    {
        enum
        {
            enum_Unknown = 0,
            enum_Not_Available = 1,
            enum_Servicing = 2,
            enum_Starting = 3,
            enum_Stopping = 4,
            enum_Stopped = 5,
            enum_Aborted = 6,
            enum_Dormant = 7,
            enum_Completed = 8,
            enum_Migrating = 9,
            enum_Emigrating = 10,
            enum_Immigrating = 11,
            enum_Snapshotting = 12,
            enum_Shutting_Down = 13,
            enum_In_Test = 14,
            enum_Transitioning = 15,
            enum_In_Service = 16,
            enum_DMTF_Reserved = 0,
            enum_Vendor_Reserved = 0,
        };
        uint16 value;
        uint8 null;
    }
    OperatingStatus;
    struct _PrimaryStatus
    {
        enum
        {
            enum_Unknown = 0,
            enum_OK = 1,
            enum_Degraded = 2,
            enum_Error = 3,
            enum_DMTF_Reserved = 0,
            enum_Vendor_Reserved = 0,
        };
        uint16 value;
        uint8 null;
    }
    PrimaryStatus;

    // CIM_LogicalElement features:

    // CIM_EnabledLogicalElement features:
    struct _EnabledState
    {
        enum
        {
            enum_Unknown = 0,
            enum_Other = 1,
            enum_Enabled = 2,
            enum_Disabled = 3,
            enum_Shutting_Down = 4,
            enum_Not_Applicable = 5,
            enum_Enabled_but_Offline = 6,
            enum_In_Test = 7,
            enum_Deferred = 8,
            enum_Quiesce = 9,
            enum_Starting = 10,
            enum_DMTF_Reserved = 11,
            enum_Vendor_Reserved = 32768,
        };
        uint16 value;
        uint8 null;
    }
    EnabledState;
    Property<String> OtherEnabledState;
    struct _RequestedState
    {
        enum
        {
            enum_Unknown = 0,
            enum_Enabled = 2,
            enum_Disabled = 3,
            enum_Shut_Down = 4,
            enum_No_Change = 5,
            enum_Offline = 6,
            enum_Test = 7,
            enum_Deferred = 8,
            enum_Quiesce = 9,
            enum_Reboot = 10,
            enum_Reset = 11,
            enum_Not_Applicable = 12,
            enum_DMTF_Reserved = 0,
            enum_Vendor_Reserved = 32768,
        };
        uint16 value;
        uint8 null;
    }
    RequestedState;
    struct _EnabledDefault
    {
        enum
        {
            enum_Enabled = 2,
            enum_Disabled = 3,
            enum_Not_Applicable = 5,
            enum_Enabled_but_Offline = 6,
            enum_No_Default = 7,
            enum_Quiesce = 9,
            enum_DMTF_Reserved = 0,
            enum_Vendor_Reserved = 32768,
        };
        uint16 value;
        uint8 null;
    }
    EnabledDefault;
    Property<Datetime> TimeOfLastStateChange;
    struct _AvailableRequestedStates
    {
        enum
        {
            enum_Enabled = 2,
            enum_Disabled = 3,
            enum_Shut_Down = 4,
            enum_Offline = 6,
            enum_Test = 7,
            enum_Defer = 8,
            enum_Quiesce = 9,
            enum_Reboot = 10,
            enum_Reset = 11,
            enum_DMTF_Reserved = 0,
        };
        Array_uint16 value;
        uint8 null;
    }
    AvailableRequestedStates;
    struct _TransitioningToState
    {
        enum
        {
            enum_Unknown = 0,
            enum_Enabled = 2,
            enum_Disabled = 3,
            enum_Shut_Down = 4,
            enum_No_Change = 5,
            enum_Offline = 6,
            enum_Test = 7,
            enum_Defer = 8,
            enum_Quiesce = 9,
            enum_Reboot = 10,
            enum_Reset = 11,
            enum_Not_Applicable = 12,
            enum_DMTF_Reserved = 0,
        };
        uint16 value;
        uint8 null;
    }
    TransitioningToState;

    // CIM_Log features:
    Property<uint64> MaxNumberOfRecords;
    Property<uint64> CurrentNumberOfRecords;
    struct _OverwritePolicy
    {
        enum
        {
            enum_Unknown = 0,
            enum_Wraps_When_Full = 2,
            enum_Never_Overwrites = 7,
            enum_DMTF_Reserved = 0,
            enum_Vendor_Reserved = 32768,
        };
        uint16 value;
        uint8 null;
    }
    OverwritePolicy;
    struct _LogState
    {
        enum
        {
            enum_Unknown = 0,
            enum_Normal = 2,
            enum_Erasing = 3,
            enum_Not_Applicable = 4,
            enum_DMTF_Reserved = 0,
            enum_Vendor_Reserved = 32768,
        };
        uint16 value;
        uint8 null;
    }
    LogState;

    // CIM_RecordLog features:

    // CIM_DiagnosticLog features:

    CIMPLE_CLASS(CIM_DiagnosticLog)
};

typedef CIM_EnabledLogicalElement_RequestStateChange_method CIM_DiagnosticLog_RequestStateChange_method;

typedef CIM_Log_ClearLog_method CIM_DiagnosticLog_ClearLog_method;

CIMPLE_NAMESPACE_END

#endif /* _cimple_CIM_DiagnosticLog_h */