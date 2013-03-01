#ifndef _ISCSI_PROTOCOL_H
#define _ISCSI_PROTOCOL_H


enum iscsi_opcode {
/* Initiator opcodes */
  ISCSI_OP_NOP_OUT          = 0x00,
  ISCSI_OP_SCSI_CMD         = 0x01,
  ISCSI_OP_SCSI_TMFUNC      = 0x02,
  ISCSI_OP_LOGIN            = 0x03,
  ISCSI_OP_TEXT             = 0x04,
  ISCSI_OP_SCSI_DATA_OUT    = 0x05,
  ISCSI_OP_LOGOUT           = 0x06,
  ISCSI_OP_SNACK            = 0x10,

  /* Target opcodes */
  ISCSI_OP_NOP_IN           = 0x20,
  ISCSI_OP_SCSI_CMD_RSP     = 0x21,
  ISCSI_OP_SCSI_TMFUNC_RSP  = 0x22,
  ISCSI_OP_LOGIN_RSP        = 0x23,
  ISCSI_OP_TEXT_RSP         = 0x24,
  ISCSI_OP_SCSI_DATA_IN     = 0x25,
  ISCSI_OP_LOGOUT_RSP       = 0x26,
  ISCSI_OP_R2T              = 0x31,
  ISCSI_OP_ASYNC_EVENT      = 0x32,
  ISCSI_OP_REJECT           = 0x3f,
};

enum iscsi_status {
  ISCSI_STATUS_GOOD                 = 0x00,
  ISCSI_STATUS_CHECK_CONDITION      = 0x02,
  ISCSI_STATUS_BUSY                 = 0x08,
  ISCSI_STATUS_RESERVATION_CONFLICT = 0x18,
  ISCSI_STATUS_TASK_SET_FULL        = 0x28,
  ISCSI_STATUS_ACA_ACTIVE           = 0x30,
  ISCSI_STATUS_TASK_ABORTED         = 0x40
};

enum iscsi_response {
  ISCSI_RESPONSE_COMMAND_COMPLETE = 0x00,
  ISCSI_RESPONSE_TARGET_FAILURE   = 0x01
};

enum iscsi_login_phase { SecurityNegotiation          = 0,
                         LoginOperationalNegotiation  = 1,
                         FullFeaturePhase             = 3 };


enum iscsi_async_event {
  ISCSI_ASYNC_EVENT_SCSI_AEN             = 0x00,
  ISCSI_ASYNC_EVENT_LOGOUT               = 0x01,
  ISCSI_ASYNC_EVENT_DROP_CONNECTION      = 0x02,
  ISCSI_ASYNC_EVENT_DROP_ALL_CONNECTIONS = 0x03,
  ISCSI_ASYNC_EVENT_NEGOTIATE            = 0x04,
  ISCSI_ASYNC_EVENT_VENDOR               = 0xFF,
};

enum iscsi_tmfunc {
  ISCSI_TMFUNC_NONE               = 0x00,
  ISCSI_TMFUNC_ABORT_TASK         = 0x01,
  ISCSI_TMFUNC_ABORT_TASK_SET     = 0x02,
  ISCSI_TMFUNC_CLEAR_ACA          = 0x03,
  ISCSI_TMFUNC_CLEAR_TASK_SET     = 0x04,
  ISCSI_TMFUNC_LOGICAL_UNIT_RESET = 0x05,
  ISCSI_TMFUNC_TARGET_WARM_RESET  = 0x06,
  ISCSI_TMFUNC_TARGET_COLD_RESET  = 0x07,
  ISCSI_TMFUNC_TASK_REASSIGN      = 0x08
};

enum {AttrUntagged=0, AttrSimple=1, AttrOrdered=2, AttrHeadOfQueue=3, AttrACA=4};

#endif /* ISCSI_PROTOCOL_H  */
