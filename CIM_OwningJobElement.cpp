/*NOCHKSRC*/
//==============================================================================
//
// PLEASE DO NOT EDIT; THIS FILE WAS AUTOMATICALLY GENERATED BY GENCLASS 2.0.24
//
//==============================================================================

#include <cimple/Meta_Class.h>
#include <cimple/Meta_Property.h>
#include <cimple/Meta_Reference.h>
#include "CIM_OwningJobElement.h"

CIMPLE_NAMESPACE_BEGIN

using namespace cimple;

extern const Meta_Repository __meta_repository_0DDA66E0FF471BAB95C366A9360C9285;

/*[1653]*/
extern const Meta_Reference
_CIM_OwningJobElement_OwningElement;

/*[1664]*/
const Meta_Reference
_CIM_OwningJobElement_OwningElement =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_REFERENCE|CIMPLE_FLAG_KEY,
    "OwningElement",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    0, /* subscript */
    &CIM_ManagedElement::static_meta_class,
    CIMPLE_OFF(CIM_OwningJobElement,OwningElement)
};

/*[1653]*/
extern const Meta_Reference
_CIM_OwningJobElement_OwnedElement;

/*[1664]*/
const Meta_Reference
_CIM_OwningJobElement_OwnedElement =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_REFERENCE|CIMPLE_FLAG_KEY,
    "OwnedElement",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    0, /* subscript */
    &CIM_Job::static_meta_class,
    CIMPLE_OFF(CIM_OwningJobElement,OwnedElement)
};

/*[2291]*/
static Meta_Feature* _CIM_OwningJobElement_MFA[] =
{
    (Meta_Feature*)(void*)&_CIM_OwningJobElement_OwningElement,
    (Meta_Feature*)(void*)&_CIM_OwningJobElement_OwnedElement,
};

/*[2338]*/
static const Meta_Feature_Local _locals[] =
{
    {1},
    {1},
};

/*[2358]*/
const Meta_Class CIM_OwningJobElement::static_meta_class =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_ASSOCIATION,
    "CIM_OwningJobElement",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    _CIM_OwningJobElement_MFA,
    CIMPLE_ARRAY_SIZE(_CIM_OwningJobElement_MFA),
    sizeof(CIM_OwningJobElement),
    _locals,
    0, /* super_class */ 
    2, /* num_keys */
    &__meta_repository_0DDA66E0FF471BAB95C366A9360C9285,
};

CIMPLE_NAMESPACE_END

