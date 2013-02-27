/*NOCHKSRC*/
//==============================================================================
//
// PLEASE DO NOT EDIT; THIS FILE WAS AUTOMATICALLY GENERATED BY GENCLASS 2.0.24
//
//==============================================================================

#ifndef _cimple_CIM_LANEndpoint_h
#define _cimple_CIM_LANEndpoint_h

#include <cimple/cimple.h>
#include "CIM_ProtocolEndpoint.h"

CIMPLE_NAMESPACE_BEGIN

// CIM_LANEndpoint keys:
//     Name
//     SystemCreationClassName
//     SystemName
//     CreationClassName

/*[1194]*/
class CIM_LANEndpoint : public Instance
{
public:
    // CIM_ManagedElement features:
    Property<String> InstanceID;
    Property<String> Caption;
    Property<String> Description;
    Property<String> ElementName;
    Property<uint64> Generation;

    // CIM_ManagedSystemElement features:
    Property<Datetime> InstallDate;
    Property<String> Name;
    struct _OperationalStatus
    {
        enum
        {
            enum_Unknown = 0,
            enum_Other = 1,
            enum_OK = 2,
            enum_Degraded = 3,
            enum_Stressed = 4,
            enum_Predictive_Failure = 5,
            enum_Error = 6,
            enum_Non_Recoverable_Error = 7,
            enum_Starting = 8,
            enum_Stopping = 9,
            enum_Stopped = 10,
            enum_In_Service = 11,
            enum_No_Contact = 12,
            enum_Lost_Communication = 13,
            enum_Aborted = 14,
            enum_Dormant = 15,
            enum_Supporting_Entity_in_Error = 16,
            enum_Completed = 17,
            enum_Power_Mode = 18,
            enum_DMTF_Reserved = 0,
            enum_Vendor_Reserved = 0,
        };
        Array_uint16 value;
        uint8 null;
    }
    OperationalStatus;
    Property<Array_String> StatusDescriptions;
    Property<String> Status;
    struct _HealthState
    {
        enum
        {
            enum_Unknown = 0,
            enum_OK = 5,
            enum_Degraded_Warning = 10,
            enum_Minor_failure = 15,
            enum_Major_failure = 20,
            enum_Critical_failure = 25,
            enum_Non_recoverable_error = 30,
            enum_DMTF_Reserved = 0,
        };
        uint16 value;
        uint8 null;
    }
    HealthState;
    struct _CommunicationStatus
    {
        enum
        {
            enum_Unknown = 0,
            enum_Not_Available = 1,
            enum_Communication_OK = 2,
            enum_Lost_Communication = 3,
            enum_No_Contact = 4,
            enum_DMTF_Reserved = 0,
            enum_Vendor_Reserved = 0,
        };
        uint16 value;
        uint8 null;
    }
    CommunicationStatus;
    struct _DetailedStatus
    {
        enum
        {
            enum_Not_Available = 0,
            enum_No_Additional_Information = 1,
            enum_Stressed = 2,
            enum_Predictive_Failure = 3,
            enum_Non_Recoverable_Error = 4,
            enum_Supporting_Entity_in_Error = 5,
            enum_DMTF_Reserved = 0,
            enum_Vendor_Reserved = 0,
        };
        uint16 value;
        uint8 null;
    }
    DetailedStatus;
    struct _OperatingStatus
    {
        enum
        {
            enum_Unknown = 0,
            enum_Not_Available = 1,
            enum_Servicing = 2,
            enum_Starting = 3,
            enum_Stopping = 4,
            enum_Stopped = 5,
            enum_Aborted = 6,
            enum_Dormant = 7,
            enum_Completed = 8,
            enum_Migrating = 9,
            enum_Emigrating = 10,
            enum_Immigrating = 11,
            enum_Snapshotting = 12,
            enum_Shutting_Down = 13,
            enum_In_Test = 14,
            enum_Transitioning = 15,
            enum_In_Service = 16,
            enum_DMTF_Reserved = 0,
            enum_Vendor_Reserved = 0,
        };
        uint16 value;
        uint8 null;
    }
    OperatingStatus;
    struct _PrimaryStatus
    {
        enum
        {
            enum_Unknown = 0,
            enum_OK = 1,
            enum_Degraded = 2,
            enum_Error = 3,
            enum_DMTF_Reserved = 0,
            enum_Vendor_Reserved = 0,
        };
        uint16 value;
        uint8 null;
    }
    PrimaryStatus;

    // CIM_LogicalElement features:

    // CIM_EnabledLogicalElement features:
    struct _EnabledState
    {
        enum
        {
            enum_Unknown = 0,
            enum_Other = 1,
            enum_Enabled = 2,
            enum_Disabled = 3,
            enum_Shutting_Down = 4,
            enum_Not_Applicable = 5,
            enum_Enabled_but_Offline = 6,
            enum_In_Test = 7,
            enum_Deferred = 8,
            enum_Quiesce = 9,
            enum_Starting = 10,
            enum_DMTF_Reserved = 11,
            enum_Vendor_Reserved = 32768,
        };
        uint16 value;
        uint8 null;
    }
    EnabledState;
    Property<String> OtherEnabledState;
    struct _RequestedState
    {
        enum
        {
            enum_Unknown = 0,
            enum_Enabled = 2,
            enum_Disabled = 3,
            enum_Shut_Down = 4,
            enum_No_Change = 5,
            enum_Offline = 6,
            enum_Test = 7,
            enum_Deferred = 8,
            enum_Quiesce = 9,
            enum_Reboot = 10,
            enum_Reset = 11,
            enum_Not_Applicable = 12,
            enum_DMTF_Reserved = 0,
            enum_Vendor_Reserved = 32768,
        };
        uint16 value;
        uint8 null;
    }
    RequestedState;
    struct _EnabledDefault
    {
        enum
        {
            enum_Enabled = 2,
            enum_Disabled = 3,
            enum_Not_Applicable = 5,
            enum_Enabled_but_Offline = 6,
            enum_No_Default = 7,
            enum_Quiesce = 9,
            enum_DMTF_Reserved = 0,
            enum_Vendor_Reserved = 32768,
        };
        uint16 value;
        uint8 null;
    }
    EnabledDefault;
    Property<Datetime> TimeOfLastStateChange;
    struct _AvailableRequestedStates
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
            enum_DMTF_Reserved = 0,
        };
        Array_uint16 value;
        uint8 null;
    }
    AvailableRequestedStates;
    struct _TransitioningToState
    {
        enum
        {
            enum_Unknown = 0,
            enum_Enabled = 2,
            enum_Disabled = 3,
            enum_Shut_Down = 4,
            enum_No_Change = 5,
            enum_Offline = 6,
            enum_Test = 7,
            enum_Defer = 8,
            enum_Quiesce = 9,
            enum_Reboot = 10,
            enum_Reset = 11,
            enum_Not_Applicable = 12,
            enum_DMTF_Reserved = 0,
        };
        uint16 value;
        uint8 null;
    }
    TransitioningToState;

    // CIM_ServiceAccessPoint features:
    Property<String> SystemCreationClassName;
    Property<String> SystemName;
    Property<String> CreationClassName;

    // CIM_ProtocolEndpoint features:
    Property<String> NameFormat;
    struct _ProtocolType
    {
        enum
        {
            enum_Unknown = 0,
            enum_Other = 1,
            enum_IPv4 = 2,
            enum_IPv6 = 3,
            enum_IPX = 4,
            enum_AppleTalk = 5,
            enum_DECnet = 6,
            enum_SNA = 7,
            enum_CONP = 8,
            enum_CLNP = 9,
            enum_VINES = 10,
            enum_XNS = 11,
            enum_ATM = 12,
            enum_Frame_Relay = 13,
            enum_Ethernet = 14,
            enum_TokenRing = 15,
            enum_FDDI = 16,
            enum_Infiniband = 17,
            enum_Fibre_Channel = 18,
            enum_ISDN_BRI_Endpoint = 19,
            enum_ISDN_B_Channel_Endpoint = 20,
            enum_ISDN_D_Channel_Endpoint = 21,
            enum_IPv4_v6 = 22,
            enum_BGP = 23,
            enum_OSPF = 24,
            enum_MPLS = 25,
            enum_UDP = 26,
            enum_TCP = 27,
        };
        uint16 value;
        uint8 null;
    }
    ProtocolType;
    struct _ProtocolIFType
    {
        enum
        {
            enum_Unknown = 0,
            enum_Other = 1,
            enum_Regular_1822 = 2,
            enum_HDH_1822 = 3,
            enum_DDN_X_25 = 4,
            enum_RFC877_X_25 = 5,
            enum_Ethernet_CSMA_CD = 6,
            enum_ISO_802_3_CSMA_CD = 7,
            enum_ISO_802_4_Token_Bus = 8,
            enum_ISO_802_5_Token_Ring = 9,
            enum_ISO_802_6_MAN = 10,
            enum_StarLAN = 11,
            enum_Proteon_10Mbit = 12,
            enum_Proteon_80Mbit = 13,
            enum_HyperChannel = 14,
            enum_FDDI = 15,
            enum_LAP_B = 16,
            enum_SDLC = 17,
            enum_DS1 = 18,
            enum_E1 = 19,
            enum_Basic_ISDN = 20,
            enum_Primary_ISDN = 21,
            enum_Proprietary_Point_to_Point_Serial = 22,
            enum_PPP = 23,
            enum_Software_Loopback = 24,
            enum_EON = 25,
            enum_Ethernet_3Mbit = 26,
            enum_NSIP = 27,
            enum_SLIP = 28,
            enum_Ultra = 29,
            enum_DS3 = 30,
            enum_SIP = 31,
            enum_Frame_Relay = 32,
            enum_RS_232 = 33,
            enum_Parallel = 34,
            enum_ARCNet = 35,
            enum_ARCNet_Plus = 36,
            enum_ATM = 37,
            enum_MIO_X_25 = 38,
            enum_SONET = 39,
            enum_X_25_PLE = 40,
            enum_ISO_802_211c = 41,
            enum_LocalTalk = 42,
            enum_SMDS_DXI = 43,
            enum_Frame_Relay_Service = 44,
            enum_V_35 = 45,
            enum_HSSI = 46,
            enum_HIPPI = 47,
            enum_Modem = 48,
            enum_AAL5 = 49,
            enum_SONET_Path = 50,
            enum_SONET_VT = 51,
            enum_SMDS_ICIP = 52,
            enum_Proprietary_Virtual_Internal = 53,
            enum_Proprietary_Multiplexor = 54,
            enum_IEEE_802_12 = 55,
            enum_Fibre_Channel = 56,
            enum_HIPPI_Interface = 57,
            enum_Frame_Relay_Interconnect = 58,
            enum_ATM_Emulated_LAN_for_802_3 = 59,
            enum_ATM_Emulated_LAN_for_802_5 = 60,
            enum_ATM_Emulated_Circuit = 61,
            enum_Fast_Ethernet__100BaseT_ = 62,
            enum_ISDN = 63,
            enum_V_11 = 64,
            enum_V_36 = 65,
            enum_G703_at_64K = 66,
            enum_G703_at_2Mb = 67,
            enum_QLLC = 68,
            enum_Fast_Ethernet_100BaseFX = 69,
            enum_Channel = 70,
            enum_IEEE_802_11 = 71,
            enum_IBM_260_370_OEMI_Channel = 72,
            enum_ESCON = 73,
            enum_Data_Link_Switching = 74,
            enum_ISDN_S_T_Interface = 75,
            enum_ISDN_U_Interface = 76,
            enum_LAP_D = 77,
            enum_IP_Switch = 78,
            enum_Remote_Source_Route_Bridging = 79,
            enum_ATM_Logical = 80,
            enum_DS0 = 81,
            enum_DS0_Bundle = 82,
            enum_BSC = 83,
            enum_Async = 84,
            enum_Combat_Net_Radio = 85,
            enum_ISO_802_5r_DTR = 86,
            enum_Ext_Pos_Loc_Report_System = 87,
            enum_AppleTalk_Remote_Access_Protocol = 88,
            enum_Proprietary_Connectionless = 89,
            enum_ITU_X_29_Host_PAD = 90,
            enum_ITU_X_3_Terminal_PAD = 91,
            enum_Frame_Relay_MPI = 92,
            enum_ITU_X_213 = 93,
            enum_ADSL = 94,
            enum_RADSL = 95,
            enum_SDSL = 96,
            enum_VDSL = 97,
            enum_ISO_802_5_CRFP = 98,
            enum_Myrinet = 99,
            enum_Voice_Receive_and_Transmit = 100,
            enum_Voice_Foreign_Exchange_Office = 101,
            enum_Voice_Foreign_Exchange_Service = 102,
            enum_Voice_Encapsulation = 103,
            enum_Voice_over_IP = 104,
            enum_ATM_DXI = 105,
            enum_ATM_FUNI = 106,
            enum_ATM_IMA = 107,
            enum_PPP_Multilink_Bundle = 108,
            enum_IP_over_CDLC = 109,
            enum_IP_over_CLAW = 110,
            enum_Stack_to_Stack = 111,
            enum_Virtual_IP_Address = 112,
            enum_MPC = 113,
            enum_IP_over_ATM = 114,
            enum_ISO_802_5j_Fibre_Token_Ring = 115,
            enum_TDLC = 116,
            enum_Gigabit_Ethernet = 117,
            enum_HDLC = 118,
            enum_LAP_F = 119,
            enum_V_37 = 120,
            enum_X_25_MLP = 121,
            enum_X_25_Hunt_Group = 122,
            enum_Transp_HDLC = 123,
            enum_Interleave_Channel = 124,
            enum_FAST_Channel = 125,
            enum_IP__for_APPN_HPR_in_IP_Networks_ = 126,
            enum_CATV_MAC_Layer = 127,
            enum_CATV_Downstream = 128,
            enum_CATV_Upstream = 129,
            enum_Avalon_12MPP_Switch = 130,
            enum_Tunnel = 131,
            enum_Coffee = 132,
            enum_Circuit_Emulation_Service = 133,
            enum_ATM_SubInterface = 134,
            enum_Layer_2_VLAN_using_802_1Q = 135,
            enum_Layer_3_VLAN_using_IP = 136,
            enum_Layer_3_VLAN_using_IPX = 137,
            enum_Digital_Power_Line = 138,
            enum_Multimedia_Mail_over_IP = 139,
            enum_DTM = 140,
            enum_DCN = 141,
            enum_IP_Forwarding = 142,
            enum_MSDSL = 143,
            enum_IEEE_1394 = 144,
            enum_IF_GSN_HIPPI_6400 = 145,
            enum_DVB_RCC_MAC_Layer = 146,
            enum_DVB_RCC_Downstream = 147,
            enum_DVB_RCC_Upstream = 148,
            enum_ATM_Virtual = 149,
            enum_MPLS_Tunnel = 150,
            enum_SRP = 151,
            enum_Voice_over_ATM = 152,
            enum_Voice_over_Frame_Relay = 153,
            enum_ISDL = 154,
            enum_Composite_Link = 155,
            enum_SS7_Signaling_Link = 156,
            enum_Proprietary_P2P_Wireless = 157,
            enum_Frame_Forward = 158,
            enum_RFC1483_Multiprotocol_over_ATM = 159,
            enum_USB = 160,
            enum_IEEE_802_3ad_Link_Aggregate = 161,
            enum_BGP_Policy_Accounting = 162,
            enum_FRF__16_Multilink_FR = 163,
            enum_H_323_Gatekeeper = 164,
            enum_H_323_Proxy = 165,
            enum_MPLS = 166,
            enum_Multi_Frequency_Signaling_Link = 167,
            enum_HDSL_2 = 168,
            enum_S_HDSL = 169,
            enum_DS1_Facility_Data_Link = 170,
            enum_Packet_over_SONET_SDH = 171,
            enum_DVB_ASI_Input = 172,
            enum_DVB_ASI_Output = 173,
            enum_Power_Line = 174,
            enum_Non_Facility_Associated_Signaling = 175,
            enum_TR008 = 176,
            enum_GR303_RDT = 177,
            enum_GR303_IDT = 178,
            enum_ISUP = 179,
            enum_Proprietary_Wireless_MAC_Layer = 180,
            enum_Proprietary_Wireless_Downstream = 181,
            enum_Proprietary_Wireless_Upstream = 182,
            enum_HIPERLAN_Type_2 = 183,
            enum_Proprietary_Broadband_Wireless_Access_Point_to_Mulipoint = 184,
            enum_SONET_Overhead_Channel = 185,
            enum_Digital_Wrapper_Overhead_Channel = 186,
            enum_ATM_Adaptation_Layer_2 = 187,
            enum_Radio_MAC = 188,
            enum_ATM_Radio = 189,
            enum_Inter_Machine_Trunk = 190,
            enum_MVL_DSL = 191,
            enum_Long_Read_DSL = 192,
            enum_Frame_Relay_DLCI_Endpoint = 193,
            enum_ATM_VCI_Endpoint = 194,
            enum_Optical_Channel = 195,
            enum_Optical_Transport = 196,
            enum_Proprietary_ATM = 197,
            enum_Voice_over_Cable = 198,
            enum_Infiniband = 199,
            enum_TE_Link = 200,
            enum_Q_2931 = 201,
            enum_Virtual_Trunk_Group = 202,
            enum_SIP_Trunk_Group = 203,
            enum_SIP_Signaling = 204,
            enum_CATV_Upstream_Channel = 205,
            enum_Econet = 206,
            enum_FSAN_155Mb_PON = 207,
            enum_FSAN_622Mb_PON = 208,
            enum_Transparent_Bridge = 209,
            enum_Line_Group = 210,
            enum_Voice_E_M_Feature_Group = 211,
            enum_Voice_FGD_EANA = 212,
            enum_Voice_DID = 213,
            enum_MPEG_Transport = 214,
            enum_6To4 = 215,
            enum_GTP = 216,
            enum_Paradyne_EtherLoop_1 = 217,
            enum_Paradyne_EtherLoop_2 = 218,
            enum_Optical_Channel_Group = 219,
            enum_HomePNA = 220,
            enum_GFP = 221,
            enum_ciscoISLvlan = 222,
            enum_actelisMetaLOOP = 223,
            enum_Fcip = 224,
            enum_IANA_Reserved = 225,
            enum_IPv4 = 4096,
            enum_IPv6 = 4097,
            enum_IPv4_v6 = 4098,
            enum_IPX = 4099,
            enum_DECnet = 4100,
            enum_SNA = 4101,
            enum_CONP = 4102,
            enum_CLNP = 4103,
            enum_VINES = 4104,
            enum_XNS = 4105,
            enum_ISDN_B_Channel_Endpoint = 4106,
            enum_ISDN_D_Channel_Endpoint = 4107,
            enum_BGP = 4108,
            enum_OSPF = 4109,
            enum_UDP = 4110,
            enum_TCP = 4111,
            enum_802_11a = 4112,
            enum_802_11b = 4113,
            enum_802_11g = 4114,
            enum_802_11h = 4115,
            enum_NFS = 4200,
            enum_CIFS = 4201,
            enum_DAFS = 4202,
            enum_WebDAV = 4203,
            enum_HTTP = 4204,
            enum_FTP = 4205,
            enum_NDMP = 4300,
            enum_Telnet = 4400,
            enum_SSH = 4401,
            enum_SM_CLP = 4402,
            enum_SMTP = 4403,
            enum_LDAP = 4404,
            enum_RDP = 4405,
            enum_HTTPS = 4406,
            enum_DMTF_Reserved = 0,
            enum_Vendor_Reserved = 32768,
        };
        uint16 value;
        uint8 null;
    }
    ProtocolIFType;
    Property<String> OtherTypeDescription;
    Property<boolean> BroadcastResetSupported;

    // CIM_LANEndpoint features:
    Property<String> LANID;
    struct _LANType
    {
        enum
        {
            enum_Unknown = 0,
            enum_Other = 1,
            enum_Ethernet = 2,
            enum_TokenRing = 3,
            enum_FDDI = 4,
        };
        uint16 value;
        uint8 null;
    }
    LANType;
    Property<String> OtherLANType;
    Property<String> MACAddress;
    Property<Array_String> AliasAddresses;
    Property<Array_String> GroupAddresses;
    Property<uint32> MaxDataSize;

    CIMPLE_CLASS(CIM_LANEndpoint)
};

typedef CIM_EnabledLogicalElement_RequestStateChange_method CIM_LANEndpoint_RequestStateChange_method;

typedef CIM_ProtocolEndpoint_BroadcastReset_method CIM_LANEndpoint_BroadcastReset_method;

CIMPLE_NAMESPACE_END

#endif /* _cimple_CIM_LANEndpoint_h */
