/*NOCHKSRC*/
//==============================================================================
//
// PLEASE DO NOT EDIT; THIS FILE WAS AUTOMATICALLY GENERATED BY GENCLASS 2.0.24
//
//==============================================================================

#include <cimple/Meta_Class.h>
#include <cimple/Meta_Property.h>
#include <cimple/Meta_Reference.h>
#include "CIM_Component.h"

CIMPLE_NAMESPACE_BEGIN

using namespace cimple;

extern const Meta_Repository __meta_repository_25890684DE9F1ED99AF8E12287919123;

/*[1653]*/
extern const Meta_Reference
_CIM_Component_GroupComponent;

/*[1664]*/
const Meta_Reference
_CIM_Component_GroupComponent =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_REFERENCE|CIMPLE_FLAG_KEY|CIMPLE_FLAG_AGGREGATE,
    "GroupComponent",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    0, /* subscript */
    &CIM_ManagedElement::static_meta_class,
    CIMPLE_OFF(CIM_Component,GroupComponent)
};

/*[1653]*/
extern const Meta_Reference
_CIM_Component_PartComponent;

/*[1664]*/
const Meta_Reference
_CIM_Component_PartComponent =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_REFERENCE|CIMPLE_FLAG_KEY,
    "PartComponent",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    0, /* subscript */
    &CIM_ManagedElement::static_meta_class,
    CIMPLE_OFF(CIM_Component,PartComponent)
};

/*[2291]*/
static Meta_Feature* _CIM_Component_MFA[] =
{
    (Meta_Feature*)(void*)&_CIM_Component_GroupComponent,
    (Meta_Feature*)(void*)&_CIM_Component_PartComponent,
};

/*[2338]*/
static const Meta_Feature_Local _locals[] =
{
    {1},
    {1},
};

/*[2358]*/
const Meta_Class CIM_Component::static_meta_class =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_ASSOCIATION|CIMPLE_FLAG_ABSTRACT|CIMPLE_FLAG_AGGREGATION,
    "CIM_Component",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    _CIM_Component_MFA,
    CIMPLE_ARRAY_SIZE(_CIM_Component_MFA),
    sizeof(CIM_Component),
    _locals,
    0, /* super_class */ 
    2, /* num_keys */
    &__meta_repository_25890684DE9F1ED99AF8E12287919123,
};

CIMPLE_NAMESPACE_END

