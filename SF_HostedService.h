/*NOCHKSRC*/
//==============================================================================
//
// PLEASE DO NOT EDIT; THIS FILE WAS AUTOMATICALLY GENERATED BY GENCLASS 2.0.24
//
//==============================================================================

#ifndef _cimple_SF_HostedService_h
#define _cimple_SF_HostedService_h

#include <cimple/cimple.h>
#include "CIM_HostedService.h"

CIMPLE_NAMESPACE_BEGIN

// SF_HostedService keys:

/*[1194]*/
class SF_HostedService : public Instance
{
public:
    // CIM_Dependency features:
    CIM_System* Antecedent;
    CIM_Service* Dependent;

    // CIM_HostedDependency features:

    // CIM_HostedService features:

    // SF_HostedService features:

    CIMPLE_CLASS(SF_HostedService)
};

CIMPLE_NAMESPACE_END

#endif /* _cimple_SF_HostedService_h */