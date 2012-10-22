/*NOCHKSRC*/
//==============================================================================
//
// PLEASE DO NOT EDIT; THIS FILE WAS AUTOMATICALLY GENERATED BY GENCLASS 2.0.24
//
//==============================================================================

#include <cimple/Meta_Class.h>
#include <cimple/Meta_Property.h>
#include <cimple/Meta_Reference.h>
#include "CIM_PhysicalConnector.h"

CIMPLE_NAMESPACE_BEGIN

using namespace cimple;

extern const Meta_Repository __meta_repository_322BD996DC5A15A5B9D5D42DDD2F5A5E;

/*[1568]*/
extern const Meta_Property
_CIM_ManagedElement_InstanceID;

/*[1568]*/
extern const Meta_Property
_CIM_ManagedElement_Caption;

/*[1568]*/
extern const Meta_Property
_CIM_PhysicalElement_Description;

/*[1568]*/
extern const Meta_Property
_CIM_PhysicalElement_ElementName;

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
_CIM_PhysicalElement_Tag;

/*[1568]*/
extern const Meta_Property
_CIM_PhysicalElement_CreationClassName;

/*[1568]*/
extern const Meta_Property
_CIM_PhysicalElement_Manufacturer;

/*[1568]*/
extern const Meta_Property
_CIM_PhysicalElement_Model;

/*[1568]*/
extern const Meta_Property
_CIM_PhysicalElement_SKU;

/*[1568]*/
extern const Meta_Property
_CIM_PhysicalElement_SerialNumber;

/*[1568]*/
extern const Meta_Property
_CIM_PhysicalElement_Version;

/*[1568]*/
extern const Meta_Property
_CIM_PhysicalElement_PartNumber;

/*[1568]*/
extern const Meta_Property
_CIM_PhysicalElement_OtherIdentifyingInfo;

/*[1568]*/
extern const Meta_Property
_CIM_PhysicalElement_PoweredOn;

/*[1568]*/
extern const Meta_Property
_CIM_PhysicalElement_ManufactureDate;

/*[1568]*/
extern const Meta_Property
_CIM_PhysicalElement_VendorEquipmentType;

/*[1568]*/
extern const Meta_Property
_CIM_PhysicalElement_UserTracking;

/*[1568]*/
extern const Meta_Property
_CIM_PhysicalElement_CanBeFRUed;

/*[1568]*/
extern const Meta_Property
_CIM_PhysicalConnector_ConnectorPinout;

/*[1591]*/
const Meta_Property
_CIM_PhysicalConnector_ConnectorPinout =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "ConnectorPinout",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    STRING,
    0, /* subscript */
    CIMPLE_OFF(CIM_PhysicalConnector,ConnectorPinout),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_PhysicalConnector_ConnectorType;

/*[1591]*/
const Meta_Property
_CIM_PhysicalConnector_ConnectorType =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "ConnectorType",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    UINT16,
    -1, /* subscript */
    CIMPLE_OFF(CIM_PhysicalConnector,ConnectorType),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_PhysicalConnector_OtherTypeDescription;

/*[1591]*/
const Meta_Property
_CIM_PhysicalConnector_OtherTypeDescription =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "OtherTypeDescription",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    STRING,
    0, /* subscript */
    CIMPLE_OFF(CIM_PhysicalConnector,OtherTypeDescription),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_PhysicalConnector_ConnectorGender;

/*[1591]*/
const Meta_Property
_CIM_PhysicalConnector_ConnectorGender =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "ConnectorGender",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    UINT16,
    0, /* subscript */
    CIMPLE_OFF(CIM_PhysicalConnector,ConnectorGender),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_PhysicalConnector_ConnectorElectricalCharacteristics;

/*[1591]*/
const Meta_Property
_CIM_PhysicalConnector_ConnectorElectricalCharacteristics =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "ConnectorElectricalCharacteristics",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    UINT16,
    -1, /* subscript */
    CIMPLE_OFF(CIM_PhysicalConnector,ConnectorElectricalCharacteristics),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_PhysicalConnector_OtherElectricalCharacteristics;

/*[1591]*/
const Meta_Property
_CIM_PhysicalConnector_OtherElectricalCharacteristics =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "OtherElectricalCharacteristics",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    STRING,
    -1, /* subscript */
    CIMPLE_OFF(CIM_PhysicalConnector,OtherElectricalCharacteristics),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_PhysicalConnector_NumPhysicalPins;

/*[1591]*/
const Meta_Property
_CIM_PhysicalConnector_NumPhysicalPins =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "NumPhysicalPins",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    UINT32,
    0, /* subscript */
    CIMPLE_OFF(CIM_PhysicalConnector,NumPhysicalPins),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_PhysicalConnector_ConnectorLayout;

/*[1591]*/
const Meta_Property
_CIM_PhysicalConnector_ConnectorLayout =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "ConnectorLayout",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    UINT16,
    0, /* subscript */
    CIMPLE_OFF(CIM_PhysicalConnector,ConnectorLayout),
    0, /* value */
};

/*[1568]*/
extern const Meta_Property
_CIM_PhysicalConnector_ConnectorDescription;

/*[1591]*/
const Meta_Property
_CIM_PhysicalConnector_ConnectorDescription =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "ConnectorDescription",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    STRING,
    0, /* subscript */
    CIMPLE_OFF(CIM_PhysicalConnector,ConnectorDescription),
    0, /* value */
};

/*[2291]*/
static Meta_Feature* _CIM_PhysicalConnector_MFA[] =
{
    (Meta_Feature*)(void*)&_CIM_ManagedElement_InstanceID,
    (Meta_Feature*)(void*)&_CIM_ManagedElement_Caption,
    (Meta_Feature*)(void*)&_CIM_PhysicalElement_Description,
    (Meta_Feature*)(void*)&_CIM_PhysicalElement_ElementName,
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
    (Meta_Feature*)(void*)&_CIM_PhysicalElement_Tag,
    (Meta_Feature*)(void*)&_CIM_PhysicalElement_CreationClassName,
    (Meta_Feature*)(void*)&_CIM_PhysicalElement_Manufacturer,
    (Meta_Feature*)(void*)&_CIM_PhysicalElement_Model,
    (Meta_Feature*)(void*)&_CIM_PhysicalElement_SKU,
    (Meta_Feature*)(void*)&_CIM_PhysicalElement_SerialNumber,
    (Meta_Feature*)(void*)&_CIM_PhysicalElement_Version,
    (Meta_Feature*)(void*)&_CIM_PhysicalElement_PartNumber,
    (Meta_Feature*)(void*)&_CIM_PhysicalElement_OtherIdentifyingInfo,
    (Meta_Feature*)(void*)&_CIM_PhysicalElement_PoweredOn,
    (Meta_Feature*)(void*)&_CIM_PhysicalElement_ManufactureDate,
    (Meta_Feature*)(void*)&_CIM_PhysicalElement_VendorEquipmentType,
    (Meta_Feature*)(void*)&_CIM_PhysicalElement_UserTracking,
    (Meta_Feature*)(void*)&_CIM_PhysicalElement_CanBeFRUed,
    (Meta_Feature*)(void*)&_CIM_PhysicalConnector_ConnectorPinout,
    (Meta_Feature*)(void*)&_CIM_PhysicalConnector_ConnectorType,
    (Meta_Feature*)(void*)&_CIM_PhysicalConnector_OtherTypeDescription,
    (Meta_Feature*)(void*)&_CIM_PhysicalConnector_ConnectorGender,
    (Meta_Feature*)(void*)&_CIM_PhysicalConnector_ConnectorElectricalCharacteristics,
    (Meta_Feature*)(void*)&_CIM_PhysicalConnector_OtherElectricalCharacteristics,
    (Meta_Feature*)(void*)&_CIM_PhysicalConnector_NumPhysicalPins,
    (Meta_Feature*)(void*)&_CIM_PhysicalConnector_ConnectorLayout,
    (Meta_Feature*)(void*)&_CIM_PhysicalConnector_ConnectorDescription,
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
};

/*[2358]*/
const Meta_Class CIM_PhysicalConnector::static_meta_class =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_CLASS,
    "CIM_PhysicalConnector",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    _CIM_PhysicalConnector_MFA,
    CIMPLE_ARRAY_SIZE(_CIM_PhysicalConnector_MFA),
    sizeof(CIM_PhysicalConnector),
    _locals,
    &CIM_PhysicalElement::static_meta_class,
    2, /* num_keys */
    &__meta_repository_322BD996DC5A15A5B9D5D42DDD2F5A5E,
};

CIMPLE_NAMESPACE_END

