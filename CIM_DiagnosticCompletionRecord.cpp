/*NOCHKSRC*/
//==============================================================================
//
// PLEASE DO NOT EDIT; THIS FILE WAS AUTOMATICALLY GENERATED BY GENCLASS 2.0.24
//
//==============================================================================

#include <cimple/Meta_Class.h>
#include <cimple/Meta_Property.h>
#include <cimple/Meta_Reference.h>
#include "CIM_DiagnosticCompletionRecord.h"

CIMPLE_NAMESPACE_BEGIN

using namespace cimple;

extern const Meta_Repository __meta_repository_52E780FCF95E157C8B11653275F6420A;

/*[1568]*/
extern const Meta_Property
_CIM_DiagnosticRecord_InstanceID;

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
_CIM_RecordForLog_RecordFormat;

/*[1568]*/
extern const Meta_Property
_CIM_RecordForLog_RecordData;

/*[1568]*/
extern const Meta_Property
_CIM_RecordForLog_Locale;

/*[1568]*/
extern const Meta_Property
_CIM_RecordForLog_PerceivedSeverity;

/*[1568]*/
extern const Meta_Property
_CIM_DiagnosticRecord_ServiceName;

/*[1568]*/
extern const Meta_Property
_CIM_DiagnosticRecord_ManagedElementName;

/*[1568]*/
extern const Meta_Property
_CIM_DiagnosticRecord_ExpirationDate;

/*[1568]*/
extern const Meta_Property
_CIM_DiagnosticRecord_RecordType;

/*[1568]*/
extern const Meta_Property
_CIM_DiagnosticRecord_OtherRecordTypeDescription;

/*[1568]*/
extern const Meta_Property
_CIM_DiagnosticRecord_CreationTimeStamp;

/*[1568]*/
extern const Meta_Property
_CIM_DiagnosticServiceRecord_ErrorCode;

/*[1568]*/
extern const Meta_Property
_CIM_DiagnosticServiceRecord_ErrorCount;

/*[1568]*/
extern const Meta_Property
_CIM_DiagnosticServiceRecord_LoopsFailed;

/*[1568]*/
extern const Meta_Property
_CIM_DiagnosticServiceRecord_LoopsPassed;

/*[1568]*/
extern const Meta_Property
_CIM_DiagnosticCompletionRecord_CompletionState;

/*[1591]*/
const Meta_Property
_CIM_DiagnosticCompletionRecord_CompletionState =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "CompletionState",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    UINT16,
    0, /* subscript */
    CIMPLE_OFF(CIM_DiagnosticCompletionRecord,CompletionState),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_DiagnosticCompletionRecord_OtherCompletionStateDescription;

/*[1591]*/
const Meta_Property
_CIM_DiagnosticCompletionRecord_OtherCompletionStateDescription =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "OtherCompletionStateDescription",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    STRING,
    0, /* subscript */
    CIMPLE_OFF(CIM_DiagnosticCompletionRecord,OtherCompletionStateDescription),
    0, /* value */
};

/*[2291]*/
static Meta_Feature* _CIM_DiagnosticCompletionRecord_MFA[] =
{
    (Meta_Feature*)(void*)&_CIM_DiagnosticRecord_InstanceID,
    (Meta_Feature*)(void*)&_CIM_ManagedElement_Caption,
    (Meta_Feature*)(void*)&_CIM_ManagedElement_Description,
    (Meta_Feature*)(void*)&_CIM_ManagedElement_ElementName,
    (Meta_Feature*)(void*)&_CIM_RecordForLog_RecordFormat,
    (Meta_Feature*)(void*)&_CIM_RecordForLog_RecordData,
    (Meta_Feature*)(void*)&_CIM_RecordForLog_Locale,
    (Meta_Feature*)(void*)&_CIM_RecordForLog_PerceivedSeverity,
    (Meta_Feature*)(void*)&_CIM_DiagnosticRecord_ServiceName,
    (Meta_Feature*)(void*)&_CIM_DiagnosticRecord_ManagedElementName,
    (Meta_Feature*)(void*)&_CIM_DiagnosticRecord_ExpirationDate,
    (Meta_Feature*)(void*)&_CIM_DiagnosticRecord_RecordType,
    (Meta_Feature*)(void*)&_CIM_DiagnosticRecord_OtherRecordTypeDescription,
    (Meta_Feature*)(void*)&_CIM_DiagnosticRecord_CreationTimeStamp,
    (Meta_Feature*)(void*)&_CIM_DiagnosticServiceRecord_ErrorCode,
    (Meta_Feature*)(void*)&_CIM_DiagnosticServiceRecord_ErrorCount,
    (Meta_Feature*)(void*)&_CIM_DiagnosticServiceRecord_LoopsFailed,
    (Meta_Feature*)(void*)&_CIM_DiagnosticServiceRecord_LoopsPassed,
    (Meta_Feature*)(void*)&_CIM_DiagnosticCompletionRecord_CompletionState,
    (Meta_Feature*)(void*)&_CIM_DiagnosticCompletionRecord_OtherCompletionStateDescription,
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
    {1},
    {1},
};

/*[2358]*/
const Meta_Class CIM_DiagnosticCompletionRecord::static_meta_class =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_CLASS,
    "CIM_DiagnosticCompletionRecord",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    _CIM_DiagnosticCompletionRecord_MFA,
    CIMPLE_ARRAY_SIZE(_CIM_DiagnosticCompletionRecord_MFA),
    sizeof(CIM_DiagnosticCompletionRecord),
    _locals,
    &CIM_DiagnosticServiceRecord::static_meta_class,
    1, /* num_keys */
    &__meta_repository_52E780FCF95E157C8B11653275F6420A,
};

CIMPLE_NAMESPACE_END
