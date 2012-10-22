/*NOCHKSRC*/
//==============================================================================
//
// PLEASE DO NOT EDIT; THIS FILE WAS AUTOMATICALLY GENERATED BY GENCLASS 2.0.24
//
//==============================================================================

#include <cimple/Meta_Class.h>
#include <cimple/Meta_Property.h>
#include <cimple/Meta_Reference.h>
#include "SF_ControlledBy.h"

CIMPLE_NAMESPACE_BEGIN

using namespace cimple;

extern const Meta_Repository __meta_repository_D1C632A09C56102581AE6FCCFC9FF5F6;

/*[1653]*/
extern const Meta_Reference
_CIM_ControlledBy_Antecedent;

/*[1653]*/
extern const Meta_Reference
_CIM_ControlledBy_Dependent;

/*[1568]*/
extern const Meta_Property
_CIM_DeviceConnection_NegotiatedSpeed;

/*[1568]*/
extern const Meta_Property
_CIM_DeviceConnection_NegotiatedDataWidth;

/*[1568]*/
extern const Meta_Property
_CIM_ControlledBy_AccessState;

/*[1568]*/
extern const Meta_Property
_CIM_ControlledBy_TimeOfDeviceReset;

/*[1568]*/
extern const Meta_Property
_CIM_ControlledBy_NumberOfHardResets;

/*[1568]*/
extern const Meta_Property
_CIM_ControlledBy_NumberOfSoftResets;

/*[1568]*/
extern const Meta_Property
_CIM_ControlledBy_DeviceNumber;

/*[1568]*/
extern const Meta_Property
_CIM_ControlledBy_AccessMode;

/*[1568]*/
extern const Meta_Property
_CIM_ControlledBy_AccessPriority;

/*[2291]*/
static Meta_Feature* _SF_ControlledBy_MFA[] =
{
    (Meta_Feature*)(void*)&_CIM_ControlledBy_Antecedent,
    (Meta_Feature*)(void*)&_CIM_ControlledBy_Dependent,
    (Meta_Feature*)(void*)&_CIM_DeviceConnection_NegotiatedSpeed,
    (Meta_Feature*)(void*)&_CIM_DeviceConnection_NegotiatedDataWidth,
    (Meta_Feature*)(void*)&_CIM_ControlledBy_AccessState,
    (Meta_Feature*)(void*)&_CIM_ControlledBy_TimeOfDeviceReset,
    (Meta_Feature*)(void*)&_CIM_ControlledBy_NumberOfHardResets,
    (Meta_Feature*)(void*)&_CIM_ControlledBy_NumberOfSoftResets,
    (Meta_Feature*)(void*)&_CIM_ControlledBy_DeviceNumber,
    (Meta_Feature*)(void*)&_CIM_ControlledBy_AccessMode,
    (Meta_Feature*)(void*)&_CIM_ControlledBy_AccessPriority,
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
};

/*[2358]*/
const Meta_Class SF_ControlledBy::static_meta_class =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_ASSOCIATION,
    "SF_ControlledBy",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    _SF_ControlledBy_MFA,
    CIMPLE_ARRAY_SIZE(_SF_ControlledBy_MFA),
    sizeof(SF_ControlledBy),
    _locals,
    &CIM_ControlledBy::static_meta_class,
    2, /* num_keys */
    &__meta_repository_D1C632A09C56102581AE6FCCFC9FF5F6,
};

CIMPLE_NAMESPACE_END

