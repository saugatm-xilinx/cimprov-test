/*NOCHKSRC*/
//==============================================================================
//
// PLEASE DO NOT EDIT; THIS FILE WAS AUTOMATICALLY GENERATED BY GENCLASS 2.0.24
//
//==============================================================================

#ifndef _cimple_SF_ConnectorRealizesPort_h
#define _cimple_SF_ConnectorRealizesPort_h

#include <cimple/cimple.h>
#include "CIM_Realizes.h"

CIMPLE_NAMESPACE_BEGIN

// SF_ConnectorRealizesPort keys:

/*[1194]*/
class SF_ConnectorRealizesPort : public Instance
{
public:
    // CIM_Dependency features:
    CIM_PhysicalElement* Antecedent;
    CIM_LogicalDevice* Dependent;

    // CIM_Realizes features:

    // SF_ConnectorRealizesPort features:

    CIMPLE_CLASS(SF_ConnectorRealizesPort)
};

CIMPLE_NAMESPACE_END

#endif /* _cimple_SF_ConnectorRealizesPort_h */
