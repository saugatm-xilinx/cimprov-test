/*NOCHKSRC*/
//==============================================================================
//
// PLEASE DO NOT EDIT; THIS FILE WAS AUTOMATICALLY GENERATED BY GENCLASS 2.0.24
//
//==============================================================================

#include <cimple/Meta_Class.h>
#include <cimple/Meta_Property.h>
#include <cimple/Meta_Reference.h>
#include "SF_AvailableDiagnosticService.h"

CIMPLE_NAMESPACE_BEGIN

using namespace cimple;

extern const Meta_Repository __meta_repository_52E780FCF95E157C8B11653275F6420A;

/*[1653]*/
extern const Meta_Reference
_CIM_AvailableDiagnosticService_ServiceProvided;

/*[1653]*/
extern const Meta_Reference
_CIM_ServiceAvailableToElement_UserOfService;

/*[1568]*/
extern const Meta_Property
_CIM_AvailableDiagnosticService_EstimatedDurationOfService;

/*[1568]*/
extern const Meta_Property
_CIM_AvailableDiagnosticService_EstimatedDurationQualifier;

/*[2291]*/
static Meta_Feature* _SF_AvailableDiagnosticService_MFA[] =
{
    (Meta_Feature*)(void*)&_CIM_AvailableDiagnosticService_ServiceProvided,
    (Meta_Feature*)(void*)&_CIM_ServiceAvailableToElement_UserOfService,
    (Meta_Feature*)(void*)&_CIM_AvailableDiagnosticService_EstimatedDurationOfService,
    (Meta_Feature*)(void*)&_CIM_AvailableDiagnosticService_EstimatedDurationQualifier,
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
const Meta_Class SF_AvailableDiagnosticService::static_meta_class =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_ASSOCIATION,
    "SF_AvailableDiagnosticService",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    _SF_AvailableDiagnosticService_MFA,
    CIMPLE_ARRAY_SIZE(_SF_AvailableDiagnosticService_MFA),
    sizeof(SF_AvailableDiagnosticService),
    _locals,
    &CIM_AvailableDiagnosticService::static_meta_class,
    2, /* num_keys */
    &__meta_repository_52E780FCF95E157C8B11653275F6420A,
};

CIMPLE_NAMESPACE_END
