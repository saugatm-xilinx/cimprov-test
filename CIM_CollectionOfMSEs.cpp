/*NOCHKSRC*/
//==============================================================================
//
// PLEASE DO NOT EDIT; THIS FILE WAS AUTOMATICALLY GENERATED BY GENCLASS 2.0.24
//
//==============================================================================

#include <cimple/Meta_Class.h>
#include <cimple/Meta_Property.h>
#include <cimple/Meta_Reference.h>
#include "CIM_CollectionOfMSEs.h"

CIMPLE_NAMESPACE_BEGIN

using namespace cimple;

extern const Meta_Repository __meta_repository_480038F9449B17AC9E2210119CF38AF2;

/*[1568]*/
extern const Meta_Property
_CIM_ManagedElement_InstanceID;

/*[1568]*/
extern const Meta_Property
_CIM_ManagedElement_Caption;

/*[1568]*/
extern const Meta_Property
_CIM_ManagedElement_Description;

/*[1568]*/
extern const Meta_Property
_CIM_ManagedElement_ElementName;

/*[1568]*/
extern const Meta_Property
_CIM_CollectionOfMSEs_CollectionID;

/*[1591]*/
const Meta_Property
_CIM_CollectionOfMSEs_CollectionID =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "CollectionID",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    STRING,
    0, /* subscript */
    CIMPLE_OFF(CIM_CollectionOfMSEs,CollectionID),
    0, /* value */
};

/*[2291]*/
static Meta_Feature* _CIM_CollectionOfMSEs_MFA[] =
{
    (Meta_Feature*)(void*)&_CIM_ManagedElement_InstanceID,
    (Meta_Feature*)(void*)&_CIM_ManagedElement_Caption,
    (Meta_Feature*)(void*)&_CIM_ManagedElement_Description,
    (Meta_Feature*)(void*)&_CIM_ManagedElement_ElementName,
    (Meta_Feature*)(void*)&_CIM_CollectionOfMSEs_CollectionID,
};

/*[2338]*/
static const Meta_Feature_Local _locals[] =
{
    {0},
    {0},
    {0},
    {0},
    {1},
};

/*[2358]*/
const Meta_Class CIM_CollectionOfMSEs::static_meta_class =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_CLASS|CIMPLE_FLAG_ABSTRACT,
    "CIM_CollectionOfMSEs",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    _CIM_CollectionOfMSEs_MFA,
    CIMPLE_ARRAY_SIZE(_CIM_CollectionOfMSEs_MFA),
    sizeof(CIM_CollectionOfMSEs),
    _locals,
    &CIM_Collection::static_meta_class,
    0, /* num_keys */
    &__meta_repository_480038F9449B17AC9E2210119CF38AF2,
};

CIMPLE_NAMESPACE_END

