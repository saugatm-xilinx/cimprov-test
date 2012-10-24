/*NOCHKSRC*/
//==============================================================================
//
// PLEASE DO NOT EDIT; THIS FILE WAS AUTOMATICALLY GENERATED BY GENCLASS 2.0.24
//
//==============================================================================

#include <cimple/Meta_Class.h>
#include <cimple/Meta_Property.h>
#include <cimple/Meta_Reference.h>
#include "CIM_ServiceAffectsElement.h"

CIMPLE_NAMESPACE_BEGIN

using namespace cimple;

extern const Meta_Repository __meta_repository_7D6DA94023C0157F82D3A7310346C853;

/*[1653]*/
extern const Meta_Reference
_CIM_ServiceAffectsElement_AffectedElement;

/*[1664]*/
const Meta_Reference
_CIM_ServiceAffectsElement_AffectedElement =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_REFERENCE|CIMPLE_FLAG_KEY,
    "AffectedElement",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    0, /* subscript */
    &CIM_ManagedElement::static_meta_class,
    CIMPLE_OFF(CIM_ServiceAffectsElement,AffectedElement)
};

/*[1653]*/
extern const Meta_Reference
_CIM_ServiceAffectsElement_AffectingElement;

/*[1664]*/
const Meta_Reference
_CIM_ServiceAffectsElement_AffectingElement =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_REFERENCE|CIMPLE_FLAG_KEY,
    "AffectingElement",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    0, /* subscript */
    &CIM_Service::static_meta_class,
    CIMPLE_OFF(CIM_ServiceAffectsElement,AffectingElement)
};

/*[1568]*/
extern const Meta_Property
_CIM_ServiceAffectsElement_ElementEffects;

/*[1591]*/
const Meta_Property
_CIM_ServiceAffectsElement_ElementEffects =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "ElementEffects",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    UINT16,
    -1, /* subscript */
    CIMPLE_OFF(CIM_ServiceAffectsElement,ElementEffects),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_ServiceAffectsElement_OtherElementEffectsDescriptions;

/*[1591]*/
const Meta_Property
_CIM_ServiceAffectsElement_OtherElementEffectsDescriptions =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "OtherElementEffectsDescriptions",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    STRING,
    -1, /* subscript */
    CIMPLE_OFF(CIM_ServiceAffectsElement,OtherElementEffectsDescriptions),
    0, /* value */
};

/*[2291]*/
static Meta_Feature* _CIM_ServiceAffectsElement_MFA[] =
{
    (Meta_Feature*)(void*)&_CIM_ServiceAffectsElement_AffectedElement,
    (Meta_Feature*)(void*)&_CIM_ServiceAffectsElement_AffectingElement,
    (Meta_Feature*)(void*)&_CIM_ServiceAffectsElement_ElementEffects,
    (Meta_Feature*)(void*)&_CIM_ServiceAffectsElement_OtherElementEffectsDescriptions,
};

/*[2338]*/
static const Meta_Feature_Local _locals[] =
{
    {1},
    {1},
    {1},
    {1},
};

/*[2358]*/
const Meta_Class CIM_ServiceAffectsElement::static_meta_class =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_ASSOCIATION,
    "CIM_ServiceAffectsElement",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    _CIM_ServiceAffectsElement_MFA,
    CIMPLE_ARRAY_SIZE(_CIM_ServiceAffectsElement_MFA),
    sizeof(CIM_ServiceAffectsElement),
    _locals,
    0, /* super_class */ 
    2, /* num_keys */
    &__meta_repository_7D6DA94023C0157F82D3A7310346C853,
};

CIMPLE_NAMESPACE_END

