/*NOCHKSRC*/
//==============================================================================
//
// PLEASE DO NOT EDIT; THIS FILE WAS AUTOMATICALLY GENERATED BY GENCLASS 2.0.24
//
//==============================================================================

#ifndef _cimple_CIM_ServiceAffectsElement_h
#define _cimple_CIM_ServiceAffectsElement_h

#include <cimple/cimple.h>
#include "CIM_ManagedElement.h"
#include "CIM_Service.h"

CIMPLE_NAMESPACE_BEGIN

// CIM_ServiceAffectsElement keys:

/*[1194]*/
class CIM_ServiceAffectsElement : public Instance
{
public:
    // CIM_ServiceAffectsElement features:
    CIM_ManagedElement* AffectedElement;
    CIM_Service* AffectingElement;
    struct _ElementEffects
    {
        enum
        {
            enum_Unknown = 0,
            enum_Other = 1,
            enum_Exclusive_Use = 2,
            enum_Performance_Impact = 3,
            enum_Element_Integrity = 4,
            enum_Manages = 5,
            enum_Consumes = 6,
            enum_Enhances_Integrity = 7,
            enum_Degrades_Integrity = 8,
            enum_Enhances_Performance = 9,
            enum_Degrades_Performance = 10,
            enum_DMTF_Reserved = 0,
            enum_Vendor_Reserved = 0,
        };
        Array_uint16 value;
        uint8 null;
    }
    ElementEffects;
    Property<Array_String> OtherElementEffectsDescriptions;
    Property<uint16> AssignedSequence;

    CIMPLE_CLASS(CIM_ServiceAffectsElement)
};

CIMPLE_NAMESPACE_END

#endif /* _cimple_CIM_ServiceAffectsElement_h */
