/*NOCHKSRC*/
//==============================================================================
//
// PLEASE DO NOT EDIT; THIS FILE WAS AUTOMATICALLY GENERATED BY GENCLASS 2.0.24
//
//==============================================================================

#include <cimple/Meta_Class.h>
#include <cimple/Meta_Property.h>
#include <cimple/Meta_Reference.h>
#include "CIM_LogicalElement.h"

CIMPLE_NAMESPACE_BEGIN

using namespace cimple;

extern const Meta_Repository __meta_repository_6CA0219685691FADA8733C94FE01BD97;

/*[1568]*/
extern const Meta_Property
_CIM_ManagedElement_InstanceID;

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

/*[2291]*/
static Meta_Feature* _CIM_LogicalElement_MFA[] =
{
    (Meta_Feature*)(void*)&_CIM_ManagedElement_InstanceID,
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
};

/*[2358]*/
const Meta_Class CIM_LogicalElement::static_meta_class =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_CLASS|CIMPLE_FLAG_ABSTRACT,
    "CIM_LogicalElement",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    _CIM_LogicalElement_MFA,
    CIMPLE_ARRAY_SIZE(_CIM_LogicalElement_MFA),
    sizeof(CIM_LogicalElement),
    _locals,
    &CIM_ManagedSystemElement::static_meta_class,
    0, /* num_keys */
    &__meta_repository_6CA0219685691FADA8733C94FE01BD97,
};

CIMPLE_NAMESPACE_END

