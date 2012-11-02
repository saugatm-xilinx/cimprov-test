/*NOCHKSRC*/
//==============================================================================
//
// PLEASE DO NOT EDIT; THIS FILE WAS AUTOMATICALLY GENERATED BY GENCLASS 2.0.24
//
//==============================================================================

#include <cimple/Meta_Class.h>
#include <cimple/Meta_Property.h>
#include <cimple/Meta_Reference.h>
#include "CIM_RecordForLog.h"

CIMPLE_NAMESPACE_BEGIN

using namespace cimple;

extern const Meta_Repository __meta_repository_52E780FCF95E157C8B11653275F6420A;

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
_CIM_RecordForLog_RecordFormat;

/*[1591]*/
const Meta_Property
_CIM_RecordForLog_RecordFormat =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "RecordFormat",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    STRING,
    0, /* subscript */
    CIMPLE_OFF(CIM_RecordForLog,RecordFormat),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_RecordForLog_RecordData;

/*[1591]*/
const Meta_Property
_CIM_RecordForLog_RecordData =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "RecordData",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    STRING,
    0, /* subscript */
    CIMPLE_OFF(CIM_RecordForLog,RecordData),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_RecordForLog_Locale;

/*[1591]*/
const Meta_Property
_CIM_RecordForLog_Locale =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "Locale",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    STRING,
    0, /* subscript */
    CIMPLE_OFF(CIM_RecordForLog,Locale),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_RecordForLog_PerceivedSeverity;

/*[1591]*/
const Meta_Property
_CIM_RecordForLog_PerceivedSeverity =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "PerceivedSeverity",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    UINT16,
    0, /* subscript */
    CIMPLE_OFF(CIM_RecordForLog,PerceivedSeverity),
    0, /* value */
};

/*[2291]*/
static Meta_Feature* _CIM_RecordForLog_MFA[] =
{
    (Meta_Feature*)(void*)&_CIM_ManagedElement_InstanceID,
    (Meta_Feature*)(void*)&_CIM_ManagedElement_Caption,
    (Meta_Feature*)(void*)&_CIM_ManagedElement_Description,
    (Meta_Feature*)(void*)&_CIM_ManagedElement_ElementName,
    (Meta_Feature*)(void*)&_CIM_RecordForLog_RecordFormat,
    (Meta_Feature*)(void*)&_CIM_RecordForLog_RecordData,
    (Meta_Feature*)(void*)&_CIM_RecordForLog_Locale,
    (Meta_Feature*)(void*)&_CIM_RecordForLog_PerceivedSeverity,
};

/*[2338]*/
static const Meta_Feature_Local _locals[] =
{
    {0},
    {0},
    {0},
    {0},
    {1},
    {1},
    {1},
    {1},
};

/*[2358]*/
const Meta_Class CIM_RecordForLog::static_meta_class =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_CLASS|CIMPLE_FLAG_ABSTRACT,
    "CIM_RecordForLog",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    _CIM_RecordForLog_MFA,
    CIMPLE_ARRAY_SIZE(_CIM_RecordForLog_MFA),
    sizeof(CIM_RecordForLog),
    _locals,
    &CIM_ManagedElement::static_meta_class,
    0, /* num_keys */
    &__meta_repository_52E780FCF95E157C8B11653275F6420A,
};

CIMPLE_NAMESPACE_END
