/*NOCHKSRC*/
//==============================================================================
//
// PLEASE DO NOT EDIT; THIS FILE WAS AUTOMATICALLY GENERATED BY GENCLASS 2.0.24
//
//==============================================================================

#ifndef _cimple_CIM_DiagnosticServiceRecord_h
#define _cimple_CIM_DiagnosticServiceRecord_h

#include <cimple/cimple.h>
#include "CIM_DiagnosticRecord.h"

CIMPLE_NAMESPACE_BEGIN

// CIM_DiagnosticServiceRecord keys:
//     InstanceID

/*[1194]*/
class CIM_DiagnosticServiceRecord : public Instance
{
public:
    // CIM_ManagedElement features:
    Property<String> InstanceID;
    Property<String> Caption;
    Property<String> Description;
    Property<String> ElementName;
    Property<uint64> Generation;

    // CIM_RecordForLog features:
    Property<String> RecordFormat;
    Property<String> RecordData;
    Property<String> Locale;
    struct _PerceivedSeverity
    {
        enum
        {
            enum_Unknown = 0,
            enum_Other = 1,
            enum_Information = 2,
            enum_Degraded_Warning = 3,
            enum_Minor = 4,
            enum_Major = 5,
            enum_Critical = 6,
            enum_Fatal_NonRecoverable = 7,
        };
        uint16 value;
        uint8 null;
    }
    PerceivedSeverity;

    // CIM_LogEntry features:
    Property<String> LogInstanceID;
    Property<String> LogName;
    Property<String> RecordID;
    Property<Datetime> CreationTimeStamp;
    Property<String> OwningEntity;
    Property<String> MessageID;
    Property<String> Message;
    Property<Array_String> MessageArguments;

    // CIM_DiagnosticRecord features:
    Property<String> ServiceName;
    Property<String> ManagedElementName;
    Property<Datetime> ExpirationDate;
    struct _RecordType
    {
        enum
        {
            enum_Unknown = 0,
            enum_Other = 1,
            enum_Results = 2,
            enum_Subtests = 3,
            enum_Actions = 4,
            enum_Warnings = 5,
            enum_Status = 6,
            enum_Device_Errors = 7,
            enum_Service_Errors = 8,
            enum_Setting_Data = 9,
            enum_Statistics = 10,
            enum_Hardware_Configuration = 11,
            enum_Software_Configuration = 12,
            enum_References = 13,
            enum_Debug = 14,
        };
        uint16 value;
        uint8 null;
    }
    RecordType;
    Property<String> OtherRecordTypeDescription;

    // CIM_DiagnosticServiceRecord features:
    Property<Array_String> ErrorCode;
    Property<Array_uint32> ErrorCount;
    Property<uint32> LoopsFailed;
    Property<uint32> LoopsPassed;

    CIMPLE_CLASS(CIM_DiagnosticServiceRecord)
};

CIMPLE_NAMESPACE_END

#endif /* _cimple_CIM_DiagnosticServiceRecord_h */
