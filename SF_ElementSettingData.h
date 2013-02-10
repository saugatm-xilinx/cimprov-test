/*NOCHKSRC*/
//==============================================================================
//
// PLEASE DO NOT EDIT; THIS FILE WAS AUTOMATICALLY GENERATED BY GENCLASS 2.0.24
//
//==============================================================================

#ifndef _cimple_SF_ElementSettingData_h
#define _cimple_SF_ElementSettingData_h

#include <cimple/cimple.h>
#include "CIM_ElementSettingData.h"

CIMPLE_NAMESPACE_BEGIN

// SF_ElementSettingData keys:

/*[1194]*/
class SF_ElementSettingData : public Instance
{
public:
    // CIM_ElementSettingData features:
    CIM_ManagedElement* ManagedElement;
    CIM_SettingData* SettingData;
    struct _IsDefault
    {
        enum
        {
            enum_Unknown = 0,
            enum_Is_Default = 1,
            enum_Is_Not_Default = 2,
        };
        uint16 value;
        uint8 null;
    }
    IsDefault;
    struct _IsCurrent
    {
        enum
        {
            enum_Unknown = 0,
            enum_Is_Current = 1,
            enum_Is_Not_Current = 2,
        };
        uint16 value;
        uint8 null;
    }
    IsCurrent;
    struct _IsNext
    {
        enum
        {
            enum_Unknown = 0,
            enum_Is_Next = 1,
            enum_Is_Not_Next = 2,
            enum_Is_Next_For_Single_Use = 3,
        };
        uint16 value;
        uint8 null;
    }
    IsNext;

    // SF_ElementSettingData features:

    CIMPLE_CLASS(SF_ElementSettingData)
};

CIMPLE_NAMESPACE_END

#endif /* _cimple_SF_ElementSettingData_h */
