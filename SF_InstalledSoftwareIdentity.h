/*NOCHKSRC*/
//==============================================================================
//
// PLEASE DO NOT EDIT; THIS FILE WAS AUTOMATICALLY GENERATED BY GENCLASS 2.0.24
//
//==============================================================================

#ifndef _cimple_SF_InstalledSoftwareIdentity_h
#define _cimple_SF_InstalledSoftwareIdentity_h

#include <cimple/cimple.h>
#include "CIM_InstalledSoftwareIdentity.h"

CIMPLE_NAMESPACE_BEGIN

// SF_InstalledSoftwareIdentity keys:

/*[1194]*/
class SF_InstalledSoftwareIdentity : public Instance
{
public:
    // CIM_InstalledSoftwareIdentity features:
    CIM_System* System;
    CIM_SoftwareIdentity* InstalledSoftware;

    // SF_InstalledSoftwareIdentity features:

    CIMPLE_CLASS(SF_InstalledSoftwareIdentity)
};

CIMPLE_NAMESPACE_END

#endif /* _cimple_SF_InstalledSoftwareIdentity_h */
