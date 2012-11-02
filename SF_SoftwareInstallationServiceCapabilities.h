/*NOCHKSRC*/
//==============================================================================
//
// PLEASE DO NOT EDIT; THIS FILE WAS AUTOMATICALLY GENERATED BY GENCLASS 2.0.24
//
//==============================================================================

#ifndef _cimple_SF_SoftwareInstallationServiceCapabilities_h
#define _cimple_SF_SoftwareInstallationServiceCapabilities_h

#include <cimple/cimple.h>
#include "CIM_SoftwareInstallationServiceCapabilities.h"

CIMPLE_NAMESPACE_BEGIN

// SF_SoftwareInstallationServiceCapabilities keys:
//     InstanceID

/*[1194]*/
class SF_SoftwareInstallationServiceCapabilities : public Instance
{
public:
    // CIM_ManagedElement features:
    Property<String> InstanceID;
    Property<String> Caption;
    Property<String> Description;
    Property<String> ElementName;

    // CIM_Capabilities features:

    // CIM_SoftwareInstallationServiceCapabilities features:
    struct _SupportedAsynchronousActions
    {
        enum
        {
            enum_None_supported = 2,
            enum_Install_From_Software_Identity = 3,
            enum_Install_from_ByteStream = 4,
            enum_Install_from_URI = 5,
        };
        Array_uint16 value;
        uint8 null;
    }
    SupportedAsynchronousActions;
    struct _SupportedSynchronousActions
    {
        enum
        {
            enum_None_supported = 2,
            enum_Install_From_Software_Identity = 3,
            enum_Install_from_ByteStream = 4,
            enum_Install_from_URI = 5,
        };
        Array_uint16 value;
        uint8 null;
    }
    SupportedSynchronousActions;
    Property<Array_String> SupportedTargetTypes;
    struct _SupportedExtendedResourceTypes
    {
        enum
        {
            enum_Unknown = 0,
            enum_Other = 1,
            enum_Not_Applicable = 2,
            enum_Linux_RPM = 3,
            enum_HP_UX_Depot = 4,
            enum_Windows_MSI = 5,
            enum_Solaris_Package = 6,
            enum_Macintosh_Disk_Image = 7,
            enum_Debian_linux_Package = 8,
            enum_VMware_vSphere_Installation_Bundle = 9,
            enum_VMware_Software_Bulletin = 10,
            enum_HP_Smart_Component = 11,
            enum_DMTF_Reserved = 0,
            enum_Vendor_Reserved = 0,
        };
        Array_uint16 value;
        uint8 null;
    }
    SupportedExtendedResourceTypes;
    Property<boolean> CanAddToCollection;
    struct _SupportedInstallOptions
    {
        enum
        {
            enum_Defer_target_system_reset = 2,
            enum_Force_installation = 3,
            enum_Install = 4,
            enum_Update = 5,
            enum_Repair = 6,
            enum_Reboot = 7,
            enum_Password = 8,
            enum_Uninstall = 9,
            enum_Log = 10,
            enum_SilentMode = 11,
            enum_AdministrativeMode = 12,
            enum_ScheduleInstallAt = 13,
            enum_DMTF_Reserved = 0,
            enum_Vendor_Specific = 32768,
        };
        Array_uint16 value;
        uint8 null;
    }
    SupportedInstallOptions;
    Property<Array_String> OtherSupportedExtendedResourceTypeDescriptions;
    Property<Array_uint16> SupportedExtendedResourceTypesMajorVersions;
    Property<Array_uint16> SupportedExtendedResourceTypesMinorVersions;
    Property<Array_uint16> SupportedExtendedResourceTypesRevisionNumbers;
    Property<Array_uint16> SupportedExtendedResourceTypesBuildNumbers;
    struct _SupportedURISchemes
    {
        enum
        {
            enum_data = 2,
            enum_file = 3,
            enum_ftp = 4,
            enum_http = 5,
            enum_https = 6,
            enum_nfs = 7,
            enum_tftp = 8,
            enum_DMTF_Reserved = 0,
            enum_Vendor_Specific = 0,
        };
        Array_uint16 value;
        uint8 null;
    }
    SupportedURISchemes;

    // SF_SoftwareInstallationServiceCapabilities features:

    CIMPLE_CLASS(SF_SoftwareInstallationServiceCapabilities)
};

CIMPLE_NAMESPACE_END

#endif /* _cimple_SF_SoftwareInstallationServiceCapabilities_h */