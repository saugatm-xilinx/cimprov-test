/*NOCHKSRC*/
//==============================================================================
//
// PLEASE DO NOT EDIT; THIS FILE WAS AUTOMATICALLY GENERATED BY GENCLASS 2.0.24
//
//==============================================================================

#ifndef _cimple_SF_HostedAccessPoint_h
#define _cimple_SF_HostedAccessPoint_h

#include <cimple/cimple.h>
#include "CIM_HostedAccessPoint.h"

CIMPLE_NAMESPACE_BEGIN

// SF_HostedAccessPoint keys:

/*[1194]*/
class SF_HostedAccessPoint : public Instance
{
public:
    // CIM_Dependency features:
    CIM_System* Antecedent;
    CIM_ServiceAccessPoint* Dependent;

    // CIM_HostedDependency features:

    // CIM_HostedAccessPoint features:

    // SF_HostedAccessPoint features:

    CIMPLE_CLASS(SF_HostedAccessPoint)
};

CIMPLE_NAMESPACE_END

#endif /* _cimple_SF_HostedAccessPoint_h */