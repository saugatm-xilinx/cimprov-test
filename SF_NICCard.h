/*NOCHKSRC*/
//==============================================================================
//
// PLEASE DO NOT EDIT; THIS FILE WAS AUTOMATICALLY GENERATED BY GENCLASS 2.0.24
//
//==============================================================================

#ifndef _cimple_SF_NICCard_h
#define _cimple_SF_NICCard_h

#include <cimple/cimple.h>
#include "CIM_Card.h"

CIMPLE_NAMESPACE_BEGIN

// SF_NICCard keys:
//     Tag
//     CreationClassName

/*[1194]*/
class SF_NICCard : public Instance
{
public:
    // CIM_ManagedElement features:
    Property<String> InstanceID;
    Property<String> Caption;
    Property<String> Description;
    Property<String> ElementName;
    Property<uint64> Generation;

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

    // CIM_PhysicalElement features:
    Property<String> Tag;
    Property<String> CreationClassName;
    Property<String> Manufacturer;
    Property<String> Model;
    Property<String> SKU;
    Property<String> SerialNumber;
    Property<String> Version;
    Property<String> PartNumber;
    Property<String> OtherIdentifyingInfo;
    Property<boolean> PoweredOn;
    Property<Datetime> ManufactureDate;
    Property<String> VendorEquipmentType;
    Property<String> UserTracking;
    Property<boolean> CanBeFRUed;

    // CIM_PhysicalPackage features:
    struct _RemovalConditions
    {
        enum
        {
            enum_Unknown = 0,
            enum_Not_Applicable = 2,
            enum_Removable_when_off = 3,
            enum_Removable_when_on_or_off = 4,
        };
        uint16 value;
        uint8 null;
    }
    RemovalConditions;
    Property<boolean> Removable;
    Property<boolean> Replaceable;
    Property<boolean> HotSwappable;
    Property<real32> Height;
    Property<real32> Depth;
    Property<real32> Width;
    Property<real32> Weight;
    struct _PackageType
    {
        enum
        {
            enum_Unknown = 0,
            enum_Other = 1,
            enum_Rack = 2,
            enum_Chassis_Frame = 3,
            enum_Cross_Connect_Backplane = 4,
            enum_Container_Frame_Slot = 5,
            enum_Power_Supply = 6,
            enum_Fan = 7,
            enum_Sensor = 8,
            enum_Module_Card = 9,
            enum_Port_Connector = 10,
            enum_Battery = 11,
            enum_Processor = 12,
            enum_Memory = 13,
            enum_Power_Source_Generator = 14,
            enum_Storage_Media_Package__e_g___Disk_or_Tape_Drive_ = 15,
            enum_Blade = 16,
            enum_Blade_Expansion = 17,
        };
        uint16 value;
        uint8 null;
    }
    PackageType;
    Property<String> OtherPackageType;
    Property<Array_String> VendorCompatibilityStrings;

    // CIM_Card features:
    Property<boolean> HostingBoard;
    Property<String> SlotLayout;
    Property<boolean> RequiresDaughterBoard;
    Property<boolean> SpecialRequirements;
    Property<String> RequirementsDescription;
    Property<Array_sint16> OperatingVoltages;

    // SF_NICCard features:

    CIMPLE_CLASS(SF_NICCard)
};

typedef CIM_PhysicalPackage_IsCompatible_method SF_NICCard_IsCompatible_method;

typedef CIM_Card_ConnectorPower_method SF_NICCard_ConnectorPower_method;

CIMPLE_NAMESPACE_END

#endif /* _cimple_SF_NICCard_h */
