/*NOCHKSRC*/
//==============================================================================
//
// PLEASE DO NOT EDIT; THIS FILE WAS AUTOMATICALLY GENERATED BY GENCLASS 2.0.24
//
//==============================================================================

#include <cimple/Meta_Class.h>
#include <cimple/Meta_Property.h>
#include <cimple/Meta_Reference.h>
#include "SF_ServiceAffectsElement.h"

CIMPLE_NAMESPACE_BEGIN

using namespace cimple;

extern const Meta_Repository __meta_repository_480038F9449B17AC9E2210119CF38AF2;

/*[1653]*/
extern const Meta_Reference
_CIM_ServiceAffectsElement_AffectedElement;

/*[1653]*/
extern const Meta_Reference
_CIM_ServiceAffectsElement_AffectingElement;

/*[1568]*/
extern const Meta_Property
_CIM_ServiceAffectsElement_ElementEffects;

/*[1568]*/
extern const Meta_Property
_CIM_ServiceAffectsElement_OtherElementEffectsDescriptions;

/*[2291]*/
static Meta_Feature* _SF_ServiceAffectsElement_MFA[] =
{
    (Meta_Feature*)(void*)&_CIM_ServiceAffectsElement_AffectedElement,
    (Meta_Feature*)(void*)&_CIM_ServiceAffectsElement_AffectingElement,
    (Meta_Feature*)(void*)&_CIM_ServiceAffectsElement_ElementEffects,
    (Meta_Feature*)(void*)&_CIM_ServiceAffectsElement_OtherElementEffectsDescriptions,
};

/*[2338]*/
static const Meta_Feature_Local _locals[] =
{
    {0},
    {0},
    {0},
    {0},
};

/*[2358]*/
const Meta_Class SF_ServiceAffectsElement::static_meta_class =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_ASSOCIATION,
    "SF_ServiceAffectsElement",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    _SF_ServiceAffectsElement_MFA,
    CIMPLE_ARRAY_SIZE(_SF_ServiceAffectsElement_MFA),
    sizeof(SF_ServiceAffectsElement),
    _locals,
    &CIM_ServiceAffectsElement::static_meta_class,
    2, /* num_keys */
    &__meta_repository_480038F9449B17AC9E2210119CF38AF2,
};

CIMPLE_NAMESPACE_END

