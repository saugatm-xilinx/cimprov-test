/*NOCHKSRC*/
//==============================================================================
//
// PLEASE DO NOT EDIT; THIS FILE WAS AUTOMATICALLY GENERATED BY GENCLASS 2.0.24
//
//==============================================================================

#include <cimple/Meta_Class.h>
#include <cimple/Meta_Property.h>
#include <cimple/Meta_Reference.h>
#include "SF_ElementSoftwareIdentity.h"

CIMPLE_NAMESPACE_BEGIN

using namespace cimple;

extern const Meta_Repository __meta_repository_322BD996DC5A15A5B9D5D42DDD2F5A5E;

/*[1653]*/
extern const Meta_Reference
_CIM_ElementSoftwareIdentity_Antecedent;

/*[1653]*/
extern const Meta_Reference
_CIM_ElementSoftwareIdentity_Dependent;

/*[1568]*/
extern const Meta_Property
_CIM_ElementSoftwareIdentity_UpgradeCondition;

/*[1568]*/
extern const Meta_Property
_CIM_ElementSoftwareIdentity_OtherUpgradeCondition;

/*[1568]*/
extern const Meta_Property
_CIM_ElementSoftwareIdentity_ElementSoftwareStatus;

/*[2291]*/
static Meta_Feature* _SF_ElementSoftwareIdentity_MFA[] =
{
    (Meta_Feature*)(void*)&_CIM_ElementSoftwareIdentity_Antecedent,
    (Meta_Feature*)(void*)&_CIM_ElementSoftwareIdentity_Dependent,
    (Meta_Feature*)(void*)&_CIM_ElementSoftwareIdentity_UpgradeCondition,
    (Meta_Feature*)(void*)&_CIM_ElementSoftwareIdentity_OtherUpgradeCondition,
    (Meta_Feature*)(void*)&_CIM_ElementSoftwareIdentity_ElementSoftwareStatus,
};

/*[2338]*/
static const Meta_Feature_Local _locals[] =
{
    {0},
    {0},
    {0},
    {0},
    {0},
};

/*[2358]*/
const Meta_Class SF_ElementSoftwareIdentity::static_meta_class =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_ASSOCIATION,
    "SF_ElementSoftwareIdentity",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    _SF_ElementSoftwareIdentity_MFA,
    CIMPLE_ARRAY_SIZE(_SF_ElementSoftwareIdentity_MFA),
    sizeof(SF_ElementSoftwareIdentity),
    _locals,
    &CIM_ElementSoftwareIdentity::static_meta_class,
    2, /* num_keys */
    &__meta_repository_322BD996DC5A15A5B9D5D42DDD2F5A5E,
};

CIMPLE_NAMESPACE_END

