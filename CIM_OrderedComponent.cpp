/*NOCHKSRC*/
//==============================================================================
//
// PLEASE DO NOT EDIT; THIS FILE WAS AUTOMATICALLY GENERATED BY GENCLASS 2.0.24
//
//==============================================================================

#include <cimple/Meta_Class.h>
#include <cimple/Meta_Property.h>
#include <cimple/Meta_Reference.h>
#include "CIM_OrderedComponent.h"

CIMPLE_NAMESPACE_BEGIN

using namespace cimple;

extern const Meta_Repository __meta_repository_52E780FCF95E157C8B11653275F6420A;

/*[1653]*/
extern const Meta_Reference
_CIM_Component_GroupComponent;

/*[1653]*/
extern const Meta_Reference
_CIM_Component_PartComponent;

/*[1568]*/
extern const Meta_Property
_CIM_OrderedComponent_AssignedSequence;

/*[1591]*/
const Meta_Property
_CIM_OrderedComponent_AssignedSequence =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "AssignedSequence",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    UINT64,
    0, /* subscript */
    CIMPLE_OFF(CIM_OrderedComponent,AssignedSequence),
    0, /* value */
};

/*[2291]*/
static Meta_Feature* _CIM_OrderedComponent_MFA[] =
{
    (Meta_Feature*)(void*)&_CIM_Component_GroupComponent,
    (Meta_Feature*)(void*)&_CIM_Component_PartComponent,
    (Meta_Feature*)(void*)&_CIM_OrderedComponent_AssignedSequence,
};

/*[2338]*/
static const Meta_Feature_Local _locals[] =
{
    {0},
    {0},
    {1},
};

/*[2358]*/
const Meta_Class CIM_OrderedComponent::static_meta_class =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_ASSOCIATION|CIMPLE_FLAG_AGGREGATION,
    "CIM_OrderedComponent",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    _CIM_OrderedComponent_MFA,
    CIMPLE_ARRAY_SIZE(_CIM_OrderedComponent_MFA),
    sizeof(CIM_OrderedComponent),
    _locals,
    &CIM_Component::static_meta_class,
    2, /* num_keys */
    &__meta_repository_52E780FCF95E157C8B11653275F6420A,
};

CIMPLE_NAMESPACE_END

