/*NOCHKSRC*/
//==============================================================================
//
// PLEASE DO NOT EDIT; THIS FILE WAS AUTOMATICALLY GENERATED BY GENCLASS 2.0.24
//
//==============================================================================

#include <cimple/Meta_Class.h>
#include <cimple/Meta_Property.h>
#include <cimple/Meta_Reference.h>
#include "CIM_UseOfLog.h"

CIMPLE_NAMESPACE_BEGIN

using namespace cimple;

extern const Meta_Repository __meta_repository_52E780FCF95E157C8B11653275F6420A;

/*[1653]*/
extern const Meta_Reference
_CIM_UseOfLog_Antecedent;

/*[1664]*/
const Meta_Reference
_CIM_UseOfLog_Antecedent =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_REFERENCE|CIMPLE_FLAG_KEY,
    "Antecedent",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    0, /* subscript */
    &CIM_Log::static_meta_class,
    CIMPLE_OFF(CIM_UseOfLog,Antecedent)
};

/*[1653]*/
extern const Meta_Reference
_CIM_UseOfLog_Dependent;

/*[1664]*/
const Meta_Reference
_CIM_UseOfLog_Dependent =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_REFERENCE|CIMPLE_FLAG_KEY,
    "Dependent",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    0, /* subscript */
    &CIM_ManagedSystemElement::static_meta_class,
    CIMPLE_OFF(CIM_UseOfLog,Dependent)
};

/*[1568]*/
extern const Meta_Property
_CIM_UseOfLog_RecordedData;

/*[1591]*/
const Meta_Property
_CIM_UseOfLog_RecordedData =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "RecordedData",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    STRING,
    0, /* subscript */
    CIMPLE_OFF(CIM_UseOfLog,RecordedData),
    0, /* value */
};

/*[2291]*/
static Meta_Feature* _CIM_UseOfLog_MFA[] =
{
    (Meta_Feature*)(void*)&_CIM_UseOfLog_Antecedent,
    (Meta_Feature*)(void*)&_CIM_UseOfLog_Dependent,
    (Meta_Feature*)(void*)&_CIM_UseOfLog_RecordedData,
};

/*[2338]*/
static const Meta_Feature_Local _locals[] =
{
    {1},
    {1},
    {1},
};

/*[2358]*/
const Meta_Class CIM_UseOfLog::static_meta_class =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_ASSOCIATION,
    "CIM_UseOfLog",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    _CIM_UseOfLog_MFA,
    CIMPLE_ARRAY_SIZE(_CIM_UseOfLog_MFA),
    sizeof(CIM_UseOfLog),
    _locals,
    &CIM_Dependency::static_meta_class,
    2, /* num_keys */
    &__meta_repository_52E780FCF95E157C8B11653275F6420A,
};

CIMPLE_NAMESPACE_END
