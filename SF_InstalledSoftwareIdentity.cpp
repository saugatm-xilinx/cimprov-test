/*NOCHKSRC*/
//==============================================================================
//
// PLEASE DO NOT EDIT; THIS FILE WAS AUTOMATICALLY GENERATED BY GENCLASS 2.0.24
//
//==============================================================================

#include <cimple/Meta_Class.h>
#include <cimple/Meta_Property.h>
#include <cimple/Meta_Reference.h>
#include "SF_InstalledSoftwareIdentity.h"

CIMPLE_NAMESPACE_BEGIN

using namespace cimple;

extern const Meta_Repository __meta_repository_5122E1D89D3817F3AE783FBE02157763;

/*[1653]*/
extern const Meta_Reference
_CIM_InstalledSoftwareIdentity_System;

/*[1653]*/
extern const Meta_Reference
_CIM_InstalledSoftwareIdentity_InstalledSoftware;

/*[2291]*/
static Meta_Feature* _SF_InstalledSoftwareIdentity_MFA[] =
{
    (Meta_Feature*)(void*)&_CIM_InstalledSoftwareIdentity_System,
    (Meta_Feature*)(void*)&_CIM_InstalledSoftwareIdentity_InstalledSoftware,
};

/*[2338]*/
static const Meta_Feature_Local _locals[] =
{
    {0},
    {0},
};

/*[2358]*/
const Meta_Class SF_InstalledSoftwareIdentity::static_meta_class =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_ASSOCIATION,
    "SF_InstalledSoftwareIdentity",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    _SF_InstalledSoftwareIdentity_MFA,
    CIMPLE_ARRAY_SIZE(_SF_InstalledSoftwareIdentity_MFA),
    sizeof(SF_InstalledSoftwareIdentity),
    _locals,
    &CIM_InstalledSoftwareIdentity::static_meta_class,
    2, /* num_keys */
    &__meta_repository_5122E1D89D3817F3AE783FBE02157763,
};

CIMPLE_NAMESPACE_END

