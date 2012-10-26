/*NOCHKSRC*/
//==============================================================================
//
// PLEASE DO NOT EDIT; THIS FILE WAS AUTOMATICALLY GENERATED BY GENCLASS 2.0.24
//
//==============================================================================

#include <cimple/Meta_Class.h>
#include <cimple/Meta_Property.h>
#include <cimple/Meta_Reference.h>
#include "CIM_LogEntry.h"

CIMPLE_NAMESPACE_BEGIN

using namespace cimple;

extern const Meta_Repository __meta_repository_52E780FCF95E157C8B11653275F6420A;

/*[1568]*/
extern const Meta_Property
_CIM_LogEntry_InstanceID;

/*[1591]*/
const Meta_Property
_CIM_LogEntry_InstanceID =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_KEY|CIMPLE_FLAG_READ,
    "InstanceID",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    STRING,
    0, /* subscript */
    CIMPLE_OFF(CIM_LogEntry,InstanceID),
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
_CIM_LogEntry_LogInstanceID;

/*[1591]*/
const Meta_Property
_CIM_LogEntry_LogInstanceID =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "LogInstanceID",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    STRING,
    0, /* subscript */
    CIMPLE_OFF(CIM_LogEntry,LogInstanceID),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_LogEntry_LogName;

/*[1591]*/
const Meta_Property
_CIM_LogEntry_LogName =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "LogName",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    STRING,
    0, /* subscript */
    CIMPLE_OFF(CIM_LogEntry,LogName),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_LogEntry_RecordID;

/*[1591]*/
const Meta_Property
_CIM_LogEntry_RecordID =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "RecordID",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    STRING,
    0, /* subscript */
    CIMPLE_OFF(CIM_LogEntry,RecordID),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_LogEntry_CreationTimeStamp;

/*[1591]*/
const Meta_Property
_CIM_LogEntry_CreationTimeStamp =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "CreationTimeStamp",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    DATETIME,
    0, /* subscript */
    CIMPLE_OFF(CIM_LogEntry,CreationTimeStamp),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_LogEntry_MessageID;

/*[1591]*/
const Meta_Property
_CIM_LogEntry_MessageID =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "MessageID",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    STRING,
    0, /* subscript */
    CIMPLE_OFF(CIM_LogEntry,MessageID),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_LogEntry_Message;

/*[1591]*/
const Meta_Property
_CIM_LogEntry_Message =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "Message",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    STRING,
    0, /* subscript */
    CIMPLE_OFF(CIM_LogEntry,Message),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_LogEntry_MessageArguments;

/*[1591]*/
const Meta_Property
_CIM_LogEntry_MessageArguments =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "MessageArguments",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    STRING,
    -1, /* subscript */
    CIMPLE_OFF(CIM_LogEntry,MessageArguments),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_LogEntry_OwningEntity;

/*[1591]*/
const Meta_Property
_CIM_LogEntry_OwningEntity =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "OwningEntity",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    STRING,
    0, /* subscript */
    CIMPLE_OFF(CIM_LogEntry,OwningEntity),
    0, /* value */
};

/*[2291]*/
static Meta_Feature* _CIM_LogEntry_MFA[] =
{
    (Meta_Feature*)(void*)&_CIM_LogEntry_InstanceID,
    (Meta_Feature*)(void*)&_CIM_ManagedElement_Caption,
    (Meta_Feature*)(void*)&_CIM_ManagedElement_Description,
    (Meta_Feature*)(void*)&_CIM_ManagedElement_ElementName,
    (Meta_Feature*)(void*)&_CIM_RecordForLog_RecordFormat,
    (Meta_Feature*)(void*)&_CIM_RecordForLog_RecordData,
    (Meta_Feature*)(void*)&_CIM_RecordForLog_Locale,
    (Meta_Feature*)(void*)&_CIM_RecordForLog_PerceivedSeverity,
    (Meta_Feature*)(void*)&_CIM_LogEntry_LogInstanceID,
    (Meta_Feature*)(void*)&_CIM_LogEntry_LogName,
    (Meta_Feature*)(void*)&_CIM_LogEntry_RecordID,
    (Meta_Feature*)(void*)&_CIM_LogEntry_CreationTimeStamp,
    (Meta_Feature*)(void*)&_CIM_LogEntry_MessageID,
    (Meta_Feature*)(void*)&_CIM_LogEntry_Message,
    (Meta_Feature*)(void*)&_CIM_LogEntry_MessageArguments,
    (Meta_Feature*)(void*)&_CIM_LogEntry_OwningEntity,
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
const Meta_Class CIM_LogEntry::static_meta_class =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_CLASS,
    "CIM_LogEntry",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    _CIM_LogEntry_MFA,
    CIMPLE_ARRAY_SIZE(_CIM_LogEntry_MFA),
    sizeof(CIM_LogEntry),
    _locals,
    &CIM_RecordForLog::static_meta_class,
    1, /* num_keys */
    &__meta_repository_52E780FCF95E157C8B11653275F6420A,
};

CIMPLE_NAMESPACE_END

