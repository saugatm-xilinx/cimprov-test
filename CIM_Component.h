/*NOCHKSRC*/
//==============================================================================
//
// PLEASE DO NOT EDIT; THIS FILE WAS AUTOMATICALLY GENERATED BY GENCLASS 2.0.24
//
//==============================================================================

#ifndef _cimple_CIM_Component_h
#define _cimple_CIM_Component_h

#include <cimple/cimple.h>
#include "CIM_ManagedElement.h"
#include "CIM_ManagedElement.h"

CIMPLE_NAMESPACE_BEGIN

// CIM_Component keys:

/*[1194]*/
class CIM_Component : public Instance
{
public:
    // CIM_Component features:
    CIM_ManagedElement* GroupComponent;
    CIM_ManagedElement* PartComponent;

    CIMPLE_CLASS(CIM_Component)
};

CIMPLE_NAMESPACE_END

#endif /* _cimple_CIM_Component_h */
