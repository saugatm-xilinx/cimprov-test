/*NOCHKSRC*/
//==============================================================================
//
// PLEASE DO NOT EDIT; THIS FILE WAS AUTOMATICALLY GENERATED BY GENCLASS 2.0.24
//
//==============================================================================

#include <cimple/Meta_Class.h>
#include <cimple/Meta_Property.h>
#include <cimple/Meta_Reference.h>
#include "CIM_InstalledSoftwareIdentity.h"

CIMPLE_NAMESPACE_BEGIN

using namespace cimple;

extern const Meta_Repository __meta_repository_322BD996DC5A15A5B9D5D42DDD2F5A5E;

/*[1653]*/
extern const Meta_Reference
_CIM_InstalledSoftwareIdentity_System;

/*[1664]*/
const Meta_Reference
_CIM_InstalledSoftwareIdentity_System =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_REFERENCE|CIMPLE_FLAG_KEY,
    "System",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    0, /* subscript */
    &CIM_System::static_meta_class,
    CIMPLE_OFF(CIM_InstalledSoftwareIdentity,System)
};

/*[1653]*/
extern const Meta_Reference
_CIM_InstalledSoftwareIdentity_InstalledSoftware;

/*[1664]*/
const Meta_Reference
_CIM_InstalledSoftwareIdentity_InstalledSoftware =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_REFERENCE|CIMPLE_FLAG_KEY,
    "InstalledSoftware",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    0, /* subscript */
    &CIM_SoftwareIdentity::static_meta_class,
    CIMPLE_OFF(CIM_InstalledSoftwareIdentity,InstalledSoftware)
};

/*[2291]*/
static Meta_Feature* _CIM_InstalledSoftwareIdentity_MFA[] =
{
    (Meta_Feature*)(void*)&_CIM_InstalledSoftwareIdentity_System,
    (Meta_Feature*)(void*)&_CIM_InstalledSoftwareIdentity_InstalledSoftware,
};

/*[2338]*/
static const Meta_Feature_Local _locals[] =
{
    {1},
    {1},
};

/*[2358]*/
const Meta_Class CIM_InstalledSoftwareIdentity::static_meta_class =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_ASSOCIATION,
    "CIM_InstalledSoftwareIdentity",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    _CIM_InstalledSoftwareIdentity_MFA,
    CIMPLE_ARRAY_SIZE(_CIM_InstalledSoftwareIdentity_MFA),
    sizeof(CIM_InstalledSoftwareIdentity),
    _locals,
    0, /* super_class */ 
    2, /* num_keys */
    &__meta_repository_322BD996DC5A15A5B9D5D42DDD2F5A5E,
};

CIMPLE_NAMESPACE_END

