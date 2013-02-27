/*NOCHKSRC*/
//==============================================================================
//
// PLEASE DO NOT EDIT; THIS FILE WAS AUTOMATICALLY GENERATED BY GENCLASS 2.0.24
//
//==============================================================================

#include <cimple/Meta_Class.h>
#include <cimple/Meta_Property.h>
#include <cimple/Meta_Reference.h>
#include "CIM_ServiceAvailableToElement.h"

CIMPLE_NAMESPACE_BEGIN

using namespace cimple;

extern const Meta_Repository __meta_repository_0DDA66E0FF471BAB95C366A9360C9285;

/*[1653]*/
extern const Meta_Reference
_CIM_ServiceAvailableToElement_ServiceProvided;

/*[1664]*/
const Meta_Reference
_CIM_ServiceAvailableToElement_ServiceProvided =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_REFERENCE|CIMPLE_FLAG_KEY,
    "ServiceProvided",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    0, /* subscript */
    &CIM_Service::static_meta_class,
    CIMPLE_OFF(CIM_ServiceAvailableToElement,ServiceProvided)
};

/*[1653]*/
extern const Meta_Reference
_CIM_ServiceAvailableToElement_UserOfService;

/*[1664]*/
const Meta_Reference
_CIM_ServiceAvailableToElement_UserOfService =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_REFERENCE|CIMPLE_FLAG_KEY,
    "UserOfService",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    0, /* subscript */
    &CIM_ManagedElement::static_meta_class,
    CIMPLE_OFF(CIM_ServiceAvailableToElement,UserOfService)
};

/*[2291]*/
static Meta_Feature* _CIM_ServiceAvailableToElement_MFA[] =
{
    (Meta_Feature*)(void*)&_CIM_ServiceAvailableToElement_ServiceProvided,
    (Meta_Feature*)(void*)&_CIM_ServiceAvailableToElement_UserOfService,
};

/*[2338]*/
static const Meta_Feature_Local _locals[] =
{
    {1},
    {1},
};

/*[2358]*/
const Meta_Class CIM_ServiceAvailableToElement::static_meta_class =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_ASSOCIATION,
    "CIM_ServiceAvailableToElement",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    _CIM_ServiceAvailableToElement_MFA,
    CIMPLE_ARRAY_SIZE(_CIM_ServiceAvailableToElement_MFA),
    sizeof(CIM_ServiceAvailableToElement),
    _locals,
    0, /* super_class */ 
    2, /* num_keys */
    &__meta_repository_0DDA66E0FF471BAB95C366A9360C9285,
};

CIMPLE_NAMESPACE_END

