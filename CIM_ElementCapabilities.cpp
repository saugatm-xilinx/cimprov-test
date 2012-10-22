/*NOCHKSRC*/
//==============================================================================
//
// PLEASE DO NOT EDIT; THIS FILE WAS AUTOMATICALLY GENERATED BY GENCLASS 2.0.24
//
//==============================================================================

#include <cimple/Meta_Class.h>
#include <cimple/Meta_Property.h>
#include <cimple/Meta_Reference.h>
#include "CIM_ElementCapabilities.h"

CIMPLE_NAMESPACE_BEGIN

using namespace cimple;

extern const Meta_Repository __meta_repository_322BD996DC5A15A5B9D5D42DDD2F5A5E;

/*[1653]*/
extern const Meta_Reference
_CIM_ElementCapabilities_ManagedElement;

/*[1664]*/
const Meta_Reference
_CIM_ElementCapabilities_ManagedElement =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_REFERENCE|CIMPLE_FLAG_KEY,
    "ManagedElement",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    0, /* subscript */
    &CIM_ManagedElement::static_meta_class,
    CIMPLE_OFF(CIM_ElementCapabilities,ManagedElement)
};

/*[1653]*/
extern const Meta_Reference
_CIM_ElementCapabilities_Capabilities;

/*[1664]*/
const Meta_Reference
_CIM_ElementCapabilities_Capabilities =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_REFERENCE|CIMPLE_FLAG_KEY,
    "Capabilities",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    0, /* subscript */
    &CIM_Capabilities::static_meta_class,
    CIMPLE_OFF(CIM_ElementCapabilities,Capabilities)
};

/*[1568]*/
extern const Meta_Property
_CIM_ElementCapabilities_Characteristics;

/*[1591]*/
const Meta_Property
_CIM_ElementCapabilities_Characteristics =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "Characteristics",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    UINT16,
    -1, /* subscript */
    CIMPLE_OFF(CIM_ElementCapabilities,Characteristics),
    0, /* value */
};

/*[2291]*/
static Meta_Feature* _CIM_ElementCapabilities_MFA[] =
{
    (Meta_Feature*)(void*)&_CIM_ElementCapabilities_ManagedElement,
    (Meta_Feature*)(void*)&_CIM_ElementCapabilities_Capabilities,
    (Meta_Feature*)(void*)&_CIM_ElementCapabilities_Characteristics,
};

/*[2338]*/
static const Meta_Feature_Local _locals[] =
{
    {1},
    {1},
    {1},
};

/*[2358]*/
const Meta_Class CIM_ElementCapabilities::static_meta_class =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_ASSOCIATION,
    "CIM_ElementCapabilities",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    _CIM_ElementCapabilities_MFA,
    CIMPLE_ARRAY_SIZE(_CIM_ElementCapabilities_MFA),
    sizeof(CIM_ElementCapabilities),
    _locals,
    0, /* super_class */ 
    2, /* num_keys */
    &__meta_repository_322BD996DC5A15A5B9D5D42DDD2F5A5E,
};

CIMPLE_NAMESPACE_END

