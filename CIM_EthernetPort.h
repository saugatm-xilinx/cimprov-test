/*NOCHKSRC*/
//==============================================================================
//
// PLEASE DO NOT EDIT; THIS FILE WAS AUTOMATICALLY GENERATED BY GENCLASS 2.0.24
//
//==============================================================================

#ifndef _cimple_CIM_EthernetPort_h
#define _cimple_CIM_EthernetPort_h

#include <cimple/cimple.h>
#include "CIM_NetworkPort.h"

CIMPLE_NAMESPACE_BEGIN

// CIM_EthernetPort keys:
//     SystemCreationClassName
//     SystemName
//     CreationClassName
//     DeviceID

/*[1194]*/
class CIM_EthernetPort : public Instance
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

    // CIM_LogicalDevice features:
    Property<String> SystemCreationClassName;
    Property<String> SystemName;
    Property<String> CreationClassName;
    Property<String> DeviceID;
    Property<boolean> PowerManagementSupported;
    struct _PowerManagementCapabilities
    {
        enum
        {
            enum_Unknown = 0,
            enum_Not_Supported = 1,
            enum_Disabled = 2,
            enum_Enabled = 3,
            enum_Power_Saving_Modes_Entered_Automatically = 4,
            enum_Power_State_Settable = 5,
            enum_Power_Cycling_Supported = 6,
            enum_Timed_Power_On_Supported = 7,
        };
        Array_uint16 value;
        uint8 null;
    }
    PowerManagementCapabilities;
    struct _Availability
    {
        enum
        {
            enum_Other = 1,
            enum_Unknown = 2,
            enum_Running_Full_Power = 3,
            enum_Warning = 4,
            enum_In_Test = 5,
            enum_Not_Applicable = 6,
            enum_Power_Off = 7,
            enum_Off_Line = 8,
            enum_Off_Duty = 9,
            enum_Degraded = 10,
            enum_Not_Installed = 11,
            enum_Install_Error = 12,
            enum_Power_Save___Unknown = 13,
            enum_Power_Save___Low_Power_Mode = 14,
            enum_Power_Save___Standby = 15,
            enum_Power_Cycle = 16,
            enum_Power_Save___Warning = 17,
            enum_Paused = 18,
            enum_Not_Ready = 19,
            enum_Not_Configured = 20,
            enum_Quiesced = 21,
        };
        uint16 value;
        uint8 null;
    }
    Availability;
    struct _StatusInfo
    {
        enum
        {
            enum_Other = 1,
            enum_Unknown = 2,
            enum_Enabled = 3,
            enum_Disabled = 4,
            enum_Not_Applicable = 5,
        };
        uint16 value;
        uint8 null;
    }
    StatusInfo;
    Property<uint32> LastErrorCode;
    Property<String> ErrorDescription;
    Property<boolean> ErrorCleared;
    Property<Array_String> OtherIdentifyingInfo;
    Property<uint64> PowerOnHours;
    Property<uint64> TotalPowerOnHours;
    Property<Array_String> IdentifyingDescriptions;
    struct _AdditionalAvailability
    {
        enum
        {
            enum_Other = 1,
            enum_Unknown = 2,
            enum_Running_Full_Power = 3,
            enum_Warning = 4,
            enum_In_Test = 5,
            enum_Not_Applicable = 6,
            enum_Power_Off = 7,
            enum_Off_Line = 8,
            enum_Off_Duty = 9,
            enum_Degraded = 10,
            enum_Not_Installed = 11,
            enum_Install_Error = 12,
            enum_Power_Save___Unknown = 13,
            enum_Power_Save___Low_Power_Mode = 14,
            enum_Power_Save___Standby = 15,
            enum_Power_Cycle = 16,
            enum_Power_Save___Warning = 17,
            enum_Paused = 18,
            enum_Not_Ready = 19,
            enum_Not_Configured = 20,
            enum_Quiesced = 21,
        };
        Array_uint16 value;
        uint8 null;
    }
    AdditionalAvailability;
    Property<uint64> MaxQuiesceTime;

    // CIM_LogicalPort features:
    Property<uint64> Speed;
    Property<uint64> MaxSpeed;
    Property<uint64> RequestedSpeed;
    struct _UsageRestriction
    {
        enum
        {
            enum_Unknown = 0,
            enum_Front_end_only = 2,
            enum_Back_end_only = 3,
            enum_Not_restricted = 4,
        };
        uint16 value;
        uint8 null;
    }
    UsageRestriction;
    struct _PortType
    {
        enum
        {
            enum_Unknown = 0,
            enum_Other = 1,
            enum_Not_Applicable = 2,
            enum_DMTF_Reserved = 3,
            enum_Vendor_Reserved = 16000,
        };
        uint16 value;
        uint8 null;
    }
    PortType;
    Property<String> OtherPortType;

    // CIM_NetworkPort features:
    Property<String> OtherNetworkPortType;
    Property<uint16> PortNumber;
    struct _LinkTechnology
    {
        enum
        {
            enum_Unknown = 0,
            enum_Other = 1,
            enum_Ethernet = 2,
            enum_IB = 3,
            enum_FC = 4,
            enum_FDDI = 5,
            enum_ATM = 6,
            enum_Token_Ring = 7,
            enum_Frame_Relay = 8,
            enum_Infrared = 9,
            enum_BlueTooth = 10,
            enum_Wireless_LAN = 11,
        };
        uint16 value;
        uint8 null;
    }
    LinkTechnology;
    Property<String> OtherLinkTechnology;
    Property<String> PermanentAddress;
    Property<Array_String> NetworkAddresses;
    Property<boolean> FullDuplex;
    Property<boolean> AutoSense;
    Property<uint64> SupportedMaximumTransmissionUnit;
    Property<uint64> ActiveMaximumTransmissionUnit;

    // CIM_EthernetPort features:
    Property<uint32> MaxDataSize;
    struct _Capabilities
    {
        enum
        {
            enum_Unknown = 0,
            enum_Other = 1,
            enum_AlertOnLan = 2,
            enum_WakeOnLan = 3,
            enum_FailOver = 4,
            enum_LoadBalancing = 5,
        };
        Array_uint16 value;
        uint8 null;
    }
    Capabilities;
    Property<Array_String> CapabilityDescriptions;
    struct _EnabledCapabilities
    {
        enum
        {
            enum_Unknown = 0,
            enum_Other = 1,
            enum_AlertOnLan = 2,
            enum_WakeOnLan = 3,
            enum_FailOver = 4,
            enum_LoadBalancing = 5,
        };
        Array_uint16 value;
        uint8 null;
    }
    EnabledCapabilities;
    Property<Array_String> OtherEnabledCapabilities;

    CIMPLE_CLASS(CIM_EthernetPort)
};

typedef CIM_EnabledLogicalElement_RequestStateChange_method CIM_EthernetPort_RequestStateChange_method;

typedef CIM_LogicalDevice_SetPowerState_method CIM_EthernetPort_SetPowerState_method;

typedef CIM_LogicalDevice_Reset_method CIM_EthernetPort_Reset_method;

typedef CIM_LogicalDevice_EnableDevice_method CIM_EthernetPort_EnableDevice_method;

typedef CIM_LogicalDevice_OnlineDevice_method CIM_EthernetPort_OnlineDevice_method;

typedef CIM_LogicalDevice_QuiesceDevice_method CIM_EthernetPort_QuiesceDevice_method;

typedef CIM_LogicalDevice_SaveProperties_method CIM_EthernetPort_SaveProperties_method;

typedef CIM_LogicalDevice_RestoreProperties_method CIM_EthernetPort_RestoreProperties_method;

CIMPLE_NAMESPACE_END

#endif /* _cimple_CIM_EthernetPort_h */
