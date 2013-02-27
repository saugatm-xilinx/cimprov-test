/*NOCHKSRC*/
//==============================================================================
//
// PLEASE DO NOT EDIT; THIS FILE WAS AUTOMATICALLY GENERATED BY GENCLASS 2.0.24
//
//==============================================================================

#ifndef _cimple_CIM_EnabledLogicalElementCapabilities_h
#define _cimple_CIM_EnabledLogicalElementCapabilities_h

#include <cimple/cimple.h>
#include "CIM_Capabilities.h"

CIMPLE_NAMESPACE_BEGIN

// CIM_EnabledLogicalElementCapabilities keys:
//     InstanceID

/*[1194]*/
class CIM_EnabledLogicalElementCapabilities : public Instance
{
public:
    // CIM_ManagedElement features:
    Property<String> InstanceID;
    Property<String> Caption;
    Property<String> Description;
    Property<String> ElementName;
    Property<uint64> Generation;

    // CIM_Capabilities features:

    // CIM_EnabledLogicalElementCapabilities features:
    Property<boolean> ElementNameEditSupported;
    Property<uint16> MaxElementNameLen;
    struct _RequestedStatesSupported
    {
        enum
        {
            enum_Enabled = 2,
            enum_Disabled = 3,
            enum_Shut_Down = 4,
            enum_Offline = 6,
            enum_Test = 7,
            enum_Defer = 8,
            enum_Quiesce = 9,
            enum_Reboot = 10,
            enum_Reset = 11,
        };
        Array_uint16 value;
        uint8 null;
    }
    RequestedStatesSupported;
    Property<String> ElementNameMask;
    struct _StateAwareness
    {
        enum
        {
            enum_Implicit = 2,
            enum_RequestStateChange = 3,
            enum_DMTF_Reserved = 0,
        };
        Array_uint16 value;
        uint8 null;
    }
    StateAwareness;

    CIMPLE_CLASS(CIM_EnabledLogicalElementCapabilities)
};

typedef CIM_Capabilities_CreateGoalSettings_method CIM_EnabledLogicalElementCapabilities_CreateGoalSettings_method;

CIMPLE_NAMESPACE_END

#endif /* _cimple_CIM_EnabledLogicalElementCapabilities_h */
