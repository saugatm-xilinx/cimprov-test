/*NOCHKSRC*/
//==============================================================================
//
// PLEASE DO NOT EDIT; THIS FILE WAS AUTOMATICALLY GENERATED BY GENCLASS 2.0.24
//
//==============================================================================

#ifndef _cimple_CIM_HostedDependency_h
#define _cimple_CIM_HostedDependency_h

#include <cimple/cimple.h>
#include "CIM_Dependency.h"
#include "CIM_ManagedElement.h"
#include "CIM_ManagedElement.h"

CIMPLE_NAMESPACE_BEGIN

// CIM_HostedDependency keys:

/*[1194]*/
class CIM_HostedDependency : public Instance
{
public:
    // CIM_Dependency features:
    CIM_ManagedElement* Antecedent;
    CIM_ManagedElement* Dependent;

    // CIM_HostedDependency features:

    CIMPLE_CLASS(CIM_HostedDependency)
};

CIMPLE_NAMESPACE_END

#endif /* _cimple_CIM_HostedDependency_h */
