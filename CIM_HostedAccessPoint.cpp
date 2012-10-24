/*NOCHKSRC*/
//==============================================================================
//
// PLEASE DO NOT EDIT; THIS FILE WAS AUTOMATICALLY GENERATED BY GENCLASS 2.0.24
//
//==============================================================================

#include <cimple/Meta_Class.h>
#include <cimple/Meta_Property.h>
#include <cimple/Meta_Reference.h>
#include "CIM_HostedAccessPoint.h"

CIMPLE_NAMESPACE_BEGIN

using namespace cimple;

extern const Meta_Repository __meta_repository_7D6DA94023C0157F82D3A7310346C853;

/*[1653]*/
extern const Meta_Reference
_CIM_HostedAccessPoint_Antecedent;

/*[1664]*/
const Meta_Reference
_CIM_HostedAccessPoint_Antecedent =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_REFERENCE|CIMPLE_FLAG_KEY,
    "Antecedent",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    0, /* subscript */
    &CIM_System::static_meta_class,
    CIMPLE_OFF(CIM_HostedAccessPoint,Antecedent)
};

/*[1653]*/
extern const Meta_Reference
_CIM_HostedAccessPoint_Dependent;

/*[1664]*/
const Meta_Reference
_CIM_HostedAccessPoint_Dependent =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_REFERENCE|CIMPLE_FLAG_KEY|CIMPLE_FLAG_WEAK,
    "Dependent",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    0, /* subscript */
    &CIM_ServiceAccessPoint::static_meta_class,
    CIMPLE_OFF(CIM_HostedAccessPoint,Dependent)
};

/*[2291]*/
static Meta_Feature* _CIM_HostedAccessPoint_MFA[] =
{
    (Meta_Feature*)(void*)&_CIM_HostedAccessPoint_Antecedent,
    (Meta_Feature*)(void*)&_CIM_HostedAccessPoint_Dependent,
};

/*[2338]*/
static const Meta_Feature_Local _locals[] =
{
    {1},
    {1},
};

/*[2358]*/
const Meta_Class CIM_HostedAccessPoint::static_meta_class =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_ASSOCIATION,
    "CIM_HostedAccessPoint",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    _CIM_HostedAccessPoint_MFA,
    CIMPLE_ARRAY_SIZE(_CIM_HostedAccessPoint_MFA),
    sizeof(CIM_HostedAccessPoint),
    _locals,
    &CIM_HostedDependency::static_meta_class,
    2, /* num_keys */
    &__meta_repository_7D6DA94023C0157F82D3A7310346C853,
};

CIMPLE_NAMESPACE_END

