/*NOCHKSRC*/
//==============================================================================
//
// PLEASE DO NOT EDIT; THIS FILE WAS AUTOMATICALLY GENERATED BY GENCLASS 2.0.24
//
//==============================================================================

#ifndef _cimple_CIM_ReferencedProfile_h
#define _cimple_CIM_ReferencedProfile_h

#include <cimple/cimple.h>
#include "CIM_ReferencedSpecification.h"
#include "CIM_RegisteredProfile.h"
#include "CIM_RegisteredProfile.h"

CIMPLE_NAMESPACE_BEGIN

// CIM_ReferencedProfile keys:

/*[1194]*/
class CIM_ReferencedProfile : public Instance
{
public:
    // CIM_Dependency features:
    CIM_RegisteredProfile* Antecedent;
    CIM_RegisteredProfile* Dependent;

    // CIM_ReferencedSpecification features:

    // CIM_ReferencedProfile features:

    CIMPLE_CLASS(CIM_ReferencedProfile)
};

CIMPLE_NAMESPACE_END

#endif /* _cimple_CIM_ReferencedProfile_h */
