/*NOCHKSRC*/
//==============================================================================
//
// PLEASE DO NOT EDIT; THIS FILE WAS AUTOMATICALLY GENERATED BY GENCLASS 2.0.24
//
//==============================================================================

#include <cimple/Meta_Class.h>
#include <cimple/Meta_Property.h>
#include <cimple/Meta_Reference.h>
#include "CIM_HostedService.h"

CIMPLE_NAMESPACE_BEGIN

using namespace cimple;

extern const Meta_Repository __meta_repository_480038F9449B17AC9E2210119CF38AF2;

/*[1653]*/
extern const Meta_Reference
_CIM_HostedService_Antecedent;

/*[1664]*/
const Meta_Reference
_CIM_HostedService_Antecedent =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_REFERENCE|CIMPLE_FLAG_KEY,
    "Antecedent",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    0, /* subscript */
    &CIM_System::static_meta_class,
    CIMPLE_OFF(CIM_HostedService,Antecedent)
};

/*[1653]*/
extern const Meta_Reference
_CIM_HostedService_Dependent;

/*[1664]*/
const Meta_Reference
_CIM_HostedService_Dependent =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_REFERENCE|CIMPLE_FLAG_KEY|CIMPLE_FLAG_WEAK,
    "Dependent",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    0, /* subscript */
    &CIM_Service::static_meta_class,
    CIMPLE_OFF(CIM_HostedService,Dependent)
};

/*[2291]*/
static Meta_Feature* _CIM_HostedService_MFA[] =
{
    (Meta_Feature*)(void*)&_CIM_HostedService_Antecedent,
    (Meta_Feature*)(void*)&_CIM_HostedService_Dependent,
};

/*[2338]*/
static const Meta_Feature_Local _locals[] =
{
    {1},
    {1},
};

/*[2358]*/
const Meta_Class CIM_HostedService::static_meta_class =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_ASSOCIATION,
    "CIM_HostedService",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    _CIM_HostedService_MFA,
    CIMPLE_ARRAY_SIZE(_CIM_HostedService_MFA),
    sizeof(CIM_HostedService),
    _locals,
    &CIM_HostedDependency::static_meta_class,
    2, /* num_keys */
    &__meta_repository_480038F9449B17AC9E2210119CF38AF2,
};

CIMPLE_NAMESPACE_END

