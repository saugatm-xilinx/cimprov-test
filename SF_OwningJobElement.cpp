/*NOCHKSRC*/
//==============================================================================
//
// PLEASE DO NOT EDIT; THIS FILE WAS AUTOMATICALLY GENERATED BY GENCLASS 2.0.24
//
//==============================================================================

#include <cimple/Meta_Class.h>
#include <cimple/Meta_Property.h>
#include <cimple/Meta_Reference.h>
#include "SF_OwningJobElement.h"

CIMPLE_NAMESPACE_BEGIN

using namespace cimple;

extern const Meta_Repository __meta_repository_0DDA66E0FF471BAB95C366A9360C9285;

/*[1653]*/
extern const Meta_Reference
_CIM_OwningJobElement_OwningElement;

/*[1653]*/
extern const Meta_Reference
_CIM_OwningJobElement_OwnedElement;

/*[2291]*/
static Meta_Feature* _SF_OwningJobElement_MFA[] =
{
    (Meta_Feature*)(void*)&_CIM_OwningJobElement_OwningElement,
    (Meta_Feature*)(void*)&_CIM_OwningJobElement_OwnedElement,
};

/*[2338]*/
static const Meta_Feature_Local _locals[] =
{
    {0},
    {0},
};

/*[2358]*/
const Meta_Class SF_OwningJobElement::static_meta_class =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_ASSOCIATION,
    "SF_OwningJobElement",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    _SF_OwningJobElement_MFA,
    CIMPLE_ARRAY_SIZE(_SF_OwningJobElement_MFA),
    sizeof(SF_OwningJobElement),
    _locals,
    &CIM_OwningJobElement::static_meta_class,
    2, /* num_keys */
    &__meta_repository_0DDA66E0FF471BAB95C366A9360C9285,
};

CIMPLE_NAMESPACE_END

