/*NOCHKSRC*/
//==============================================================================
//
// PLEASE DO NOT EDIT; THIS FILE WAS AUTOMATICALLY GENERATED BY GENCLASS 2.0.24
//
//==============================================================================

#ifndef _cimple_CIM_ElementSoftwareIdentity_h
#define _cimple_CIM_ElementSoftwareIdentity_h

#include <cimple/cimple.h>
#include "CIM_Dependency.h"
#include "CIM_SoftwareIdentity.h"
#include "CIM_ManagedElement.h"

CIMPLE_NAMESPACE_BEGIN

// CIM_ElementSoftwareIdentity keys:

/*[1194]*/
class CIM_ElementSoftwareIdentity : public Instance
{
public:
    // CIM_Dependency features:
    CIM_SoftwareIdentity* Antecedent;
    CIM_ManagedElement* Dependent;

    // CIM_ElementSoftwareIdentity features:
    struct _UpgradeCondition
    {
        enum
        {
            enum_Unknown = 0,
            enum_Other = 1,
            enum_Resides_off_device = 2,
            enum_Owner_Upgradeable = 3,
            enum_Factory_Upgradeable = 4,
            enum_Not_Upgradeable = 5,
            enum_DMTF_Reserved = 0,
            enum_Vendor_Reserved = 0,
        };
        uint16 value;
        uint8 null;
    }
    UpgradeCondition;
    Property<String> OtherUpgradeCondition;
    struct _ElementSoftwareStatus
    {
        enum
        {
            enum_Unknown = 0,
            enum_Current = 2,
            enum_Next = 3,
            enum_FallBack = 4,
            enum_Default = 5,
            enum_Installed = 6,
            enum_Single_Use = 7,
            enum_Available = 8,
            enum_Supports = 9,
            enum_DMTF_Reserved = 0,
            enum_Vendor_Reserved = 32768,
        };
        Array_uint16 value;
        uint8 null;
    }
    ElementSoftwareStatus;

    CIMPLE_CLASS(CIM_ElementSoftwareIdentity)
};

CIMPLE_NAMESPACE_END

#endif /* _cimple_CIM_ElementSoftwareIdentity_h */