/*NOCHKSRC*/
//==============================================================================
//
// PLEASE DO NOT EDIT; THIS FILE WAS AUTOMATICALLY GENERATED BY GENCLASS 2.0.24
//
//==============================================================================

#include <cimple/Meta_Class.h>
#include <cimple/Meta_Property.h>
#include <cimple/Meta_Reference.h>
#include "CIM_RegisteredProfile.h"

CIMPLE_NAMESPACE_BEGIN

using namespace cimple;

extern const Meta_Repository __meta_repository_0DDA66E0FF471BAB95C366A9360C9285;

/*[1568]*/
extern const Meta_Property
_CIM_RegisteredSpecification_InstanceID;

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
_CIM_ManagedElement_Generation;

/*[1568]*/
extern const Meta_Property
_CIM_RegisteredProfile_SpecificationType;

/*[1591]*/
const Meta_Property
_CIM_RegisteredProfile_SpecificationType =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "SpecificationType",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    UINT16,
    0, /* subscript */
    CIMPLE_OFF(CIM_RegisteredProfile,SpecificationType),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_RegisteredSpecification_OtherSpecificationType;

/*[1568]*/
extern const Meta_Property
_CIM_RegisteredSpecification_RegisteredOrganization;

/*[1568]*/
extern const Meta_Property
_CIM_RegisteredSpecification_OtherRegisteredOrganization;

/*[1568]*/
extern const Meta_Property
_CIM_RegisteredSpecification_RegisteredName;

/*[1568]*/
extern const Meta_Property
_CIM_RegisteredSpecification_RegisteredVersion;

/*[1568]*/
extern const Meta_Property
_CIM_RegisteredSpecification_AdvertiseTypes;

/*[1568]*/
extern const Meta_Property
_CIM_RegisteredSpecification_AdvertiseTypeDescriptions;

/*[1568]*/
extern const Meta_Property
_CIM_RegisteredProfile_ImplementedFeatures;

/*[1591]*/
const Meta_Property
_CIM_RegisteredProfile_ImplementedFeatures =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_EXPERIMENTAL|CIMPLE_FLAG_READ,
    "ImplementedFeatures",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    STRING,
    -1, /* subscript */
    CIMPLE_OFF(CIM_RegisteredProfile,ImplementedFeatures),
    0, /* value */
};

/*[1782]*/
static const Meta_Property
_CIM_RegisteredProfile_CloseConformantInstances_EnumerationContext =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_IN,
    "EnumerationContext",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    STRING,
    0,
    CIMPLE_OFF(CIM_RegisteredProfile_CloseConformantInstances_method,EnumerationContext),
    0, /* value */
};

/*[1935]*/
static const Meta_Property
_CIM_RegisteredProfile_CloseConformantInstances_return_value =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_OUT,
    "return_value",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    UINT32,
    0,
    CIMPLE_OFF(CIM_RegisteredProfile_CloseConformantInstances_method,return_value),
    0, /* value */
};

/*[1911]*/
static Meta_Feature* _CIM_RegisteredProfile_CloseConformantInstances_MFA[] =
{
    (Meta_Feature*)(void*)&_CIM_RegisteredProfile_CloseConformantInstances_EnumerationContext,
    (Meta_Feature*)(void*)&_CIM_RegisteredProfile_CloseConformantInstances_return_value
};

/*[2113]*/
const Meta_Method
CIM_RegisteredProfile_CloseConformantInstances_method::static_meta_class =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_METHOD|CIMPLE_FLAG_EXPERIMENTAL,
    "CloseConformantInstances",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    _CIM_RegisteredProfile_CloseConformantInstances_MFA,
    CIMPLE_ARRAY_SIZE(_CIM_RegisteredProfile_CloseConformantInstances_MFA),
    sizeof(CIM_RegisteredProfile_CloseConformantInstances_method),
    UINT32,
};

/*[1782]*/
static const Meta_Property
_CIM_RegisteredProfile_OpenConformantInstances_ResultClass =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_IN,
    "ResultClass",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    STRING,
    0,
    CIMPLE_OFF(CIM_RegisteredProfile_OpenConformantInstances_method,ResultClass),
    0, /* value */
};

/*[1782]*/
static const Meta_Property
_CIM_RegisteredProfile_OpenConformantInstances_IncludedPropertyList =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_IN,
    "IncludedPropertyList",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    STRING,
    -1,
    CIMPLE_OFF(CIM_RegisteredProfile_OpenConformantInstances_method,IncludedPropertyList),
    0, /* value */
};

/*[1782]*/
static const Meta_Property
_CIM_RegisteredProfile_OpenConformantInstances_OperationTimeout =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_IN,
    "OperationTimeout",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    UINT32,
    0,
    CIMPLE_OFF(CIM_RegisteredProfile_OpenConformantInstances_method,OperationTimeout),
    0, /* value */
};

/*[1782]*/
static const Meta_Property
_CIM_RegisteredProfile_OpenConformantInstances_ContinueOnError =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_IN,
    "ContinueOnError",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    BOOLEAN,
    0,
    CIMPLE_OFF(CIM_RegisteredProfile_OpenConformantInstances_method,ContinueOnError),
    0, /* value */
};

/*[1782]*/
static const Meta_Property
_CIM_RegisteredProfile_OpenConformantInstances_MaxObjectCount =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_IN,
    "MaxObjectCount",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    UINT32,
    0,
    CIMPLE_OFF(CIM_RegisteredProfile_OpenConformantInstances_method,MaxObjectCount),
    0, /* value */
};

/*[1782]*/
static const Meta_Property
_CIM_RegisteredProfile_OpenConformantInstances_EnumerationContext =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_OUT,
    "EnumerationContext",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    STRING,
    0,
    CIMPLE_OFF(CIM_RegisteredProfile_OpenConformantInstances_method,EnumerationContext),
    0, /* value */
};

/*[1782]*/
static const Meta_Property
_CIM_RegisteredProfile_OpenConformantInstances_EndOfSequence =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_OUT,
    "EndOfSequence",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    BOOLEAN,
    0,
    CIMPLE_OFF(CIM_RegisteredProfile_OpenConformantInstances_method,EndOfSequence),
    0, /* value */
};

/*[1782]*/
static const Meta_Property
_CIM_RegisteredProfile_OpenConformantInstances_InstanceType =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_OUT,
    "InstanceType",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    UINT16,
    -1,
    CIMPLE_OFF(CIM_RegisteredProfile_OpenConformantInstances_method,InstanceType),
    0, /* value */
};

/*[1846]*/
static const Meta_Reference
_CIM_RegisteredProfile_OpenConformantInstances_InstanceWithPathList =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_REFERENCE|CIMPLE_FLAG_EMBEDDED_INSTANCE|CIMPLE_FLAG_OUT,
    "InstanceWithPathList",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    -1, /* subscript */
    &CIM_ManagedElement::static_meta_class,
    CIMPLE_OFF(CIM_RegisteredProfile_OpenConformantInstances_method,InstanceWithPathList)
};

/*[1935]*/
static const Meta_Property
_CIM_RegisteredProfile_OpenConformantInstances_return_value =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_OUT,
    "return_value",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    UINT32,
    0,
    CIMPLE_OFF(CIM_RegisteredProfile_OpenConformantInstances_method,return_value),
    0, /* value */
};

/*[1911]*/
static Meta_Feature* _CIM_RegisteredProfile_OpenConformantInstances_MFA[] =
{
    (Meta_Feature*)(void*)&_CIM_RegisteredProfile_OpenConformantInstances_ResultClass,
    (Meta_Feature*)(void*)&_CIM_RegisteredProfile_OpenConformantInstances_IncludedPropertyList,
    (Meta_Feature*)(void*)&_CIM_RegisteredProfile_OpenConformantInstances_OperationTimeout,
    (Meta_Feature*)(void*)&_CIM_RegisteredProfile_OpenConformantInstances_ContinueOnError,
    (Meta_Feature*)(void*)&_CIM_RegisteredProfile_OpenConformantInstances_MaxObjectCount,
    (Meta_Feature*)(void*)&_CIM_RegisteredProfile_OpenConformantInstances_EnumerationContext,
    (Meta_Feature*)(void*)&_CIM_RegisteredProfile_OpenConformantInstances_EndOfSequence,
    (Meta_Feature*)(void*)&_CIM_RegisteredProfile_OpenConformantInstances_InstanceType,
    (Meta_Feature*)(void*)&_CIM_RegisteredProfile_OpenConformantInstances_InstanceWithPathList,
    (Meta_Feature*)(void*)&_CIM_RegisteredProfile_OpenConformantInstances_return_value
};

/*[2113]*/
const Meta_Method
CIM_RegisteredProfile_OpenConformantInstances_method::static_meta_class =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_METHOD|CIMPLE_FLAG_EXPERIMENTAL,
    "OpenConformantInstances",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    _CIM_RegisteredProfile_OpenConformantInstances_MFA,
    CIMPLE_ARRAY_SIZE(_CIM_RegisteredProfile_OpenConformantInstances_MFA),
    sizeof(CIM_RegisteredProfile_OpenConformantInstances_method),
    UINT32,
};

/*[1782]*/
static const Meta_Property
_CIM_RegisteredProfile_PullConformantInstances_MaxObjectCount =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_IN,
    "MaxObjectCount",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    UINT32,
    0,
    CIMPLE_OFF(CIM_RegisteredProfile_PullConformantInstances_method,MaxObjectCount),
    0, /* value */
};

/*[1782]*/
static const Meta_Property
_CIM_RegisteredProfile_PullConformantInstances_EnumerationContext =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_IN|CIMPLE_FLAG_OUT,
    "EnumerationContext",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    STRING,
    0,
    CIMPLE_OFF(CIM_RegisteredProfile_PullConformantInstances_method,EnumerationContext),
    0, /* value */
};

/*[1782]*/
static const Meta_Property
_CIM_RegisteredProfile_PullConformantInstances_EndOfSequence =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_OUT,
    "EndOfSequence",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    BOOLEAN,
    0,
    CIMPLE_OFF(CIM_RegisteredProfile_PullConformantInstances_method,EndOfSequence),
    0, /* value */
};

/*[1782]*/
static const Meta_Property
_CIM_RegisteredProfile_PullConformantInstances_InstanceType =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_OUT,
    "InstanceType",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    UINT16,
    -1,
    CIMPLE_OFF(CIM_RegisteredProfile_PullConformantInstances_method,InstanceType),
    0, /* value */
};

/*[1846]*/
static const Meta_Reference
_CIM_RegisteredProfile_PullConformantInstances_InstanceWithPathList =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_REFERENCE|CIMPLE_FLAG_EMBEDDED_INSTANCE|CIMPLE_FLAG_OUT,
    "InstanceWithPathList",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    -1, /* subscript */
    &CIM_ManagedElement::static_meta_class,
    CIMPLE_OFF(CIM_RegisteredProfile_PullConformantInstances_method,InstanceWithPathList)
};

/*[1935]*/
static const Meta_Property
_CIM_RegisteredProfile_PullConformantInstances_return_value =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_OUT,
    "return_value",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    UINT32,
    0,
    CIMPLE_OFF(CIM_RegisteredProfile_PullConformantInstances_method,return_value),
    0, /* value */
};

/*[1911]*/
static Meta_Feature* _CIM_RegisteredProfile_PullConformantInstances_MFA[] =
{
    (Meta_Feature*)(void*)&_CIM_RegisteredProfile_PullConformantInstances_MaxObjectCount,
    (Meta_Feature*)(void*)&_CIM_RegisteredProfile_PullConformantInstances_EnumerationContext,
    (Meta_Feature*)(void*)&_CIM_RegisteredProfile_PullConformantInstances_EndOfSequence,
    (Meta_Feature*)(void*)&_CIM_RegisteredProfile_PullConformantInstances_InstanceType,
    (Meta_Feature*)(void*)&_CIM_RegisteredProfile_PullConformantInstances_InstanceWithPathList,
    (Meta_Feature*)(void*)&_CIM_RegisteredProfile_PullConformantInstances_return_value
};

/*[2113]*/
const Meta_Method
CIM_RegisteredProfile_PullConformantInstances_method::static_meta_class =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_METHOD|CIMPLE_FLAG_EXPERIMENTAL,
    "PullConformantInstances",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    _CIM_RegisteredProfile_PullConformantInstances_MFA,
    CIMPLE_ARRAY_SIZE(_CIM_RegisteredProfile_PullConformantInstances_MFA),
    sizeof(CIM_RegisteredProfile_PullConformantInstances_method),
    UINT32,
};

/*[2291]*/
static Meta_Feature* _CIM_RegisteredProfile_MFA[] =
{
    (Meta_Feature*)(void*)&_CIM_RegisteredSpecification_InstanceID,
    (Meta_Feature*)(void*)&_CIM_ManagedElement_Caption,
    (Meta_Feature*)(void*)&_CIM_ManagedElement_Description,
    (Meta_Feature*)(void*)&_CIM_ManagedElement_ElementName,
    (Meta_Feature*)(void*)&_CIM_ManagedElement_Generation,
    (Meta_Feature*)(void*)&_CIM_RegisteredProfile_SpecificationType,
    (Meta_Feature*)(void*)&_CIM_RegisteredSpecification_OtherSpecificationType,
    (Meta_Feature*)(void*)&_CIM_RegisteredSpecification_RegisteredOrganization,
    (Meta_Feature*)(void*)&_CIM_RegisteredSpecification_OtherRegisteredOrganization,
    (Meta_Feature*)(void*)&_CIM_RegisteredSpecification_RegisteredName,
    (Meta_Feature*)(void*)&_CIM_RegisteredSpecification_RegisteredVersion,
    (Meta_Feature*)(void*)&_CIM_RegisteredSpecification_AdvertiseTypes,
    (Meta_Feature*)(void*)&_CIM_RegisteredSpecification_AdvertiseTypeDescriptions,
    (Meta_Feature*)(void*)&_CIM_RegisteredProfile_ImplementedFeatures,
    (Meta_Feature*)(void*)&CIM_RegisteredProfile_CloseConformantInstances_method::static_meta_class,
    (Meta_Feature*)(void*)&CIM_RegisteredProfile_OpenConformantInstances_method::static_meta_class,
    (Meta_Feature*)(void*)&CIM_RegisteredProfile_PullConformantInstances_method::static_meta_class,
};

/*[2338]*/
static const Meta_Feature_Local _locals[] =
{
    {0},
    {0},
    {0},
    {0},
    {0},
    {1},
    {0},
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
};

/*[2358]*/
const Meta_Class CIM_RegisteredProfile::static_meta_class =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_CLASS,
    "CIM_RegisteredProfile",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    _CIM_RegisteredProfile_MFA,
    CIMPLE_ARRAY_SIZE(_CIM_RegisteredProfile_MFA),
    sizeof(CIM_RegisteredProfile),
    _locals,
    &CIM_RegisteredSpecification::static_meta_class,
    1, /* num_keys */
    &__meta_repository_0DDA66E0FF471BAB95C366A9360C9285,
};

CIMPLE_NAMESPACE_END

