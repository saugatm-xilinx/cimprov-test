/*NOCHKSRC*/
//==============================================================================
//
// PLEASE DO NOT EDIT; THIS FILE WAS AUTOMATICALLY GENERATED BY GENCLASS 2.0.24
//
//==============================================================================

#include <cimple/Meta_Class.h>
#include <cimple/Meta_Property.h>
#include <cimple/Meta_Reference.h>
#include "SF_ConnectorOnNIC.h"

CIMPLE_NAMESPACE_BEGIN

using namespace cimple;

extern const Meta_Repository __meta_repository_6CA0219685691FADA8733C94FE01BD97;

/*[1653]*/
extern const Meta_Reference
_CIM_ConnectorOnPackage_GroupComponent;

/*[1653]*/
extern const Meta_Reference
_CIM_ConnectorOnPackage_PartComponent;

/*[1568]*/
extern const Meta_Property
_CIM_Container_LocationWithinContainer;

/*[2291]*/
static Meta_Feature* _SF_ConnectorOnNIC_MFA[] =
{
    (Meta_Feature*)(void*)&_CIM_ConnectorOnPackage_GroupComponent,
    (Meta_Feature*)(void*)&_CIM_ConnectorOnPackage_PartComponent,
    (Meta_Feature*)(void*)&_CIM_Container_LocationWithinContainer,
};

/*[2338]*/
static const Meta_Feature_Local _locals[] =
{
    {0},
    {0},
    {0},
};

/*[2358]*/
const Meta_Class SF_ConnectorOnNIC::static_meta_class =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_ASSOCIATION|CIMPLE_FLAG_AGGREGATION,
    "SF_ConnectorOnNIC",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    _SF_ConnectorOnNIC_MFA,
    CIMPLE_ARRAY_SIZE(_SF_ConnectorOnNIC_MFA),
    sizeof(SF_ConnectorOnNIC),
    _locals,
    &CIM_ConnectorOnPackage::static_meta_class,
    2, /* num_keys */
    &__meta_repository_6CA0219685691FADA8733C94FE01BD97,
};

CIMPLE_NAMESPACE_END

