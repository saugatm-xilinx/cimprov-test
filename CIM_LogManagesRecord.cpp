/*NOCHKSRC*/
//==============================================================================
//
// PLEASE DO NOT EDIT; THIS FILE WAS AUTOMATICALLY GENERATED BY GENCLASS 2.0.24
//
//==============================================================================

#include <cimple/Meta_Class.h>
#include <cimple/Meta_Property.h>
#include <cimple/Meta_Reference.h>
#include "CIM_LogManagesRecord.h"

CIMPLE_NAMESPACE_BEGIN

using namespace cimple;

extern const Meta_Repository __meta_repository_52E780FCF95E157C8B11653275F6420A;

/*[1653]*/
extern const Meta_Reference
_CIM_LogManagesRecord_Log;

/*[1664]*/
const Meta_Reference
_CIM_LogManagesRecord_Log =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_REFERENCE|CIMPLE_FLAG_KEY|CIMPLE_FLAG_AGGREGATE,
    "Log",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    0, /* subscript */
    &CIM_Log::static_meta_class,
    CIMPLE_OFF(CIM_LogManagesRecord,Log)
};

/*[1653]*/
extern const Meta_Reference
_CIM_LogManagesRecord_Record;

/*[1664]*/
const Meta_Reference
_CIM_LogManagesRecord_Record =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_REFERENCE|CIMPLE_FLAG_KEY,
    "Record",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    0, /* subscript */
    &CIM_RecordForLog::static_meta_class,
    CIMPLE_OFF(CIM_LogManagesRecord,Record)
};

/*[2291]*/
static Meta_Feature* _CIM_LogManagesRecord_MFA[] =
{
    (Meta_Feature*)(void*)&_CIM_LogManagesRecord_Log,
    (Meta_Feature*)(void*)&_CIM_LogManagesRecord_Record,
};

/*[2338]*/
static const Meta_Feature_Local _locals[] =
{
    {1},
    {1},
};

/*[2358]*/
const Meta_Class CIM_LogManagesRecord::static_meta_class =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_ASSOCIATION|CIMPLE_FLAG_AGGREGATION,
    "CIM_LogManagesRecord",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    _CIM_LogManagesRecord_MFA,
    CIMPLE_ARRAY_SIZE(_CIM_LogManagesRecord_MFA),
    sizeof(CIM_LogManagesRecord),
    _locals,
    0, /* super_class */ 
    2, /* num_keys */
    &__meta_repository_52E780FCF95E157C8B11653275F6420A,
};

CIMPLE_NAMESPACE_END

