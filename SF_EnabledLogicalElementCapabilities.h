/*NOCHKSRC*/
//==============================================================================
//
// PLEASE DO NOT EDIT; THIS FILE WAS AUTOMATICALLY GENERATED BY GENCLASS 2.0.24
//
//==============================================================================

#ifndef _cimple_SF_EnabledLogicalElementCapabilities_h
#define _cimple_SF_EnabledLogicalElementCapabilities_h

#include <cimple/cimple.h>
#include "CIM_EnabledLogicalElementCapabilities.h"

CIMPLE_NAMESPACE_BEGIN

// SF_EnabledLogicalElementCapabilities keys:
//     InstanceID

/*[1194]*/
class SF_EnabledLogicalElementCapabilities : public Instance
{
public:
    // CIM_ManagedElement features:
    Property<String> InstanceID;
    Property<String> Caption;
    Property<String> Description;
    Property<String> ElementName;

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

    // SF_EnabledLogicalElementCapabilities features:

    CIMPLE_CLASS(SF_EnabledLogicalElementCapabilities)
};

CIMPLE_NAMESPACE_END

#endif /* _cimple_SF_EnabledLogicalElementCapabilities_h */