/*NOCHKSRC*/
//==============================================================================
//
// PLEASE DO NOT EDIT; THIS FILE WAS AUTOMATICALLY GENERATED BY GENCLASS 2.0.24
//
//==============================================================================

#include <cimple/Meta_Class.h>
#include <cimple/Meta_Property.h>
#include <cimple/Meta_Reference.h>
#include "CIM_ConnectorOnPackage.h"

CIMPLE_NAMESPACE_BEGIN

using namespace cimple;

extern const Meta_Repository __meta_repository_0DDA66E0FF471BAB95C366A9360C9285;

/*[1653]*/
extern const Meta_Reference
_CIM_ConnectorOnPackage_GroupComponent;

/*[1664]*/
const Meta_Reference
_CIM_ConnectorOnPackage_GroupComponent =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_REFERENCE|CIMPLE_FLAG_KEY|CIMPLE_FLAG_AGGREGATE,
    "GroupComponent",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    0, /* subscript */
    &CIM_PhysicalPackage::static_meta_class,
    CIMPLE_OFF(CIM_ConnectorOnPackage,GroupComponent)
};

/*[1653]*/
extern const Meta_Reference
_CIM_ConnectorOnPackage_PartComponent;

/*[1664]*/
const Meta_Reference
_CIM_ConnectorOnPackage_PartComponent =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_REFERENCE|CIMPLE_FLAG_KEY,
    "PartComponent",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    0, /* subscript */
    &CIM_PhysicalConnector::static_meta_class,
    CIMPLE_OFF(CIM_ConnectorOnPackage,PartComponent)
};

/*[1568]*/
extern const Meta_Property
_CIM_Container_LocationWithinContainer;

/*[2291]*/
static Meta_Feature* _CIM_ConnectorOnPackage_MFA[] =
{
    (Meta_Feature*)(void*)&_CIM_ConnectorOnPackage_GroupComponent,
    (Meta_Feature*)(void*)&_CIM_ConnectorOnPackage_PartComponent,
    (Meta_Feature*)(void*)&_CIM_Container_LocationWithinContainer,
};

/*[2338]*/
static const Meta_Feature_Local _locals[] =
{
    {1},
    {1},
    {0},
};

/*[2358]*/
const Meta_Class CIM_ConnectorOnPackage::static_meta_class =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_ASSOCIATION|CIMPLE_FLAG_AGGREGATION,
    "CIM_ConnectorOnPackage",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    _CIM_ConnectorOnPackage_MFA,
    CIMPLE_ARRAY_SIZE(_CIM_ConnectorOnPackage_MFA),
    sizeof(CIM_ConnectorOnPackage),
    _locals,
    &CIM_Container::static_meta_class,
    2, /* num_keys */
    &__meta_repository_0DDA66E0FF471BAB95C366A9360C9285,
};

CIMPLE_NAMESPACE_END

