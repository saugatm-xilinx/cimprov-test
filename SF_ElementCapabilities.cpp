/*NOCHKSRC*/
//==============================================================================
//
// PLEASE DO NOT EDIT; THIS FILE WAS AUTOMATICALLY GENERATED BY GENCLASS 2.0.24
//
//==============================================================================

#include <cimple/Meta_Class.h>
#include <cimple/Meta_Property.h>
#include <cimple/Meta_Reference.h>
#include "SF_ElementCapabilities.h"

CIMPLE_NAMESPACE_BEGIN

using namespace cimple;

extern const Meta_Repository __meta_repository_52E780FCF95E157C8B11653275F6420A;

/*[1653]*/
extern const Meta_Reference
_CIM_ElementCapabilities_ManagedElement;

/*[1653]*/
extern const Meta_Reference
_CIM_ElementCapabilities_Capabilities;

/*[1568]*/
extern const Meta_Property
_CIM_ElementCapabilities_Characteristics;

/*[2291]*/
static Meta_Feature* _SF_ElementCapabilities_MFA[] =
{
    (Meta_Feature*)(void*)&_CIM_ElementCapabilities_ManagedElement,
    (Meta_Feature*)(void*)&_CIM_ElementCapabilities_Capabilities,
    (Meta_Feature*)(void*)&_CIM_ElementCapabilities_Characteristics,
};

/*[2338]*/
static const Meta_Feature_Local _locals[] =
{
    {0},
    {0},
    {0},
};

/*[2358]*/
const Meta_Class SF_ElementCapabilities::static_meta_class =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_ASSOCIATION,
    "SF_ElementCapabilities",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    _SF_ElementCapabilities_MFA,
    CIMPLE_ARRAY_SIZE(_SF_ElementCapabilities_MFA),
    sizeof(SF_ElementCapabilities),
    _locals,
    &CIM_ElementCapabilities::static_meta_class,
    2, /* num_keys */
    &__meta_repository_52E780FCF95E157C8B11653275F6420A,
};

CIMPLE_NAMESPACE_END
