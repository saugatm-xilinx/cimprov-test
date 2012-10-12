/*NOCHKSRC*/
//==============================================================================
//
// PLEASE DO NOT EDIT; THIS FILE WAS AUTOMATICALLY GENERATED BY GENCLASS 2.0.24
//
//==============================================================================

#include <cimple/Meta_Class.h>
#include <cimple/Meta_Property.h>
#include <cimple/Meta_Reference.h>
#include "CIM_SoftwareIdentity.h"

CIMPLE_NAMESPACE_BEGIN

using namespace cimple;

extern const Meta_Repository __meta_repository_6CA0219685691FADA8733C94FE01BD97;

/*[1568]*/
extern const Meta_Property
_CIM_SoftwareIdentity_InstanceID;

/*[1591]*/
const Meta_Property
_CIM_SoftwareIdentity_InstanceID =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_KEY|CIMPLE_FLAG_READ,
    "InstanceID",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    STRING,
    0, /* subscript */
    CIMPLE_OFF(CIM_SoftwareIdentity,InstanceID),
    0, /* value */
};

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
_CIM_ManagedSystemElement_InstallDate;

/*[1568]*/
extern const Meta_Property
_CIM_ManagedSystemElement_Name;

/*[1568]*/
extern const Meta_Property
_CIM_ManagedSystemElement_OperationalStatus;

/*[1568]*/
extern const Meta_Property
_CIM_ManagedSystemElement_StatusDescriptions;

/*[1568]*/
extern const Meta_Property
_CIM_ManagedSystemElement_Status;

/*[1568]*/
extern const Meta_Property
_CIM_ManagedSystemElement_HealthState;

/*[1568]*/
extern const Meta_Property
_CIM_ManagedSystemElement_CommunicationStatus;

/*[1568]*/
extern const Meta_Property
_CIM_ManagedSystemElement_DetailedStatus;

/*[1568]*/
extern const Meta_Property
_CIM_ManagedSystemElement_OperatingStatus;

/*[1568]*/
extern const Meta_Property
_CIM_ManagedSystemElement_PrimaryStatus;

/*[1568]*/
extern const Meta_Property
_CIM_SoftwareIdentity_MajorVersion;

/*[1591]*/
const Meta_Property
_CIM_SoftwareIdentity_MajorVersion =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "MajorVersion",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    UINT16,
    0, /* subscript */
    CIMPLE_OFF(CIM_SoftwareIdentity,MajorVersion),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_SoftwareIdentity_MinorVersion;

/*[1591]*/
const Meta_Property
_CIM_SoftwareIdentity_MinorVersion =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "MinorVersion",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    UINT16,
    0, /* subscript */
    CIMPLE_OFF(CIM_SoftwareIdentity,MinorVersion),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_SoftwareIdentity_RevisionNumber;

/*[1591]*/
const Meta_Property
_CIM_SoftwareIdentity_RevisionNumber =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "RevisionNumber",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    UINT16,
    0, /* subscript */
    CIMPLE_OFF(CIM_SoftwareIdentity,RevisionNumber),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_SoftwareIdentity_BuildNumber;

/*[1591]*/
const Meta_Property
_CIM_SoftwareIdentity_BuildNumber =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "BuildNumber",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    UINT16,
    0, /* subscript */
    CIMPLE_OFF(CIM_SoftwareIdentity,BuildNumber),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_SoftwareIdentity_LargeBuildNumber;

/*[1591]*/
const Meta_Property
_CIM_SoftwareIdentity_LargeBuildNumber =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "LargeBuildNumber",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    UINT64,
    0, /* subscript */
    CIMPLE_OFF(CIM_SoftwareIdentity,LargeBuildNumber),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_SoftwareIdentity_IsLargeBuildNumber;

/*[540]*/
static const Meta_Value_Scalar<boolean>
_CIM_SoftwareIdentity_IsLargeBuildNumber_MV =
{
    false
};

/*[1591]*/
const Meta_Property
_CIM_SoftwareIdentity_IsLargeBuildNumber =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "IsLargeBuildNumber",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    BOOLEAN,
    0, /* subscript */
    CIMPLE_OFF(CIM_SoftwareIdentity,IsLargeBuildNumber),
    (const Meta_Value*)(void*)&_CIM_SoftwareIdentity_IsLargeBuildNumber_MV,
};

/*[1568]*/
extern const Meta_Property
_CIM_SoftwareIdentity_VersionString;

/*[1591]*/
const Meta_Property
_CIM_SoftwareIdentity_VersionString =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "VersionString",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    STRING,
    0, /* subscript */
    CIMPLE_OFF(CIM_SoftwareIdentity,VersionString),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_SoftwareIdentity_TargetOperatingSystems;

/*[1591]*/
const Meta_Property
_CIM_SoftwareIdentity_TargetOperatingSystems =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "TargetOperatingSystems",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    STRING,
    -1, /* subscript */
    CIMPLE_OFF(CIM_SoftwareIdentity,TargetOperatingSystems),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_SoftwareIdentity_Manufacturer;

/*[1591]*/
const Meta_Property
_CIM_SoftwareIdentity_Manufacturer =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "Manufacturer",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    STRING,
    0, /* subscript */
    CIMPLE_OFF(CIM_SoftwareIdentity,Manufacturer),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_SoftwareIdentity_Languages;

/*[1591]*/
const Meta_Property
_CIM_SoftwareIdentity_Languages =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "Languages",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    STRING,
    -1, /* subscript */
    CIMPLE_OFF(CIM_SoftwareIdentity,Languages),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_SoftwareIdentity_Classifications;

/*[1591]*/
const Meta_Property
_CIM_SoftwareIdentity_Classifications =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "Classifications",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    UINT16,
    -1, /* subscript */
    CIMPLE_OFF(CIM_SoftwareIdentity,Classifications),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_SoftwareIdentity_ClassificationDescriptions;

/*[1591]*/
const Meta_Property
_CIM_SoftwareIdentity_ClassificationDescriptions =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "ClassificationDescriptions",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    STRING,
    -1, /* subscript */
    CIMPLE_OFF(CIM_SoftwareIdentity,ClassificationDescriptions),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_SoftwareIdentity_SerialNumber;

/*[1591]*/
const Meta_Property
_CIM_SoftwareIdentity_SerialNumber =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "SerialNumber",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    STRING,
    0, /* subscript */
    CIMPLE_OFF(CIM_SoftwareIdentity,SerialNumber),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_SoftwareIdentity_TargetTypes;

/*[1591]*/
const Meta_Property
_CIM_SoftwareIdentity_TargetTypes =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "TargetTypes",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    STRING,
    -1, /* subscript */
    CIMPLE_OFF(CIM_SoftwareIdentity,TargetTypes),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_SoftwareIdentity_IdentityInfoValue;

/*[1591]*/
const Meta_Property
_CIM_SoftwareIdentity_IdentityInfoValue =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "IdentityInfoValue",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    STRING,
    -1, /* subscript */
    CIMPLE_OFF(CIM_SoftwareIdentity,IdentityInfoValue),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_SoftwareIdentity_IdentityInfoType;

/*[1591]*/
const Meta_Property
_CIM_SoftwareIdentity_IdentityInfoType =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "IdentityInfoType",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    STRING,
    -1, /* subscript */
    CIMPLE_OFF(CIM_SoftwareIdentity,IdentityInfoType),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_SoftwareIdentity_ReleaseDate;

/*[1591]*/
const Meta_Property
_CIM_SoftwareIdentity_ReleaseDate =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "ReleaseDate",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    DATETIME,
    0, /* subscript */
    CIMPLE_OFF(CIM_SoftwareIdentity,ReleaseDate),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_SoftwareIdentity_IsEntity;

/*[540]*/
static const Meta_Value_Scalar<boolean>
_CIM_SoftwareIdentity_IsEntity_MV =
{
    false
};

/*[1591]*/
const Meta_Property
_CIM_SoftwareIdentity_IsEntity =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "IsEntity",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    BOOLEAN,
    0, /* subscript */
    CIMPLE_OFF(CIM_SoftwareIdentity,IsEntity),
    (const Meta_Value*)(void*)&_CIM_SoftwareIdentity_IsEntity_MV,
};

/*[1568]*/
extern const Meta_Property
_CIM_SoftwareIdentity_ExtendedResourceType;

/*[1591]*/
const Meta_Property
_CIM_SoftwareIdentity_ExtendedResourceType =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "ExtendedResourceType",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    UINT16,
    0, /* subscript */
    CIMPLE_OFF(CIM_SoftwareIdentity,ExtendedResourceType),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_SoftwareIdentity_OtherExtendedResourceTypeDescription;

/*[1591]*/
const Meta_Property
_CIM_SoftwareIdentity_OtherExtendedResourceTypeDescription =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "OtherExtendedResourceTypeDescription",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    STRING,
    0, /* subscript */
    CIMPLE_OFF(CIM_SoftwareIdentity,OtherExtendedResourceTypeDescription),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_SoftwareIdentity_MinExtendedResourceTypeMajorVersion;

/*[1591]*/
const Meta_Property
_CIM_SoftwareIdentity_MinExtendedResourceTypeMajorVersion =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "MinExtendedResourceTypeMajorVersion",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    UINT16,
    0, /* subscript */
    CIMPLE_OFF(CIM_SoftwareIdentity,MinExtendedResourceTypeMajorVersion),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_SoftwareIdentity_MinExtendedResourceTypeMinorVersion;

/*[1591]*/
const Meta_Property
_CIM_SoftwareIdentity_MinExtendedResourceTypeMinorVersion =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "MinExtendedResourceTypeMinorVersion",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    UINT16,
    0, /* subscript */
    CIMPLE_OFF(CIM_SoftwareIdentity,MinExtendedResourceTypeMinorVersion),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_SoftwareIdentity_MinExtendedResourceTypeRevisionNumber;

/*[1591]*/
const Meta_Property
_CIM_SoftwareIdentity_MinExtendedResourceTypeRevisionNumber =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "MinExtendedResourceTypeRevisionNumber",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    UINT16,
    0, /* subscript */
    CIMPLE_OFF(CIM_SoftwareIdentity,MinExtendedResourceTypeRevisionNumber),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_SoftwareIdentity_MinExtendedResourceTypeBuildNumber;

/*[1591]*/
const Meta_Property
_CIM_SoftwareIdentity_MinExtendedResourceTypeBuildNumber =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "MinExtendedResourceTypeBuildNumber",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    UINT16,
    0, /* subscript */
    CIMPLE_OFF(CIM_SoftwareIdentity,MinExtendedResourceTypeBuildNumber),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_SoftwareIdentity_TargetOSTypes;

/*[1591]*/
const Meta_Property
_CIM_SoftwareIdentity_TargetOSTypes =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "TargetOSTypes",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    UINT16,
    -1, /* subscript */
    CIMPLE_OFF(CIM_SoftwareIdentity,TargetOSTypes),
    0, /* value */
};

/*[2291]*/
static Meta_Feature* _CIM_SoftwareIdentity_MFA[] =
{
    (Meta_Feature*)(void*)&_CIM_SoftwareIdentity_InstanceID,
    (Meta_Feature*)(void*)&_CIM_ManagedElement_Caption,
    (Meta_Feature*)(void*)&_CIM_ManagedElement_Description,
    (Meta_Feature*)(void*)&_CIM_ManagedElement_ElementName,
    (Meta_Feature*)(void*)&_CIM_ManagedSystemElement_InstallDate,
    (Meta_Feature*)(void*)&_CIM_ManagedSystemElement_Name,
    (Meta_Feature*)(void*)&_CIM_ManagedSystemElement_OperationalStatus,
    (Meta_Feature*)(void*)&_CIM_ManagedSystemElement_StatusDescriptions,
    (Meta_Feature*)(void*)&_CIM_ManagedSystemElement_Status,
    (Meta_Feature*)(void*)&_CIM_ManagedSystemElement_HealthState,
    (Meta_Feature*)(void*)&_CIM_ManagedSystemElement_CommunicationStatus,
    (Meta_Feature*)(void*)&_CIM_ManagedSystemElement_DetailedStatus,
    (Meta_Feature*)(void*)&_CIM_ManagedSystemElement_OperatingStatus,
    (Meta_Feature*)(void*)&_CIM_ManagedSystemElement_PrimaryStatus,
    (Meta_Feature*)(void*)&_CIM_SoftwareIdentity_MajorVersion,
    (Meta_Feature*)(void*)&_CIM_SoftwareIdentity_MinorVersion,
    (Meta_Feature*)(void*)&_CIM_SoftwareIdentity_RevisionNumber,
    (Meta_Feature*)(void*)&_CIM_SoftwareIdentity_BuildNumber,
    (Meta_Feature*)(void*)&_CIM_SoftwareIdentity_LargeBuildNumber,
    (Meta_Feature*)(void*)&_CIM_SoftwareIdentity_IsLargeBuildNumber,
    (Meta_Feature*)(void*)&_CIM_SoftwareIdentity_VersionString,
    (Meta_Feature*)(void*)&_CIM_SoftwareIdentity_TargetOperatingSystems,
    (Meta_Feature*)(void*)&_CIM_SoftwareIdentity_Manufacturer,
    (Meta_Feature*)(void*)&_CIM_SoftwareIdentity_Languages,
    (Meta_Feature*)(void*)&_CIM_SoftwareIdentity_Classifications,
    (Meta_Feature*)(void*)&_CIM_SoftwareIdentity_ClassificationDescriptions,
    (Meta_Feature*)(void*)&_CIM_SoftwareIdentity_SerialNumber,
    (Meta_Feature*)(void*)&_CIM_SoftwareIdentity_TargetTypes,
    (Meta_Feature*)(void*)&_CIM_SoftwareIdentity_IdentityInfoValue,
    (Meta_Feature*)(void*)&_CIM_SoftwareIdentity_IdentityInfoType,
    (Meta_Feature*)(void*)&_CIM_SoftwareIdentity_ReleaseDate,
    (Meta_Feature*)(void*)&_CIM_SoftwareIdentity_IsEntity,
    (Meta_Feature*)(void*)&_CIM_SoftwareIdentity_ExtendedResourceType,
    (Meta_Feature*)(void*)&_CIM_SoftwareIdentity_OtherExtendedResourceTypeDescription,
    (Meta_Feature*)(void*)&_CIM_SoftwareIdentity_MinExtendedResourceTypeMajorVersion,
    (Meta_Feature*)(void*)&_CIM_SoftwareIdentity_MinExtendedResourceTypeMinorVersion,
    (Meta_Feature*)(void*)&_CIM_SoftwareIdentity_MinExtendedResourceTypeRevisionNumber,
    (Meta_Feature*)(void*)&_CIM_SoftwareIdentity_MinExtendedResourceTypeBuildNumber,
    (Meta_Feature*)(void*)&_CIM_SoftwareIdentity_TargetOSTypes,
};

/*[2338]*/
static const Meta_Feature_Local _locals[] =
{
    {1},
    {0},
    {0},
    {0},
    {0},
    {0},
    {0},
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
    {1},
    {1},
    {1},
    {1},
    {1},
    {1},
    {1},
    {1},
    {1},
    {1},
    {1},
    {1},
    {1},
    {1},
    {1},
    {1},
    {1},
    {1},
    {1},
    {1},
    {1},
};

/*[2358]*/
const Meta_Class CIM_SoftwareIdentity::static_meta_class =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_CLASS,
    "CIM_SoftwareIdentity",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    _CIM_SoftwareIdentity_MFA,
    CIMPLE_ARRAY_SIZE(_CIM_SoftwareIdentity_MFA),
    sizeof(CIM_SoftwareIdentity),
    _locals,
    &CIM_LogicalElement::static_meta_class,
    1, /* num_keys */
    &__meta_repository_6CA0219685691FADA8733C94FE01BD97,
};

CIMPLE_NAMESPACE_END

