/*NOCHKSRC*/
//==============================================================================
//
// PLEASE DO NOT EDIT; THIS FILE WAS AUTOMATICALLY GENERATED BY GENCLASS 2.0.24
//
//==============================================================================

#include <cimple/Meta_Class.h>
#include <cimple/Meta_Property.h>
#include <cimple/Meta_Reference.h>
#include "CIM_EnabledLogicalElementCapabilities.h"

CIMPLE_NAMESPACE_BEGIN

using namespace cimple;

extern const Meta_Repository __meta_repository_0DDA66E0FF471BAB95C366A9360C9285;

/*[1568]*/
extern const Meta_Property
_CIM_Capabilities_InstanceID;

/*[1568]*/
extern const Meta_Property
_CIM_ManagedElement_Caption;

/*[1568]*/
extern const Meta_Property
_CIM_ManagedElement_Description;

/*[1568]*/
extern const Meta_Property
_CIM_Capabilities_ElementName;

/*[1568]*/
extern const Meta_Property
_CIM_ManagedElement_Generation;

/*[1568]*/
extern const Meta_Property
_CIM_EnabledLogicalElementCapabilities_ElementNameEditSupported;

/*[1591]*/
const Meta_Property
_CIM_EnabledLogicalElementCapabilities_ElementNameEditSupported =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "ElementNameEditSupported",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    BOOLEAN,
    0, /* subscript */
    CIMPLE_OFF(CIM_EnabledLogicalElementCapabilities,ElementNameEditSupported),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_EnabledLogicalElementCapabilities_MaxElementNameLen;

/*[1591]*/
const Meta_Property
_CIM_EnabledLogicalElementCapabilities_MaxElementNameLen =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "MaxElementNameLen",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    UINT16,
    0, /* subscript */
    CIMPLE_OFF(CIM_EnabledLogicalElementCapabilities,MaxElementNameLen),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_EnabledLogicalElementCapabilities_RequestedStatesSupported;

/*[1591]*/
const Meta_Property
_CIM_EnabledLogicalElementCapabilities_RequestedStatesSupported =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "RequestedStatesSupported",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    UINT16,
    -1, /* subscript */
    CIMPLE_OFF(CIM_EnabledLogicalElementCapabilities,RequestedStatesSupported),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_EnabledLogicalElementCapabilities_ElementNameMask;

/*[1591]*/
const Meta_Property
_CIM_EnabledLogicalElementCapabilities_ElementNameMask =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "ElementNameMask",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    STRING,
    0, /* subscript */
    CIMPLE_OFF(CIM_EnabledLogicalElementCapabilities,ElementNameMask),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_EnabledLogicalElementCapabilities_StateAwareness;

/*[1591]*/
const Meta_Property
_CIM_EnabledLogicalElementCapabilities_StateAwareness =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_EXPERIMENTAL|CIMPLE_FLAG_READ,
    "StateAwareness",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    UINT16,
    -1, /* subscript */
    CIMPLE_OFF(CIM_EnabledLogicalElementCapabilities,StateAwareness),
    0, /* value */
};

/*[2291]*/
static Meta_Feature* _CIM_EnabledLogicalElementCapabilities_MFA[] =
{
    (Meta_Feature*)(void*)&_CIM_Capabilities_InstanceID,
    (Meta_Feature*)(void*)&_CIM_ManagedElement_Caption,
    (Meta_Feature*)(void*)&_CIM_ManagedElement_Description,
    (Meta_Feature*)(void*)&_CIM_Capabilities_ElementName,
    (Meta_Feature*)(void*)&_CIM_ManagedElement_Generation,
    (Meta_Feature*)(void*)&CIM_Capabilities_CreateGoalSettings_method::static_meta_class,
    (Meta_Feature*)(void*)&_CIM_EnabledLogicalElementCapabilities_ElementNameEditSupported,
    (Meta_Feature*)(void*)&_CIM_EnabledLogicalElementCapabilities_MaxElementNameLen,
    (Meta_Feature*)(void*)&_CIM_EnabledLogicalElementCapabilities_RequestedStatesSupported,
    (Meta_Feature*)(void*)&_CIM_EnabledLogicalElementCapabilities_ElementNameMask,
    (Meta_Feature*)(void*)&_CIM_EnabledLogicalElementCapabilities_StateAwareness,
};

/*[2338]*/
static const Meta_Feature_Local _locals[] =
{
    {0},
    {0},
    {0},
    {0},
    {0},
    {0},
    {1},
    {1},
    {1},
    {1},
    {1},
};

/*[2358]*/
const Meta_Class CIM_EnabledLogicalElementCapabilities::static_meta_class =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_CLASS,
    "CIM_EnabledLogicalElementCapabilities",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    _CIM_EnabledLogicalElementCapabilities_MFA,
    CIMPLE_ARRAY_SIZE(_CIM_EnabledLogicalElementCapabilities_MFA),
    sizeof(CIM_EnabledLogicalElementCapabilities),
    _locals,
    &CIM_Capabilities::static_meta_class,
    1, /* num_keys */
    &__meta_repository_0DDA66E0FF471BAB95C366A9360C9285,
};

CIMPLE_NAMESPACE_END

