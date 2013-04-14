/**************************************************************************\
*//*! \file internal.h
** <L5_PRIVATE L5_HEADER >
** \author  mjs
**  \brief  ISCSI interface to SCSI devices
**   \date  2005/07/06
**    \cop  (c) Level 5 Networks Limited.
** </L5_PRIVATE>
*//*
\**************************************************************************/

/*! \cidoxg_include_ci_iscsi */

#ifndef __ISCSI_TYPES_H__
#define __ISCSI_TYPES_H__

/*
 * A collection of handles.
 */
#ifdef __ci_storport__
typedef void *ci_iscsi_tm_handle;
typedef struct ci_iscsi_cmd_info *ci_iscsi_cmd_handle;
#endif


/*
 * Objects upon which other implementations can wait.
 */
struct ci_wait_object_s{
  ci_uint32   state;
};
typedef struct ci_wait_object_s     ci_wait_object;


#ifdef __ci_storport__
struct ci_sock_cmn_s {
  int                   domain;

  ci_uint32             s_aflags;
#define CI_SOCK_AFLAG_RCVTIMEO          0x04          /* SO_RCVTIMEO  */
#define CI_SOCK_AFLAG_RCVTIMEO_BIT      2u
#define CI_SOCK_AFLAG_SNDTIMEO          0x08          /* SO_SNDTIMEO  */
#define CI_SOCK_AFLAG_SNDTIMEO_BIT      3u
#if 0
#define CI_SOCK_AFLAG_CORK              0x01          /* TCP_CORK     */
#define CI_SOCK_AFLAG_CORK_BIT          0u
#define CI_SOCK_AFLAG_NONBLOCK          0x40         /* MSG_DONTWAIT */
#define CI_SOCK_AFLAG_NONBLOCK_BIT      6u
#define CI_SOCK_AFLAG_NDELAY            CI_SOCK_AFLAG_NONBLOCK
#define CI_SOCK_AFLAG_NDELAY_BIT        CI_SOCK_AFLAG_NONBLOCK_BIT
#define CI_SOCK_AFLAG_ASYNC             0x80         /* O_ASYNC      */
#define CI_SOCK_AFLAG_ASYNC_BIT         7u
#define CI_SOCK_AFLAG_APPEND            0x100        /* O_APPEND     */
#define CI_SOCK_AFLAG_APPEND_BIT        8u
#define CI_SOCK_AFLAG_NODELAY           0x200        /* TCP_NODELAY  */
#define CI_SOCK_AFLAG_NODELAY_BIT       9u
  /* Some TCP-specific flags, but we don't want another flags field. */
#define CI_SOCK_AFLAG_NEED_ACK          0x400
#define CI_SOCK_AFLAG_NEED_ACK_BIT      10u
#endif

  struct {
    struct oo_timeval   rcvtimeo;
    struct oo_timeval   sndtimeo;
    /* This contains only sockopts that are inherited from the listening
    ** socket by newly accepted TCP sockets.
    */
    ci_int32            sndbuf;
    ci_int32            rcvbuf;
    ci_uint32           linger;
    ci_int32            rcvlowat;
    ci_int32            so_debug; /* Flags for dummy options */
# define CI_SOCKOPT_FLAG_SO_DEBUG    0x1
# define CI_SOCKOPT_FLAG_IP_RECVERR  0x2
  } so;

  ci_wait_object        b;
};

typedef struct ci_sock_cmn_s         ci_sock_cmn;

struct ci_tcp_state_s {
  ci_sock_cmn s;
};

union citp_waitable_obj_u {
  ci_wait_object        waitable;
  ci_sock_cmn           sock;
  ci_tcp_state          tcp;
};


/*
 * The following (kernelspace -> userspace) pointer type "ci_user_ptr_t", is
 * defined in "onload/primitive_types.h", which I don't want to include in the
 * Storport environment.  
 * TODO: consider moving "ci_user_ptr_t" to ci/compat.h.
 */
/* A fixed width ptr wrapper. */
typedef struct {
  ci_uint64 ptr CI_ALIGN(8);
} ci_user_ptr_t;

#endif


/*
 * The following union is used in this files nd in sis_protocol.h, so must
 * be defined before sis_protocol is included.
 * TODO: consider moving "ci_iscsi_lun" to ci/compat.h.
 */
typedef union {
  ci_uint64 data64;
  ci_uint32 data32[2];
  ci_uint16 data16[4];
} ci_iscsi_lun;

#define CI_ISCSI_LUN_INVAL {0xffffffffffffffff}

/* This is a struture for the iSCSI CMD PDUs.
 */
typedef struct iscsi_generic_pdu {
  /* The opcode */
  struct {
    unsigned char  code      : 6;
    unsigned char  immediate : 1;
    unsigned char  reserved  : 1;
  } op;

  ci_uint8 pad[3];

  unsigned TotalAHSLength    : 8;
  unsigned DataSegmentLength : 24;

} ci_iscsi_generic_pdu;

/* This is a struture for the iSCSI CMD PDUs.
 */
typedef struct iscsi_cmd_pdu {
  /* The opcode (always 3 for login) */
  struct {
    unsigned char  code      : 6;
    unsigned char  immediate : 1;
    unsigned char  reserved  : 1;
  } op;

  /* Flags */
  struct {
    unsigned char  attr  : 3;
    unsigned char  pad   : 2;
    unsigned char  write : 1;
    unsigned char  read  : 1;
    unsigned char  final : 1;
  } flags;

  ci_uint16 pad0;

  unsigned TotalAHSLength    : 8;
  unsigned DataSegmentLength : 24;

  ci_iscsi_lun LUN;

  ci_uint32 ITT;

  ci_uint32 ExpectedDataLen;

  /* Command sequence number */
  ci_uint32 CmdSN;

  /* Expected status sequence number */
  ci_uint32 ExpStatSN;

  /* The SCSI command itself */
  unsigned char  SCSI_CDB [16];

  /* OPTIONAL header digest */
  ci_uint32 header_digest;

  /* Other funkiness follows (none relevant to current limited impl */
} ci_iscsi_cmd_pdu;


/* This is a struture for the iSCSI LOGIN PDUs.  There are two kinds of login
 * PDUs -- request and response.  Both follow pretty-much the same layout, so
 * both are encoded with one structure
 *
 * We also use this same struct for iSCSI LOGOUT PDUs (again request and
 * response) as they're also virtually the same layout.
 */
typedef struct iscsi_login_pdu {
  /* The opcode */
  struct {
    unsigned char  code      : 6;
    unsigned char  immediate : 1;
    unsigned char  pad       : 1;
  } op;

  /* Flags */
  struct {
    unsigned char  nsg     : 2;
    unsigned char  csg     : 2;
    unsigned char  pad0    : 1;
    unsigned char  pad1    : 1;
    unsigned char  continu : 1;
    unsigned char  transit : 1;
  } flags;

  /* Versions accepted */
  ci_uint8 ver_max;          /* "response" for LOGOUT response */
  ci_uint8 ver_min;

  unsigned TotalAHSLen : 8;
  unsigned data_len  : 24;

  ci_uint8 isid[6];          /* used for LOGIN only */
  ci_uint16 tsih;            /* used for LOGIN only */

  ci_uint32 ITT;
  ci_uint16 CID;             /* used for requests only */
  ci_uint16 reserved;

  /* Command sequence number */
  ci_uint32 CmdSN;           /* StatSN for responses */

  /* Expected status sequence number */
  ci_uint32 ExpStatSN;       /* ExpCmdSN for responses */
  ci_uint32 MaxCmdSN;        /* used for responses only */

  ci_uint8  status_class;    /* used for LOGIN response only */
  ci_uint8  status_detail;   /* used for LOGIN response only */
  ci_uint16 reserved2;

  ci_uint16 time2wait;       /* used for LOGOUT response only */
  ci_uint16 time2retain;     /* used for LOGOUT response only */

  ci_uint32 reserved3;

  /* OPTIONAL header digest */
  ci_uint32 header_digest;

  /* data-segment (text-login params) follow */
} ci_iscsi_login_pdu;

/* This is a struture for the iSCSI TEXT PDUs.  There are two kinds of text
 * PDUs -- request and response.  Both follow pretty-much the same layout, so
 * both are encoded with one structure
 */
typedef struct iscsi_text_pdu {
  /* The opcode (always 4 for request, 0x24 for response) */
  struct {
    unsigned char code      : 6;
    unsigned char immediate : 1;
    unsigned char pad       : 1;
  } op;

  /* Flags */
  struct {
    unsigned char pad0    : 6;
    unsigned char continu : 1;
    unsigned char final   : 1;
    unsigned char pad1;
    unsigned char pad2;
  } flags;

  unsigned TotalAHSLength : 8;
  unsigned int data_len   : 24;

  ci_iscsi_lun LUN;       /* 64 bits for LUN !!! */

  ci_uint32 ITT;        /* Initiator Task Tag */
  ci_uint32 TTT;        /* Target Transfer Tag */
  ci_uint32 CmdSN;      /* Command sequence number / StatSN for response */
  ci_uint32 ExpStatSN;  /* Expected status sequence number /
			** ExpCmdSN for response */
  ci_uint32 MaxCmdSN;   /* Max sequence number (response only) */

  ci_uint32 pad1;
  ci_uint32 pad2;
  ci_uint32 pad3;

  /* OPTIONAL header digest */
  ci_uint32 header_digest;

  /* data-segment (text params) follow */
} ci_iscsi_text_pdu;


typedef struct iscsi_cmd_rsp_pdu {
  /* The opcode (always 3 for login) */
  struct {
    unsigned char  code      : 6;
    unsigned char  immediate : 1;
    unsigned char  reserved  : 1;
  } op;

  /* Flags */
  struct {
    unsigned char  reserved : 1;
    unsigned char  U        : 1;
    unsigned char  O        : 1;
    unsigned char  u        : 1;
    unsigned char  o        : 1;
    unsigned char  reserved2: 2;
    unsigned char  one      : 1;
  } flags;

  ci_uint8 response;
  
  ci_uint8 status;

  unsigned TotalAHSLength    : 8;
  unsigned DataSegmentLength : 24;

  ci_uint64 reserved;

  ci_uint32 ITT;

  ci_uint32 SNACKTAG;

  ci_uint32 StatSN;

  /* Expected command sequence number */
  ci_uint32 ExpCmdSN;

  /* Expected status sequence number */
  ci_uint32 MaxCmdSN;

  /* Expected status sequence number */
  ci_uint32 ExpDataSN;

  /* Bidirectional Read Residual Count */
  ci_uint32 BidrecReadResidCount;

  /* Residual Count */
  ci_uint32 ResidCount;

  /* OPTIONAL header digest */
  ci_uint32 header_digest;

  /* Data segment follows */
} ci_iscsi_cmd_rsp_pdu;

typedef struct iscsi_data_in_pdu {
  /* The opcode (always 3 for login) */
  struct {
    unsigned char  code      : 6;
    unsigned char  reserved  : 2;
  } op;

  /* Flags */
  struct {
    unsigned char  status   : 1;
    unsigned char  U        : 1;
    unsigned char  O        : 1;
    unsigned char  zero     : 3;
    unsigned char  ack      : 1;
    unsigned char  final    : 1;
  } flags;

  ci_uint8 reserved;
  
  ci_uint8 status;

  unsigned TotalAHSLength    : 8;
  unsigned DataSegmentLength : 24;

  ci_iscsi_lun LUN;

  ci_uint32 ITT;

  ci_uint32 TTT;

  /* Current status sequence number */
  ci_uint32 StatSN;

  /* Expected command sequence number */
  ci_uint32 ExpCmdSN;

  /* Maximum command sequence number */
  ci_uint32 MaxCmdSN;

  /* Data-in PDU sequence number (when data spans multiple PDUs) */
  ci_uint32 DataSN;

  /* Offset of this data-in PDU (when data spans multiple PDUs) */
  ci_uint32 BufferOffset;

  /* Residual Count */
  ci_uint32 ResidCount;

  /* OPTIONAL header digest */
  ci_uint32 header_digest;

  /* Data segment follows */
} ci_iscsi_data_in_pdu;

typedef struct iscsi_data_out_pdu {
  /* The opcode (always 3 for login) */
  struct {
    unsigned char  code      : 6;
    unsigned char  reserved  : 2;
  } op;

  /* Flags */
  struct {
    unsigned char  pad      : 7;
    unsigned char  final    : 1;
  } flags;

  ci_uint16 reserved;
  
  unsigned TotalAHSLength    : 8;
  unsigned DataSegmentLength : 24;

  ci_iscsi_lun LUN;

  ci_uint32 ITT;

  ci_uint32 TTT;

  ci_uint32 resrerved2;

  /* Expected status sequence number */
  ci_uint32 ExpStatSN;

  ci_uint32 reserved3;

  /* Data-in PDU sequence number (when data spans multiple PDUs) */
  ci_uint32 DataSN;

  /* Offset of this data-in PDU (when data spans multiple PDUs) */
  ci_uint32 BufferOffset;

  ci_uint32 reserved4;

  /* OPTIONAL header digest */
  ci_uint32 header_digest;

  /* Data segment follows */
} ci_iscsi_data_out_pdu;


typedef struct iscsi_tmfunc_req_pdu {
  /* The opcode (always 3 for login) */
  struct {
    unsigned char  code      : 6;
    unsigned char  immediate : 1;
    unsigned char  reserved  : 1;
  } op;

  /* Flags */
  struct {
    unsigned char  function : 7;
    unsigned char  one      : 1;
  } flags;

  ci_uint16 reserved1;
  
  unsigned TotalAHSLength    : 8;
  unsigned DataSegmentLength : 24;

  ci_iscsi_lun LUN;

  ci_uint32 ITT;

  ci_uint32 ReferencedTaskTag;

  /* Current command sequence number */
  ci_uint32 CmdSN;

  /* Expected status sequence number */
  ci_uint32 ExpStatSN;

  /* Referenced command sequence number */
  ci_uint32 RefCmdSN;

  /* Expected data sequence number */
  ci_uint32 ExpDataSN;

  ci_uint32 reserved2;
  ci_uint32 reserved3;

  /* OPTIONAL header digest */
  ci_uint32 header_digest;

  /* Data segment follows */
} ci_iscsi_tmfunc_req_pdu;


typedef struct iscsi_tmfunc_resp_pdu {
  /* The opcode (always 3 for login) */
  struct {
    unsigned char  code      : 6;
    unsigned char  immediate : 1;
    unsigned char  reserved  : 1;
  } op;

  /* Flags */
  struct {
    unsigned char  function : 7;
    unsigned char  one      : 1;
  } flags;

  ci_uint8 response;
  ci_uint8 reserved1;
  
  unsigned TotalAHSLength    : 8;
  unsigned DataSegmentLength : 24;

  ci_iscsi_lun reserved2;
  ci_uint32 ITT;
  ci_uint32 reserved3;
  ci_uint32 StatSN;
  ci_uint32 ExpCmdSN;
  ci_uint32 MaxCmdSN;
  ci_uint32 reserved4;

  /* OPTIONAL header digest */
  ci_uint32 header_digest;

  /* Data segment follows */
} ci_iscsi_tmfunc_resp_pdu;


typedef struct iscsi_reject_pdu {
  /* The opcode (always 3 for login) */
  struct {
    unsigned char  code      : 6;
    unsigned char  immediate : 1;
    unsigned char  reserved  : 1;
  } op;

  ci_uint8 reserved1;
  ci_uint8 reason;
  ci_uint8 reserved2;

  unsigned TotalAHSLength    : 8;
  unsigned DataSegmentLength : 24;

  ci_uint8 reserved3 [16];

  ci_uint32 StatSN;
  ci_uint32 ExpCmdSN;
  ci_uint32 MaxCmdSN;

  ci_uint32 DataSN;
  ci_uint64 Reserved;

  /* OPTIONAL header digest */
  ci_uint32 header_digest;

  /* Data segment follows (often the bad PDU) */
} ci_iscsi_reject_pdu;

typedef struct iscsi_r2t_pdu {
  /* The opcode (always 3 for login) */
  struct {
    unsigned char  code      : 6;
    unsigned char  reserved  : 2;
  } op;

  ci_uint8 reserved1;
  ci_uint16 reserved2;

  unsigned TotalAHSLength    : 8;
  unsigned DataSegmentLength : 24;

  ci_iscsi_lun LUN;

  ci_uint32 ITT;

  ci_uint32 TTT;

  ci_uint32 StatSN;
  ci_uint32 ExpCmdSN;
  ci_uint32 MaxCmdSN;

  ci_uint32 R2TSN;
  ci_uint32 BufferOffset;
  ci_uint32 DesiredTransferLength;

  /* OPTIONAL header digest */
  ci_uint32 header_digest;

  /* Data segment follows (often the bad PDU) */
} ci_iscsi_r2t_pdu;


typedef struct iscsi_nop_in_pdu {
  /* The opcode (always 32 for NOP-In) */
  struct {
    unsigned char  code      : 6;
    unsigned char  reserved  : 2;
  } op;

  ci_uint8 reserved;
  
  ci_uint16 reserved2;

  unsigned TotalAHSLength    : 8;
  unsigned DataSegmentLength : 24;

  ci_iscsi_lun LUN;

  ci_uint32 ITT;

  ci_uint32 TTT;

  ci_uint32 StatSN;

  /* Expected status sequence number */
  ci_uint32 ExpCmdSN;

  /* Expected status sequence number */
  ci_uint32 MaxCmdSN;

  ci_uint32 reserved3;
  ci_uint64 reserved4;

  /* OPTIONAL header digest */
  ci_uint32 header_digest;

  /* Data segment follows */
} ci_iscsi_nop_in_pdu;


typedef struct iscsi_nop_out_pdu {
  /* The opcode (always 0 for NOP-Out) */
  struct {
    unsigned char  code      : 6;
    unsigned char  immediate : 1;
    unsigned char  reserved  : 1;
  } op;

  /* Flags */
  struct {
    unsigned char  pad      : 7;
    unsigned char  final    : 1;
  } flags;
  
  ci_uint16 reserved;

  unsigned TotalAHSLength    : 8;
  unsigned DataSegmentLength : 24;

  ci_iscsi_lun LUN;

  ci_uint32 ITT;

  ci_uint32 TTT;

  /* Current command sequence number */
  ci_uint32 CmdSN;

  /* Expected status sequence number */
  ci_uint32 ExpStatSN;

  ci_uint32 reserved2;
  ci_uint32 reserved3;
  ci_uint64 reserved4;

  /* OPTIONAL header digest */
  ci_uint32 header_digest;

  /* Data segment follows */
} ci_iscsi_nop_out_pdu;


typedef struct iscsi_async_pdu {
  /* The opcode (always 3 for login) */
  struct {
    unsigned char  code      : 6;
    unsigned char  reserved  : 2;
  } op;

  ci_uint8 reserved;
  
  ci_uint16 reserved2;

  unsigned TotalAHSLength    : 8;
  unsigned DataSegmentLength : 24;

  ci_iscsi_lun LUN;

  ci_uint64 reserved3;

  ci_uint32 StatSN;

  /* Expected status sequence number */
  ci_uint32 ExpCmdSN;

  /* Expected status sequence number */
  ci_uint32 MaxCmdSN;

  ci_uint8  AsyncEvent;
  ci_uint8  AsyncVCode;
  ci_uint16 Param1;
  ci_uint16 Param2;
  ci_uint16 Param3;

  ci_uint32 reserved4;

  /* OPTIONAL header digest */
  ci_uint32 header_digest;

  /* Data segment follows */
} ci_iscsi_async_pdu;


typedef union iscsi_pdu {

  ci_iscsi_generic_pdu  bhs_generic;
  ci_iscsi_login_pdu    login;
  ci_iscsi_text_pdu     text;
  ci_iscsi_cmd_pdu      cmd;
  ci_iscsi_cmd_rsp_pdu  cmd_rsp;
  ci_iscsi_data_in_pdu  data_in;
  ci_iscsi_data_out_pdu data_out;
  ci_iscsi_reject_pdu   reject;
  ci_iscsi_r2t_pdu      r2t;
  ci_iscsi_nop_in_pdu   nop_in;
  ci_iscsi_async_pdu    async;
  struct {
    ci_uint8  header[48];
    ci_uint32 header_digest;
  } generic;
} ci_iscsi_pdu;

#endif /* __ISCSI_TYPES_H__  */

/*! \cidoxg_end */

