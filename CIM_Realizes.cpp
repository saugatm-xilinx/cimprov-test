/*NOCHKSRC*/
//==============================================================================
//
// PLEASE DO NOT EDIT; THIS FILE WAS AUTOMATICALLY GENERATED BY GENCLASS 2.0.24
//
//==============================================================================

#include <cimple/Meta_Class.h>
#include <cimple/Meta_Property.h>
#include <cimple/Meta_Reference.h>
#include "CIM_Realizes.h"

CIMPLE_NAMESPACE_BEGIN

using namespace cimple;

extern const Meta_Repository __meta_repository_CCA3FB4C49C51F2D868FF3E04426D671;

/*[1653]*/
extern const Meta_Reference
_CIM_Realizes_Antecedent;

/*[1664]*/
const Meta_Reference
_CIM_Realizes_Antecedent =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_REFERENCE|CIMPLE_FLAG_KEY,
    "Antecedent",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    0, /* subscript */
    &CIM_PhysicalElement::static_meta_class,
    CIMPLE_OFF(CIM_Realizes,Antecedent)
};

/*[1653]*/
extern const Meta_Reference
_CIM_Realizes_Dependent;

/*[1664]*/
const Meta_Reference
_CIM_Realizes_Dependent =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_REFERENCE|CIMPLE_FLAG_KEY,
    "Dependent",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    0, /* subscript */
    &CIM_LogicalDevice::static_meta_class,
    CIMPLE_OFF(CIM_Realizes,Dependent)
};

/*[2291]*/
static Meta_Feature* _CIM_Realizes_MFA[] =
{
    (Meta_Feature*)(void*)&_CIM_Realizes_Antecedent,
    (Meta_Feature*)(void*)&_CIM_Realizes_Dependent,
};

/*[2338]*/
static const Meta_Feature_Local _locals[] =
{
    {1},
    {1},
};

/*[2358]*/
const Meta_Class CIM_Realizes::static_meta_class =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_ASSOCIATION,
    "CIM_Realizes",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    _CIM_Realizes_MFA,
    CIMPLE_ARRAY_SIZE(_CIM_Realizes_MFA),
    sizeof(CIM_Realizes),
    _locals,
    &CIM_Dependency::static_meta_class,
    2, /* num_keys */
    &__meta_repository_CCA3FB4C49C51F2D868FF3E04426D671,
};

CIMPLE_NAMESPACE_END

