/*NOCHKSRC*/
//==============================================================================
//
// PLEASE DO NOT EDIT; THIS FILE WAS AUTOMATICALLY GENERATED BY GENCLASS 2.0.24
//
//==============================================================================

#include <cimple/Meta_Class.h>
#include <cimple/Meta_Property.h>
#include <cimple/Meta_Reference.h>
#include "CIM_ElementSettingData.h"

CIMPLE_NAMESPACE_BEGIN

using namespace cimple;

extern const Meta_Repository __meta_repository_480038F9449B17AC9E2210119CF38AF2;

/*[1653]*/
extern const Meta_Reference
_CIM_ElementSettingData_ManagedElement;

/*[1664]*/
const Meta_Reference
_CIM_ElementSettingData_ManagedElement =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_REFERENCE|CIMPLE_FLAG_KEY,
    "ManagedElement",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    0, /* subscript */
    &CIM_ManagedElement::static_meta_class,
    CIMPLE_OFF(CIM_ElementSettingData,ManagedElement)
};

/*[1653]*/
extern const Meta_Reference
_CIM_ElementSettingData_SettingData;

/*[1664]*/
const Meta_Reference
_CIM_ElementSettingData_SettingData =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_REFERENCE|CIMPLE_FLAG_KEY,
    "SettingData",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    0, /* subscript */
    &CIM_SettingData::static_meta_class,
    CIMPLE_OFF(CIM_ElementSettingData,SettingData)
};

/*[1568]*/
extern const Meta_Property
_CIM_ElementSettingData_IsDefault;

/*[1591]*/
const Meta_Property
_CIM_ElementSettingData_IsDefault =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "IsDefault",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    UINT16,
    0, /* subscript */
    CIMPLE_OFF(CIM_ElementSettingData,IsDefault),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_ElementSettingData_IsCurrent;

/*[1591]*/
const Meta_Property
_CIM_ElementSettingData_IsCurrent =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "IsCurrent",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    UINT16,
    0, /* subscript */
    CIMPLE_OFF(CIM_ElementSettingData,IsCurrent),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_ElementSettingData_IsNext;

/*[1591]*/
const Meta_Property
_CIM_ElementSettingData_IsNext =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "IsNext",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    UINT16,
    0, /* subscript */
    CIMPLE_OFF(CIM_ElementSettingData,IsNext),
    0, /* value */
};

/*[2291]*/
static Meta_Feature* _CIM_ElementSettingData_MFA[] =
{
    (Meta_Feature*)(void*)&_CIM_ElementSettingData_ManagedElement,
    (Meta_Feature*)(void*)&_CIM_ElementSettingData_SettingData,
    (Meta_Feature*)(void*)&_CIM_ElementSettingData_IsDefault,
    (Meta_Feature*)(void*)&_CIM_ElementSettingData_IsCurrent,
    (Meta_Feature*)(void*)&_CIM_ElementSettingData_IsNext,
};

/*[2338]*/
static const Meta_Feature_Local _locals[] =
{
    {1},
    {1},
    {1},
    {1},
    {1},
};

/*[2358]*/
const Meta_Class CIM_ElementSettingData::static_meta_class =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_ASSOCIATION,
    "CIM_ElementSettingData",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    _CIM_ElementSettingData_MFA,
    CIMPLE_ARRAY_SIZE(_CIM_ElementSettingData_MFA),
    sizeof(CIM_ElementSettingData),
    _locals,
    0, /* super_class */ 
    2, /* num_keys */
    &__meta_repository_480038F9449B17AC9E2210119CF38AF2,
};

CIMPLE_NAMESPACE_END

