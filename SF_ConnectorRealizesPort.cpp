/*NOCHKSRC*/
//==============================================================================
//
// PLEASE DO NOT EDIT; THIS FILE WAS AUTOMATICALLY GENERATED BY GENCLASS 2.0.24
//
//==============================================================================

#include <cimple/Meta_Class.h>
#include <cimple/Meta_Property.h>
#include <cimple/Meta_Reference.h>
#include "SF_ConnectorRealizesPort.h"

CIMPLE_NAMESPACE_BEGIN

using namespace cimple;

extern const Meta_Repository __meta_repository_7D6DA94023C0157F82D3A7310346C853;

/*[1653]*/
extern const Meta_Reference
_CIM_Realizes_Antecedent;

/*[1653]*/
extern const Meta_Reference
_CIM_Realizes_Dependent;

/*[2291]*/
static Meta_Feature* _SF_ConnectorRealizesPort_MFA[] =
{
    (Meta_Feature*)(void*)&_CIM_Realizes_Antecedent,
    (Meta_Feature*)(void*)&_CIM_Realizes_Dependent,
};

/*[2338]*/
static const Meta_Feature_Local _locals[] =
{
    {0},
    {0},
};

/*[2358]*/
const Meta_Class SF_ConnectorRealizesPort::static_meta_class =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_ASSOCIATION,
    "SF_ConnectorRealizesPort",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    _SF_ConnectorRealizesPort_MFA,
    CIMPLE_ARRAY_SIZE(_SF_ConnectorRealizesPort_MFA),
    sizeof(SF_ConnectorRealizesPort),
    _locals,
    &CIM_Realizes::static_meta_class,
    2, /* num_keys */
    &__meta_repository_7D6DA94023C0157F82D3A7310346C853,
};

CIMPLE_NAMESPACE_END

