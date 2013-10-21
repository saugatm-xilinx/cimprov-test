/**************************************************************************\
*//*! \file isns.h   Definitions for iSNS
** <L5_PRIVATE L5_SOURCE>
** \author  mjs
**  \brief  Package - lib/iscsi       iSCSI library
**   \date  2006/09/12
**    \cop  (c) Level 5 Networks Limited.
** </L5_PRIVATE>
*//*
\**************************************************************************/

/*! \cidoxg_include_ci_iscsi */

#ifndef __CI_ISCSI_ISNS_H__
#define __CI_ISCSI_ISNS_H__


// #include <ci/tools/dllist.h>


/* ============================================================================
 *  iSNS protocol constants
 * ============================================================================
 */

/* iSNS functions (RFC4171 4.1.3).  (REQ indicates "required for client"
 * according to the RFC, although it's not clear that we need to do anything
 * with Discovery Domains for our purposes.)
 */
#define ISNS_FUNC_DevAttrReg    0x0001 /*REQ*/ /* Device Attr Reg Request   */
#define ISNS_FUNC_DevAttrQry    0x0002 /*REQ*/ /* Dev Attr Query Request    */
#define ISNS_FUNC_DevGetNext    0x0003         /* Dev Get Next Request      */
#define ISNS_FUNC_DevDereg      0x0004 /*REQ*/ /* Deregister Dev Request    */
#define ISNS_FUNC_SCNReg        0x0005         /* SCN Register Request      */
#define ISNS_FUNC_SCNDereg      0x0006         /* SCN Deregister Request    */
#define ISNS_FUNC_SCNEvent      0x0007         /* SCN Event                 */
#define ISNS_FUNC_SCN           0x0008         /* State Change Notification */
#define ISNS_FUNC_DDReg         0x0009 /*REQ*/ /* DD Register               */
#define ISNS_FUNC_DDDereg       0x000A /*REQ*/ /* DD Deregister             */
#define ISNS_FUNC_DDSReg        0x000B /*REQ*/ /* DDS Register              */
#define ISNS_FUNC_DDSDereg      0x000C /*REQ*/ /* DDS Deregister            */
#define ISNS_FUNC_ESI           0x000D         /* Entity Status Inquiry     */
#define ISNS_FUNC_Heartbeat     0x000E         /* Name Service Heartbeat    */
#define ISNS_FUNC_DevAttrRegRsp 0x8001 /*REQ*/ /* Device Attr Register Rsp  */
#define ISNS_FUNC_DevAttrQryRsp 0x8002 /*REQ*/ /* Device Attr Query Rsp     */
#define ISNS_FUNC_DevGetNextRsp 0x8003         /* Device Get Next Rsp       */
#define ISNS_FUNC_DevDeregRsp   0x8004 /*REQ*/ /* Device Dereg Rsp          */
#define ISNS_FUNC_SCNRegRsp     0x8005         /* SCN Register Rsp          */
#define ISNS_FUNC_SCNDeregRsp   0x8006         /* SCN Deregister Rsp        */
#define ISNS_FUNC_SCNEventRsp   0x8007         /* SCN Event Rsp             */
#define ISNS_FUNC_SCNRsp        0x8008         /* SCN Response              */
#define ISNS_FUNC_DDRegRsp      0x8009 /*REQ*/ /* DD Register Rsp           */
#define ISNS_FUNC_DDDeregRsp    0x800A /*REQ*/ /* DD Deregister Rsp         */
#define ISNS_FUNC_DDSRegRsp     0x800B /*REQ*/ /* DDS Register Rsp          */
#define ISNS_FUNC_DDSDeregRsp   0x800C /*REQ*/ /* DDS Deregister Rsp        */
#define ISNS_FUNC_ESIRsp        0x800D         /* Entity Stat Inquiry Rsp   */


/* iSNSP version (RFC4171 5.1.1)
 */
#define ISNS_PROTOCOL_VERSION 0x0001


/* iSNSP flags (RFC4171 5.1.4) - beware backwards bit numbering in the RFC!
 */
#define ISNS_FLAG_SenderIsClient   0x8000
#define ISNS_FLAG_SenderIsServer   0x4000
#define ISNS_FLAG_AuthBlockPresent 0x2000
#define ISNS_FLAG_ReplaceFlag      0x1000
#define ISNS_FLAG_LastPDU          0x0800
#define ISNS_FLAG_FirstPDU         0x0400


/* iSNSP Response Status Codes (RFC4171 5.4)
 */
#define ISNS_STATUS_Successful              0 /* Successful                  */
#define ISNS_STATUS_UnknownError            1 /* Unknown Error               */
#define ISNS_STATUS_MessageFormatError      2 /* Message Format Error        */
#define ISNS_STATUS_InvalidRegistration     3 /* Invalid Registration        */
/*                                          4    RESERVED                    */
#define ISNS_STATUS_InvalidQuery            5 /* Invalid Query               */
#define ISNS_STATUS_SourceUnknown           6 /* Source Unknown              */
#define ISNS_STATUS_SourceAbsent            7 /* Source Absent               */
#define ISNS_STATUS_SourceUnauthorized      8 /* Source Unauthorized         */
#define ISNS_STATUS_NoSuchEntry             9 /* No Such Entry               */
#define ISNS_STATUS_VersionNotSupported    10 /* Version Not Supported       */
#define ISNS_STATUS_InternalError          11 /* Internal Error              */
#define ISNS_STATUS_Busy                   12 /* Busy                        */
#define ISNS_STATUS_OptionNotUnderstood    13 /* Option Not Understood       */
#define ISNS_STATUS_InvalidUpdate          14 /* Invalid Update              */
#define ISNS_STATUS_MessageNotSupported    15 /* Message (FUNC_ID) Not Supp. */
#define ISNS_STATUS_SCNEventRejected       16 /* SCN Event Rejected          */
#define ISNS_STATUS_SCNRegRejected         17 /* SCN Registration Rejected   */
#define ISNS_STATUS_AttrNotImplemented     18 /* Attribute Not Implemented   */
#define ISNS_STATUS_FCNotAvailable         19 /* FC_DOMAIN_ID Not Available  */
#define ISNS_STATUS_FCNotAllocated         20 /* FC_DOMAIN_ID Not Allocated  */
#define ISNS_STATUS_ESINotAvailable        21 /* ESI Not Available           */
#define ISNS_STATUS_InvalidDeregistration  22 /* Invalid Deregistration      */
#define ISNS_STATUS_RegFeatureNotSupported 23 /* Registr. Feature Not Supp.  */


/* Attributes which are relevant to iSCSI (RFC4171 6.1).  FC-specific tags are
 * not listed here, and have also been removed from the query_key column.
 */
/*  name                                    tag     reg_key    query_key     */
#define ISNS_TAG_Delimiter                   0  /*   N/A        N/A          */
#define ISNS_TAG_Entity_Identifier           1  /*    1     1|2|16&17|32     */
#define ISNS_TAG_Entity_Protocol             2  /*    1     1|2|16&17|32     */
#define ISNS_TAG_Management_IP_Address       3  /*    1     1|2|16&17|32     */
#define ISNS_TAG_Timestamp                   4  /*   --     1|2|16&17|32     */
#define ISNS_TAG_Protocol_Version_Range      5  /*    1     1|2|16&17|32     */
#define ISNS_TAG_Registration_Period         6  /*    1     1|2|16&17|32     */
#define ISNS_TAG_Entity_Index                7  /*    1     1|2|16&17|32     */
#define ISNS_TAG_Entity_Next_Index           8  /*   --     1|2|16&17|32     */
#define ISNS_TAG_Entity_ISAKMP_Phase_1      11  /*    1     1|2|16&17|32     */
#define ISNS_TAG_Entity_Certificate         12  /*    1     1|2|16&17|32     */
#define ISNS_TAG_Portal_IP_Address          16  /*    1     1|16&17|32       */
#define ISNS_TAG_Portal_TCP_UDP_Port        17  /*    1     1|16&17|32       */
#define ISNS_TAG_Portal_Symbolic_Name       18  /*  16&17   1|16&17|32       */
#define ISNS_TAG_ESI_Interval               19  /*  16&17   1|16&17|32       */
#define ISNS_TAG_ESI_Port                   20  /*  16&17   1|16&17|32       */
#define ISNS_TAG_Portal_Index               22  /*  16&17   1|16&17|32       */
#define ISNS_TAG_SCN_Port                   23  /*  16&17   1|16&17|32       */
#define ISNS_TAG_Portal_Next_Index          24  /*   --     1|16&17|32       */
#define ISNS_TAG_Portal_Security_Bitmap     27  /*  16&17   1|16&17|32       */
#define ISNS_TAG_Portal_ISAKMP_Phase_1      28  /*  16&17   1|16&17|32       */
#define ISNS_TAG_Portal_ISAKMP_Phase_2      29  /*  16&17   1|16&17|32       */
#define ISNS_TAG_Portal_Certificate         31  /*  16&17   1|16&17|32       */
#define ISNS_TAG_iSCSI_Name                 32  /*    1     1|16&17|32|33    */
#define ISNS_TAG_iSCSI_Node_Type            33  /*   32     1|16&17|32       */
#define ISNS_TAG_iSCSI_Alias                34  /*   32     1|16&17|32       */
#define ISNS_TAG_iSCSI_SCN_Bitmap           35  /*   32     1|16&17|32       */
#define ISNS_TAG_iSCSI_Node_Index           36  /*   32     1|16&17|32       */
#define ISNS_TAG_WWNN_Token                 37  /*   32     1|16&17|32       */
#define ISNS_TAG_iSCSI_Node_Next_Index      38  /*   --     1|16&17|32       */
#define ISNS_TAG_iSCSI_AuthMethod           42  /*   32     1|16&17|32       */
#define ISNS_TAG_PG_iSCSI_Name              48  /* 32|16&17 1|16&17|32|52    */
#define ISNS_TAG_PG_Portal_IP_Addr          49  /* 32|16&17 1|16&17|32|52    */
#define ISNS_TAG_PG_Portal_TCP_UDP_Port     50  /* 32|16&17 1|16&17|32|52    */
#define ISNS_TAG_PG_Tag                     51  /* 32|16&17 1|16&17|32|52    */
#define ISNS_TAG_PG_Index                   52  /* 32|16&17 1|16&17|32|52    */
#define ISNS_TAG_PG_Next_Index              53  /*   --     1|16&17|32|52    */
#define ISNS_TAG_iSNS_Server_Vendor_OUI    256  /*   --     SOURCE Attribute */
#define ISNS_TAG_DD_Set_ID                2049  /*   2049   1|32|2049|2065   */
#define ISNS_TAG_DD_Set_Sym_Name          2050  /*   2049   2049             */
#define ISNS_TAG_DD_Set_Status            2051  /*   2049   2049             */
#define ISNS_TAG_DD_Set_Next_ID           2052  /*   --     2049             */
#define ISNS_TAG_DD_ID                    2065  /*   2049   1|32|2049|2065   */
#define ISNS_TAG_DD_Symbolic_Name         2066  /*   2065   2065             */
#define ISNS_TAG_DD_Member_iSCSI_Index    2067  /*   2065   2065             */
#define ISNS_TAG_DD_Member_iSCSI_Name     2068  /*   2065   2065             */
#define ISNS_TAG_DD_Member_FC_Port_Name   2069  /*   2065   2065             */
#define ISNS_TAG_DD_Member_Portal_Index   2070  /*   2065   2065             */
#define ISNS_TAG_DD_Member_Portal_IP_Addr 2071  /*   2065   2065             */
#define ISNS_TAG_DD_Member_Portal_TCP_UDP 2072  /*   2065   2065             */
#define ISNS_TAG_DD_Features              2078  /*   2065   2065             */
#define ISNS_TAG_DD_ID_Next_ID            2079  /*   --     2065             */


/* Flags for Entity Protocol attribute (RFC4171 6.2.2)
 */
#define ISNS_ENTITY_PROTOCOL_None          0x00000001
#define ISNS_ENTITY_PROTOCOL_iSCSI         0x00000002
#define ISNS_ENTITY_PROTOCOL_iFCP          0x00000003


/* Flags for Portal Security Bitmap attribute (RFC4171 6.3.9)
 */
#define ISNS_PORTAL_SECURITY_TunnelPref    0x00000040
#define ISNS_PORTAL_SECURITY_TransportPref 0x00000020
#define ISNS_PORTAL_SECURITY_PFSEnabled    0x00000010
#define ISNS_PORTAL_SECURITY_Aggressive    0x00000008
#define ISNS_PORTAL_SECURITY_MainMode      0x00000004
#define ISNS_PORTAL_SECURITY_IKE_IPSec     0x00000002
#define ISNS_PORTAL_SECURITY_BitmapValid   0x00000001
#define ISNS_PORTAL_SECURITY_INVALID       0x00000000


/* Flags for iSCSI Node Type attribute (RFC4171 6.4.2)
 */
#define ISNS_ISCSI_NODE_TYPE_Control       0x00000004
#define ISNS_ISCSI_NODE_TYPE_Initiator     0x00000002
#define ISNS_ISCSI_NODE_TYPE_Target        0x00000001


/* Flags for iSCSI Node SCN Bitmap attribute (RFC4171 6.4.4)
 */
#define ISNS_ISCSI_NODE_SCN_InitiatorSelf  0x00000080
#define ISNS_ISCSI_NODE_SCN_TargetSelf     0x00000040
#define ISNS_ISCSI_NODE_SCN_Management     0x00000020
#define ISNS_ISCSI_NODE_SCN_ObjectRemoved  0x00000010
#define ISNS_ISCSI_NODE_SCN_ObjectAdded    0x00000008
#define ISNS_ISCSI_NODE_SCN_ObjectUpdated  0x00000004
#define ISNS_ISCSI_NODE_SCN_DD_DDS_Removed 0x00000002
#define ISNS_ISCSI_NODE_SCN_DD_DDS_Added   0x00000001


/* Flags for Discovery Domain Set Status attribute (RFC4171 6.11.1.3)
 */
#define ISNS_DDS_STATUS_Enabled            0x00000001


/* ============================================================================
 *  Types for our iSNS implementation
 * ============================================================================
 */

typedef struct ci_isns_portal_s {
  ci_dllink link;
  ci_uint32 ip_address;  /* no need for 16 bytes: we can only handle IPv4 */
  ci_uint16 port;
  const char *symbolic_name;
} ci_isns_portal_t;

typedef struct ci_isns_pgmemb_s {
  ci_dllink link;
  ci_isns_portal_t *portal;
} ci_isns_pgmemb_t;

typedef struct ci_isns_pgroup_s {
  ci_dllink link;
  const char *iscsi_name;
  ci_uint32 ip_address;
  ci_uint16 port;
  ci_uint32 tag;
  ci_dllist pgmembs;
} ci_isns_pgroup_t;

typedef struct ci_isns_snode_s {
  ci_dllink link;
  const char *iscsi_name;
  const char *iscsi_alias;
  ci_dllist pgroups;
} ci_isns_snode_t;

typedef struct ci_isns_entity_s {
  ci_dllink link;
  const char *id;
  ci_dllist portals;
  ci_dllist snodes;
} ci_isns_entity_t;

typedef struct ci_isns_state_s {

  /* Low-level output state */
  struct {
    ci_uint8 *   pdu_start;
    ci_uint8 *   pdu_ptr;
    ci_uint8 *   pdu_end;
    ci_uint8 *   pdu_build_start;
    int          pdu_build_error;
    ci_uint16    current_func;
    ci_uint16    current_xid;
    ci_uint16    current_seq;
  } out;

  /* Low-level input state */
  struct {
    ci_uint8     pdu_hdr_buf[12];
    unsigned int pdu_hdr_pos;
    ci_uint8     tlv_hdr_buf[8];
    unsigned int tlv_hdr_pos;
    ci_uint16    pdu_flags;
    ci_uint32    pdu_bytes_left;
    ci_uint16    current_func;
    ci_uint16    current_xid;
    ci_uint16    current_seq;
  } in;

  /* Client processing state */
  struct {
    int          result;
    char *       source;
    char *       entity_id;
    ci_dllist    entities;
    ci_dllist    strings;
  } client;

  /* Socket state for server connection */
  struct {
    tips_socket  ep;
    ci_uint32    server_ip_address;
    ci_uint16    server_port;
  } conn;

} ci_isns_state_t;


/* ============================================================================
 *  Interface to our iSNS implementation
 * ============================================================================
 */

extern void ci_isns_client_ctor(ci_isns_state_t *state);

extern void ci_isns_client_set_server(ci_isns_state_t *state,
                                      ci_uint32 ip_address, ci_uint16 port);

extern void ci_isns_client_free_data(ci_isns_state_t *state);

extern int ci_isns_client_register(ci_isns_state_t *state, const char *name,
                                   unsigned int period);

extern int ci_isns_client_refresh(ci_isns_state_t *state);

extern int ci_isns_client_query_targets(ci_isns_state_t *state);

extern int ci_isns_client_deregister(ci_isns_state_t *state);

extern void ci_isns_client_dtor(ci_isns_state_t *state);

#ifndef NDEBUG
extern void ci_isns_dump_tree(ci_isns_state_t *state);
#endif


#endif /* __CI_ISCSI_ISNS_H__ */

/*! \cidoxg_end */

