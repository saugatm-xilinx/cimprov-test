/*NOCHKSRC*/
//==============================================================================
//
// PLEASE DO NOT EDIT; THIS FILE WAS AUTOMATICALLY GENERATED BY GENCLASS 2.0.24
//
//==============================================================================

#ifndef _cimple_SF_ReferencedProfile_h
#define _cimple_SF_ReferencedProfile_h

#include <cimple/cimple.h>
#include "CIM_ReferencedProfile.h"

CIMPLE_NAMESPACE_BEGIN

// SF_ReferencedProfile keys:

/*[1194]*/
class SF_ReferencedProfile : public Instance
{
public:
    // CIM_Dependency features:
    CIM_RegisteredProfile* Antecedent;
    CIM_RegisteredProfile* Dependent;

    // CIM_ReferencedProfile features:

    // SF_ReferencedProfile features:

    CIMPLE_CLASS(SF_ReferencedProfile)
};

CIMPLE_NAMESPACE_END

#endif /* _cimple_SF_ReferencedProfile_h */
