/*NOCHKSRC*/
//==============================================================================
//
// PLEASE DO NOT EDIT; THIS FILE WAS AUTOMATICALLY GENERATED BY GENCLASS 2.0.24
//
//==============================================================================

#include <cimple/Meta_Class.h>
#include <cimple/Meta_Property.h>
#include <cimple/Meta_Reference.h>
#include "CIM_Capabilities.h"

CIMPLE_NAMESPACE_BEGIN

using namespace cimple;

extern const Meta_Repository __meta_repository_322BD996DC5A15A5B9D5D42DDD2F5A5E;

/*[1568]*/
extern const Meta_Property
_CIM_Capabilities_InstanceID;

/*[1591]*/
const Meta_Property
_CIM_Capabilities_InstanceID =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_KEY|CIMPLE_FLAG_READ,
    "InstanceID",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    STRING,
    0, /* subscript */
    CIMPLE_OFF(CIM_Capabilities,InstanceID),
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
_CIM_Capabilities_ElementName;

/*[1591]*/
const Meta_Property
_CIM_Capabilities_ElementName =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ|CIMPLE_FLAG_REQUIRED,
    "ElementName",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    STRING,
    0, /* subscript */
    CIMPLE_OFF(CIM_Capabilities,ElementName),
    0, /* value */
};

/*[2291]*/
static Meta_Feature* _CIM_Capabilities_MFA[] =
{
    (Meta_Feature*)(void*)&_CIM_Capabilities_InstanceID,
    (Meta_Feature*)(void*)&_CIM_ManagedElement_Caption,
    (Meta_Feature*)(void*)&_CIM_ManagedElement_Description,
    (Meta_Feature*)(void*)&_CIM_Capabilities_ElementName,
};

/*[2338]*/
static const Meta_Feature_Local _locals[] =
{
    {1},
    {0},
    {0},
    {1},
};

/*[2358]*/
const Meta_Class CIM_Capabilities::static_meta_class =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_CLASS|CIMPLE_FLAG_ABSTRACT,
    "CIM_Capabilities",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    _CIM_Capabilities_MFA,
    CIMPLE_ARRAY_SIZE(_CIM_Capabilities_MFA),
    sizeof(CIM_Capabilities),
    _locals,
    &CIM_ManagedElement::static_meta_class,
    1, /* num_keys */
    &__meta_repository_322BD996DC5A15A5B9D5D42DDD2F5A5E,
};

CIMPLE_NAMESPACE_END

