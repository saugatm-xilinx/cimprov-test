/*NOCHKSRC*/
//==============================================================================
//
// PLEASE DO NOT EDIT; THIS FILE WAS AUTOMATICALLY GENERATED BY GENCLASS 2.0.24
//
//==============================================================================

#ifndef _cimple_SF_OwningJobElement_h
#define _cimple_SF_OwningJobElement_h

#include <cimple/cimple.h>
#include "CIM_OwningJobElement.h"

CIMPLE_NAMESPACE_BEGIN

// SF_OwningJobElement keys:

/*[1194]*/
class SF_OwningJobElement : public Instance
{
public:
    // CIM_OwningJobElement features:
    CIM_ManagedElement* OwningElement;
    CIM_Job* OwnedElement;

    // SF_OwningJobElement features:

    CIMPLE_CLASS(SF_OwningJobElement)
};

CIMPLE_NAMESPACE_END

#endif /* _cimple_SF_OwningJobElement_h */
