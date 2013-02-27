/*NOCHKSRC*/
//==============================================================================
//
// PLEASE DO NOT EDIT; THIS FILE WAS AUTOMATICALLY GENERATED BY GENCLASS 2.0.24
//
//==============================================================================

#include <cimple/Meta_Class.h>
#include <cimple/Meta_Property.h>
#include <cimple/Meta_Reference.h>
#include "CIM_DiagnosticSettingData.h"

CIMPLE_NAMESPACE_BEGIN

using namespace cimple;

extern const Meta_Repository __meta_repository_0DDA66E0FF471BAB95C366A9360C9285;

/*[1568]*/
extern const Meta_Property
_CIM_SettingData_InstanceID;

/*[1568]*/
extern const Meta_Property
_CIM_ManagedElement_Caption;

/*[1568]*/
extern const Meta_Property
_CIM_ManagedElement_Description;

/*[1568]*/
extern const Meta_Property
_CIM_SettingData_ElementName;

/*[1568]*/
extern const Meta_Property
_CIM_ManagedElement_Generation;

/*[1568]*/
extern const Meta_Property
_CIM_SettingData_ConfigurationName;

/*[1568]*/
extern const Meta_Property
_CIM_SettingData_ChangeableType;

/*[1568]*/
extern const Meta_Property
_CIM_DiagnosticSettingData_HaltOnError;

/*[1591]*/
const Meta_Property
_CIM_DiagnosticSettingData_HaltOnError =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "HaltOnError",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    BOOLEAN,
    0, /* subscript */
    CIMPLE_OFF(CIM_DiagnosticSettingData,HaltOnError),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_DiagnosticSettingData_QuickMode;

/*[1591]*/
const Meta_Property
_CIM_DiagnosticSettingData_QuickMode =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "QuickMode",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    BOOLEAN,
    0, /* subscript */
    CIMPLE_OFF(CIM_DiagnosticSettingData,QuickMode),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_DiagnosticSettingData_PercentOfTestCoverage;

/*[1591]*/
const Meta_Property
_CIM_DiagnosticSettingData_PercentOfTestCoverage =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "PercentOfTestCoverage",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    UINT8,
    0, /* subscript */
    CIMPLE_OFF(CIM_DiagnosticSettingData,PercentOfTestCoverage),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_DiagnosticSettingData_LoopControlParameter;

/*[1591]*/
const Meta_Property
_CIM_DiagnosticSettingData_LoopControlParameter =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "LoopControlParameter",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    STRING,
    -1, /* subscript */
    CIMPLE_OFF(CIM_DiagnosticSettingData,LoopControlParameter),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_DiagnosticSettingData_LoopControl;

/*[1591]*/
const Meta_Property
_CIM_DiagnosticSettingData_LoopControl =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "LoopControl",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    UINT16,
    -1, /* subscript */
    CIMPLE_OFF(CIM_DiagnosticSettingData,LoopControl),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_DiagnosticSettingData_OtherLoopControlDescriptions;

/*[1591]*/
const Meta_Property
_CIM_DiagnosticSettingData_OtherLoopControlDescriptions =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "OtherLoopControlDescriptions",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    STRING,
    -1, /* subscript */
    CIMPLE_OFF(CIM_DiagnosticSettingData,OtherLoopControlDescriptions),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_DiagnosticSettingData_ResultPersistence;

/*[1591]*/
const Meta_Property
_CIM_DiagnosticSettingData_ResultPersistence =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "ResultPersistence",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    UINT32,
    0, /* subscript */
    CIMPLE_OFF(CIM_DiagnosticSettingData,ResultPersistence),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_DiagnosticSettingData_LogOptions;

/*[1591]*/
const Meta_Property
_CIM_DiagnosticSettingData_LogOptions =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "LogOptions",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    UINT16,
    -1, /* subscript */
    CIMPLE_OFF(CIM_DiagnosticSettingData,LogOptions),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_DiagnosticSettingData_OtherLogOptionsDescriptions;

/*[1591]*/
const Meta_Property
_CIM_DiagnosticSettingData_OtherLogOptionsDescriptions =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "OtherLogOptionsDescriptions",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    STRING,
    -1, /* subscript */
    CIMPLE_OFF(CIM_DiagnosticSettingData,OtherLogOptionsDescriptions),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_DiagnosticSettingData_LogStorage;

/*[1591]*/
const Meta_Property
_CIM_DiagnosticSettingData_LogStorage =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "LogStorage",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    UINT16,
    -1, /* subscript */
    CIMPLE_OFF(CIM_DiagnosticSettingData,LogStorage),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_DiagnosticSettingData_OtherLogStorageDescriptions;

/*[1591]*/
const Meta_Property
_CIM_DiagnosticSettingData_OtherLogStorageDescriptions =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "OtherLogStorageDescriptions",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    STRING,
    -1, /* subscript */
    CIMPLE_OFF(CIM_DiagnosticSettingData,OtherLogStorageDescriptions),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_DiagnosticSettingData_VerbosityLevel;

/*[1591]*/
const Meta_Property
_CIM_DiagnosticSettingData_VerbosityLevel =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "VerbosityLevel",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    UINT16,
    -1, /* subscript */
    CIMPLE_OFF(CIM_DiagnosticSettingData,VerbosityLevel),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_DiagnosticSettingData_QueryTimeout;

/*[540]*/
static const Meta_Value_Scalar<uint32>
_CIM_DiagnosticSettingData_QueryTimeout_MV =
{
    CIMPLE_UINT32_LITERAL(0)
};

/*[1591]*/
const Meta_Property
_CIM_DiagnosticSettingData_QueryTimeout =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "QueryTimeout",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    UINT32,
    0, /* subscript */
    CIMPLE_OFF(CIM_DiagnosticSettingData,QueryTimeout),
    (const Meta_Value*)(void*)&_CIM_DiagnosticSettingData_QueryTimeout_MV,
};

/*[1568]*/
extern const Meta_Property
_CIM_DiagnosticSettingData_NonDestructive;

/*[540]*/
static const Meta_Value_Scalar<boolean>
_CIM_DiagnosticSettingData_NonDestructive_MV =
{
    false
};

/*[1591]*/
const Meta_Property
_CIM_DiagnosticSettingData_NonDestructive =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "NonDestructive",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    BOOLEAN,
    0, /* subscript */
    CIMPLE_OFF(CIM_DiagnosticSettingData,NonDestructive),
    (const Meta_Value*)(void*)&_CIM_DiagnosticSettingData_NonDestructive_MV,
};

/*[2291]*/
static Meta_Feature* _CIM_DiagnosticSettingData_MFA[] =
{
    (Meta_Feature*)(void*)&_CIM_SettingData_InstanceID,
    (Meta_Feature*)(void*)&_CIM_ManagedElement_Caption,
    (Meta_Feature*)(void*)&_CIM_ManagedElement_Description,
    (Meta_Feature*)(void*)&_CIM_SettingData_ElementName,
    (Meta_Feature*)(void*)&_CIM_ManagedElement_Generation,
    (Meta_Feature*)(void*)&_CIM_SettingData_ConfigurationName,
    (Meta_Feature*)(void*)&_CIM_SettingData_ChangeableType,
    (Meta_Feature*)(void*)&_CIM_DiagnosticSettingData_HaltOnError,
    (Meta_Feature*)(void*)&_CIM_DiagnosticSettingData_QuickMode,
    (Meta_Feature*)(void*)&_CIM_DiagnosticSettingData_PercentOfTestCoverage,
    (Meta_Feature*)(void*)&_CIM_DiagnosticSettingData_LoopControlParameter,
    (Meta_Feature*)(void*)&_CIM_DiagnosticSettingData_LoopControl,
    (Meta_Feature*)(void*)&_CIM_DiagnosticSettingData_OtherLoopControlDescriptions,
    (Meta_Feature*)(void*)&_CIM_DiagnosticSettingData_ResultPersistence,
    (Meta_Feature*)(void*)&_CIM_DiagnosticSettingData_LogOptions,
    (Meta_Feature*)(void*)&_CIM_DiagnosticSettingData_OtherLogOptionsDescriptions,
    (Meta_Feature*)(void*)&_CIM_DiagnosticSettingData_LogStorage,
    (Meta_Feature*)(void*)&_CIM_DiagnosticSettingData_OtherLogStorageDescriptions,
    (Meta_Feature*)(void*)&_CIM_DiagnosticSettingData_VerbosityLevel,
    (Meta_Feature*)(void*)&_CIM_DiagnosticSettingData_QueryTimeout,
    (Meta_Feature*)(void*)&_CIM_DiagnosticSettingData_NonDestructive,
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
    {1},
    {1},
    {1},
    {1},
    {1},
    {1},
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
const Meta_Class CIM_DiagnosticSettingData::static_meta_class =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_CLASS|CIMPLE_FLAG_EXPERIMENTAL,
    "CIM_DiagnosticSettingData",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    _CIM_DiagnosticSettingData_MFA,
    CIMPLE_ARRAY_SIZE(_CIM_DiagnosticSettingData_MFA),
    sizeof(CIM_DiagnosticSettingData),
    _locals,
    &CIM_SettingData::static_meta_class,
    1, /* num_keys */
    &__meta_repository_0DDA66E0FF471BAB95C366A9360C9285,
};

CIMPLE_NAMESPACE_END

