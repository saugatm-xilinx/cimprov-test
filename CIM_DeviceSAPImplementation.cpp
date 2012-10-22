/*NOCHKSRC*/
//==============================================================================
//
// PLEASE DO NOT EDIT; THIS FILE WAS AUTOMATICALLY GENERATED BY GENCLASS 2.0.24
//
//==============================================================================

#include <cimple/Meta_Class.h>
#include <cimple/Meta_Property.h>
#include <cimple/Meta_Reference.h>
#include "CIM_DeviceSAPImplementation.h"

CIMPLE_NAMESPACE_BEGIN

using namespace cimple;

extern const Meta_Repository __meta_repository_5122E1D89D3817F3AE783FBE02157763;

/*[1653]*/
extern const Meta_Reference
_CIM_DeviceSAPImplementation_Antecedent;

/*[1664]*/
const Meta_Reference
_CIM_DeviceSAPImplementation_Antecedent =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_REFERENCE|CIMPLE_FLAG_KEY,
    "Antecedent",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    0, /* subscript */
    &CIM_LogicalDevice::static_meta_class,
    CIMPLE_OFF(CIM_DeviceSAPImplementation,Antecedent)
};

/*[1653]*/
extern const Meta_Reference
_CIM_DeviceSAPImplementation_Dependent;

/*[1664]*/
const Meta_Reference
_CIM_DeviceSAPImplementation_Dependent =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_REFERENCE|CIMPLE_FLAG_KEY,
    "Dependent",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    0, /* subscript */
    &CIM_ServiceAccessPoint::static_meta_class,
    CIMPLE_OFF(CIM_DeviceSAPImplementation,Dependent)
};

/*[2291]*/
static Meta_Feature* _CIM_DeviceSAPImplementation_MFA[] =
{
    (Meta_Feature*)(void*)&_CIM_DeviceSAPImplementation_Antecedent,
    (Meta_Feature*)(void*)&_CIM_DeviceSAPImplementation_Dependent,
};

/*[2338]*/
static const Meta_Feature_Local _locals[] =
{
    {1},
    {1},
};

/*[2358]*/
const Meta_Class CIM_DeviceSAPImplementation::static_meta_class =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_ASSOCIATION,
    "CIM_DeviceSAPImplementation",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    _CIM_DeviceSAPImplementation_MFA,
    CIMPLE_ARRAY_SIZE(_CIM_DeviceSAPImplementation_MFA),
    sizeof(CIM_DeviceSAPImplementation),
    _locals,
    &CIM_Dependency::static_meta_class,
    2, /* num_keys */
    &__meta_repository_5122E1D89D3817F3AE783FBE02157763,
};

CIMPLE_NAMESPACE_END

