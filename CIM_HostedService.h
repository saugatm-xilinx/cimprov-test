/*NOCHKSRC*/
//==============================================================================
//
// PLEASE DO NOT EDIT; THIS FILE WAS AUTOMATICALLY GENERATED BY GENCLASS 2.0.24
//
//==============================================================================

#ifndef _cimple_CIM_HostedService_h
#define _cimple_CIM_HostedService_h

#include <cimple/cimple.h>
#include "CIM_HostedDependency.h"
#include "CIM_System.h"
#include "CIM_Service.h"

CIMPLE_NAMESPACE_BEGIN

// CIM_HostedService keys:

/*[1194]*/
class CIM_HostedService : public Instance
{
public:
    // CIM_Dependency features:
    CIM_System* Antecedent;
    CIM_Service* Dependent;

    // CIM_HostedDependency features:

    // CIM_HostedService features:

    CIMPLE_CLASS(CIM_HostedService)
};

CIMPLE_NAMESPACE_END

#endif /* _cimple_CIM_HostedService_h */