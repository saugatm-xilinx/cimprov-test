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

#ifndef __CI_ISCSI_INTERNAL_H__
#define __CI_ISCSI_INTERNAL_H__

#ifndef __ci_driver__
#  error Driver only
#endif

#ifdef __ci_storport__
#ifndef __ci_iscsi__
#  error iSCSI only
#endif

# include <miniport.h>
# include <stor_abstrct.h>
# include <stor_early.h>
# include <ci/internal/version.h>
# include <ci/tools.h>
# include <ci/tools/crc32c.h>
# include <ci/tools/iovec.h>
# include <ci/tools/log.h>

#else

# include <ci/internal/transport_config_opt.h>
# include <ci/internal/tips_onload.h>
# include <ci/internal/ip.h>
# include <ci/tools.h>
# include <ci/tools/iovec.h>
# include <ci/tools/crc32c.h>
# include <onload/tcp_helper_fns.h>
# ifdef _WIN32
#  include <../driver/win/iscsi/iscsi_internal.h>
# endif

#endif


#include <ci/iscsi/sis_types.h>
#include <ci/iscsi/sis_interface.h>
#include <ci/iscsi/sis_protocol.h>
#include <ci/iscsi/sis_timer.h>
#include <ci/iscsi/sis_stats.h>
#include <ci/iscsi/sis_citools.h>

#ifdef __ci_storport__

#include <stor_iscsi.h>
#include <ci/iscsi/tips_lwip.h>   // TODO: decide correct location for this file
#endif


/* Extra debug tracing options: */
#define CI_ISCSI_TXTHREAD_DEBUG      1  /* Timestamps of tx thread activity */
#define CI_ISCSI_COUNT_SCSI_COMMANDS 1  /* Counts of each SCSI command */
#define CI_ISCSI_COUNT_DATA_LENGTHS  1  /* Distribution of data lengths */


#ifdef __ci_storport__
# define ISCSI_TXTHREAD_TIME_UPDATE(t)
#else
# if CI_ISCSI_TXTHREAD_DEBUG
#  define ISCSI_TXTHREAD_TIME_UPDATE(t) ci_frc64(&(t))
# else
#  define ISCSI_TXTHREAD_TIME_UPDATE(t)
# endif
#endif

#if CI_CFG_ISCSI_ZC_TX

#define CI_ISCSI_ASYNC_TX 1  /* iSCSI async tx (zero-copy) */

#else /* CI_CFG_ISCSI_ZC_TX */
#define CI_ISCSI_ASYNC_TX 0  /* No iSCSI async tx (zero-copy) */
#endif

#ifdef WIN32  //async-tx not working quite right on Windows just yet
#undef CI_ISCSI_ASYNC_TX
#define CI_ISCSI_ASYNC_TX 0
#endif


/* iSCSI timeout for login/logout request/response turnaround, etc... see RFC
 * 3720, section 9.3 (p109) for discussion of such timeouts; we just use a
 * generous fixed value for now.
 *
 * For Windows this is now configurable via the registry, with the value
 * below being the default.
 */
#define CI_ISCSI_TIMEOUT_MSECS 3000



/* ============================================================================
 *  Import OS-specific SCSI driver definitions
 * ============================================================================
 */

#ifdef __ci_driver__

#ifdef __linux__
#include <ci/driver/efab/linux_iscsi.h>
#if defined(__x86_64__)
typedef ci_uint64 INT_PTR;
#else
typedef ci_uint32 INT_PTR;
#endif
#endif

#ifdef __ci_wdm__
#include <ci/driver/efab/win_iscsi.h>

/* Calls to the wdm driver need to go via the sicsi_link */
#include <ci/driver/efab/iscsi_link.h>

#endif /* _WIN32 */

#ifdef __ci_storport__

/*! We use the per session scsi_handle to locate our device extension.
 * Unfortunately we run into dependency nasties defining HW_DEVICE_EXTENSION
 * here, so we use a void * instead.
 */
struct         ci_iscsi_scsi_handle_s;
typedef struct ci_iscsi_scsi_handle_s *ci_iscsi_scsi_handle;

struct         ci_iscsi_cmd_info;
typedef struct ci_iscsi_cmd_info      *ci_iscsi_cmd_handle;

typedef        void                   *ci_iscsi_tm_handle;

struct         ci_iscsi_session_s;
typedef struct ci_iscsi_session_s     ci_iscsi_session;

/* Handles are NULL for invalid */
ci_inline void ci_iscsi_cmd_handle_invalidate(ci_iscsi_cmd_handle *handlep)
{
  ci_assert(handlep!=NULL);
  *handlep = NULL;
}


ci_inline int ci_iscsi_cmd_handle_is_valid(ci_iscsi_cmd_handle handle)
{
  return handle != NULL;
}


ci_inline void ci_iscsi_scsi_handle_invalidate(ci_iscsi_scsi_handle *handle)
{
  /* ?? Not needed for Storport ?? */
}


#define CI_MSG_SOCK_LOCK_NOT_NEEDED 0x20000000

#define CI_MSG_ISCSI_TX_HW_DDIGEST  0x04000000
#define CI_MSG_ISCSI_TX_HW_HDIGEST  0x02000000

typedef void *ci_os_scatterlist_t;

#endif

#else

/* TODO: no user-level ISCSI yet */
#error No user-level iSCSI

#endif /* __ci_driver__ */


/* ============================================================================
 *  Constants
 * ============================================================================
 */

#define CI_ISCSI_VISIBLE_NAME "Solarstorm iSCSI HBA"
enum {SG_SIZE = 128};
enum {QUEUE_SIZE = 256};
enum {MAX_R2Ts = 1};
enum {MAX_RECV_DSL = 8*1024};    /* Must be at least 8192, and a multiple of 4 */
enum {R2T_QUEUE_SIZE = (MAX_R2Ts * QUEUE_SIZE)};
#define CI_ISCSI_MAX_TRANSFER (256*1024)
#define CI_ISCSI_MAX_TX_SIZE 65536
#define CI_ISCSI_IOV_SIZE (2*SG_SIZE + (CI_ISCSI_MAX_TX_SIZE/CI_PAGE_SIZE))
/* This should indicate the typical size of a scatter-gather list
 * entry.  There's no point it being over CI_PAGE_SIZE. */
#define CI_ISCSI_RX_DATA_CHUNK 1024


/* ============================================================================
 *  Global declarations
 * ============================================================================
 */

/* This boolean used primarily for debugging, but also useful for real-world
 * situations where one doesn't want to use async.
 * Controlled via the ISCSI_CONTROL_ZC_TX op.
 */
extern ci_boolean_t ci_iscsi_zc_tx_is_enabled;

/* A similar tweakable setting to allow disabling hardware Tx digest offload.
 * Controlled via the ISCSI_CONTROL_TX_DO op.
 * (Where Tx digest offload is not available, we #define this to 0 instead, in
 * the hope that the compiler will optimise away the code to handle it.)
 */
#if CI_CFG_ISCSI_TX_DIGEST_OFFLOAD
extern ci_boolean_t ci_iscsi_tx_do_is_enabled;
#else
#define ci_iscsi_tx_do_is_enabled 0
#endif


/* ============================================================================
 *  Main data structures
 * ============================================================================
 */

struct ci_iscsi_connection_s;  /* (just a forward declaration) */
struct ci_iscsi_session_s;  /* (just a forward declaration) */
struct ci_iscsi_chap_keys_s;  /* (just a forward declaration) */

/* This is used to iterate over OS scatter-lists */
struct memdesc_cursor {
  struct memdesc_iter {
    unsigned int  old_offset;     /* to repair async's damage to scatterlist */
    unsigned int  old_length;     /* to repair async's damage to scatterlist */
    unsigned int  logical_offs;   /* logical offset of cursor */
    ci_os_scatterlist_t *sg;      /* sg element the cursor points at */
    unsigned int         offset;  /* cursor's offs within sg element */
  } start, end;
};

/* CHAP info. One per ci_iscsi_connection. */
typedef struct ci_iscsi_chap_keys_s {
  unsigned char c[1024];
  size_t c_length;
  unsigned char i;
  unsigned char r[16];
  char n[CI_ISCSI_SECRET_LEN];
} ci_iscsi_chap_keys;


/* The pending command queue needs to be a lock free FIFO with multiple
 * producers and one consumer. To achieve this we use 2 stacks.
 * The producers add to the first (queue) stack as normal.
 * The consumer reads from the second (reorder) stack. When this stack
 * is empty the consumer atomically removes all items from the first
 * stack and then reverses them into the second stack.
 * This can only be lock free because there is only one consumer.
 * Since items are only ever removed from the first stack all together
 * rather than one at a time we don't need a generation counter.
 * And we do the same thing for R2Ts
 */

typedef struct iscsi_fifo {
  ci_atomic_t in_queue;
  int         reorder;
} iscsi_fifo_t;


/* Connection (one per TCP connection; each lives in one session, multiple per
 * session)
 *
 * Each connection has a state to tracks its progress through the
 * "free -> login -> in use -> logout" life cycle, including waiting for
 * necessary replies from the target. This is acheived via a state which tells
 * us which part of the life cycle we're in, and a set of event flags that tell
 * us what event have occurred that may cause us to advance our state.
 *
 * The state is only ever changed in the tx thread. The event flags can be
 * set in any context. To avoid losing events the flags should only ever be
 * altered via the set and clear routines provided.
 *
 * Connection operational states:                                 (version 6)
 * free          - Not in use (possibly on free list)
 * connecting    - Performing login procedure
 * active        - Commands can be processed
 * queue closed  - Existing commands can be processed, but no new commands
 *                 may be issued
 * logout        - Logout request has been sent, waiting for logout response
 * nap           - Waiting for timeout before recovering
 * zombie        - Disconnected, but not by an ioctl. May be resurrected
 * purgatory     - Waiting for unexpected close timeout before disconnecting
 *
 * Note that all RX threads etc should be stopped before a connection is put
 * back into the "free" state.
 *
 * Event flags:
 * login               - we want to perform a login
 * logout              - we want to perform a logout
 * got logout response - we've received a logout response
 * active              - connection has reached the active state and has not
 *                       received a logout ioctl
 * last command        - we've processed all our outstanding commands
 * login ioctl         - there is currently a login ioctl waiting for us
 * logout ioctl        - there is currently a logout ioctl waiting for us
 * timeout             - a timeout has occurred
 * drop                - we want to drop the connection
 * recovery            - we want to perform recovery
 *
 * Note that the recovery flag is handled separately since it is a session
 * wide event, whereas all the others are connection based.
 *
 * Connection operational states transitions:
 * old state     -> new state        (event)
 * free          -> connecting       (login)
 * connecting    -> zombie           (login fail)
 * connecting    -> active           (login pass) #
 * connecting    -> purgatory        (drop) #
 * connecting    -> nap              (recovery)
 * active        -> nap              (recovery)
 * active        -> queue closed     (logout)
 * active        -> purgatory        (drop) #
 * queue closed  -> logout           (last command)
 * queue closed  -> zombie           (recovery)
 * queue closed  -> purgatory        (drop) #
 * logout        -> zombie           (recovery or got logout resp)
 * logout        -> purgatory        (drop) #
 * nap           -> connecting       (timeout)
 * nap           -> purgatory        (drop) #
 * zombie        -> free             (!active) #
 * zombie        -> nap              (recovery & not had logout ioctl)
 * zombie        -> purgatory        (drop) #
 * purgatory     -> zombie           (timeout)
 *
 * When we want to log a connection in or out we simply set the appropriate
 * event flag. The work is then done by the session tx thread. To enable
 * ioctls to wait on these async operations each connection has a semaphore
 * (ioctl_wait). This is signalled when the state transitions marked # in the
 * table above occur (if currently waiting).
 *
 * For this system to work there must never be 2 or more simultaneous ioctls
 * attempting to login / logout the same connection. It is up to the upper
 * level to ensure this.
 */
typedef struct ci_iscsi_connection_s {
  /* Some state for the socket for this connection */
  tips_socket            ep;  /* handle for kernel-created socket */
  volatile int socket_live;  /* Is this connections socket alive */
  ci_uint16 portalGroup;     /* Portal group, as specified by target */

  struct ci_iscsi_session_s *session;  /* Session we belong to */
  unsigned int is_leading;    /* 1 if this is leading connection, 0 if not */

/* Connection states */
#define CONN_STATE_FREE           0
#define CONN_STATE_CONNECTING_1   1
#define CONN_STATE_CONNECTING_2   2
#define CONN_STATE_ACTIVE         3
#define CONN_STATE_Q_CLOSED       4
#define CONN_STATE_LOGOUT         5
#define CONN_STATE_NAP            6
#define CONN_STATE_ZOMBIE         7
#define CONN_STATE_PURGATORY      8
#define CONN_STATE_MAX            8  /* for range checking */
  ci_uint32 state;

  /* State event flags */
  volatile ci_uint32 event_flags;

  ci_uint8 reason_code;  /* reason for logging out */

#define CI_ISCSI_STAGE_SECURITY  0   /* RFC3720 10.12.3 */
#define CI_ISCSI_STAGE_LOGIN     1   /* RFC3720 10.12.3 */
#define CI_ISCSI_STAGE_FFP       3   /* RFC3720 10.12.3 */
  unsigned int our_csg;           /* control sets this for login request */
  unsigned int our_nsg;           /* control sets this for login request */
  unsigned int their_csg;         /* core sets this from login response */
  unsigned int their_nsg;         /* core sets this from login response */
  volatile ci_uint32 queued_keys; /* one bit set for each key we want to send */
  ci_uint32 sent_keys;            /* one bit set for each key we've sent */
  ci_uint32 rcvd_keys;            /* one bit set for each key we've received */
  ci_uint32 irrel_keys;           /* and one for each recv'd as "Irrelevant" */
  ci_iscsi_connection_params params;         /* post negotiation */
  ci_iscsi_connection_params params_wanted;  /* pre negotiation */

  /* The last PDU received on this connection */
  struct iscsi_cmd_rsp_pdu pdu;

  /* Is this connection expecting a PDU or data next?  If data, then
   * the awaiting_pdu_dlen field contains how much data we're awaiting
   * (including padding and, if necessary, digest) and the pdu field
   * contains the PDU whose data we're waiting for.
   */
  ci_uint32 awaiting_pdu_dlen;

  /* When non-NULL indicates that received data is part of the payload
   * of a command.  This command has this connection's rx callback
   * reference taken.  This reference needs releasing if the rx
   * callback is disarmed.
   */
  struct command_args *rx_data_command;

  /* Did the hardware indicate digest OK for this PDU? */
  ci_uint32    rx_data_hw_digest_ok;

  /* The partial CRC as calculated so far. */
  ci_uint32    rx_data_crc;

  /* The current offset into the SCSI buffer. */
  ci_uint32    rx_data_offset;

  /* The number of padding/crc bytes after the payload. */
  ci_uint32    rx_data_extra_len;

  /* Used for synchronous transmit, and for reads.  There can be two entries
   * per scatter-gather list entry since each can easily cross 2 pages.  To
   * get another entry here needs an entry to cross another page which means
   * that it contains another page of data.  The iov array requires 3 extra
   * entries for the pdu, padding and data digest. */
  void *  bases [CI_ISCSI_IOV_SIZE];

  ci_iovec     iov [CI_ISCSI_IOV_SIZE + 3];

  /* Note that the extra 4 bytes are for a possible data digest.  This
   * can contain one of:
   *    SCSI Response   - SCSI sense data + vendor specific
   *    TMFunc Response - None
   *    R2T             - None
   *    Async Message   - SCSI sense data + vendor specific
   *    Text Response   - Text keys
   *    Login Response  - Text keys
   *    Logout Response - None
   *    Reject          - PDU Header + vendor specific
   *    NOP-In          - Returned ping data
   * This is therefore limited to 100 bytes of useful data.
   */
  ci_uint8         rx_data[MAX_RECV_DSL + 4];

  /* Copy buffer for transmitted data when data digests are enabled.  We can't
   * calculate the digest until we've copied the data, as it may change under
   * our feet before we pass to the IP stack.  TODO: Yuck...
   */
  ci_uint8 *tx_copy_buffer;         /* points to a kmalloc()ed block */
  ci_uint32 our_max_send_data_segment_length;  /* length of tx_copy_buffer */

  /* Flags used when sending data, used for iSCSI digest offload with Falcon.
   */
  ci_uint32 send_flags;

  /* Last error that occurred on this connection (which may be propagated back
   * to userland e.g. as the result of a control op ioctl), and an iSCSI
   * result code (which may also be propagated back) which should contain a
   * CI_ISCSI_RESULT_xxx code -- either the generic CI_ISCSI_RESULT_ERROR or
   * or CI_ISCSI_RESULT_SUCCESS, or the value corresponding to the Status-Class
   * and Status-Details fields from the Login Response PDU.
   */
  int last_errno; 
  ci_uint32 iscsi_result;

  /* The number of commands currently outstanding on this connection. The msb
   * is a flag which disallows adding any more commands to this connection.
   * This is set when we want to log out the connection.
   */
#define Q_CLOSED 0x80000000
  ci_uint32    command_count;

  /* The number of tasks we currently have outstanding.
   * Possible tasks are:
   *   R2T responses
   *   Text negotiation requests
   *   NOP response
   *   login requests
   *   logout requests
   */
  ci_atomic_t pending_tasks;

  int ExpStatSN_valid;
  ci_uint32    ExpStatSN;

  char *key_buffer;        /* buffer used for key exchanges */
  size_t key_buffer_len;   /* allocated length of above buffer */
  size_t key_buffer_used;  /* Amount of buffer filled. */

  int next_free;  /* index of next free connection struct in session */

#define NEGOTIATION_STATE_IDLE       0
#define NEGOTIATION_STATE_RECV       1
#define NEGOTIATION_STATE_SEND_DATA  2
#define NEGOTIATION_STATE_SEND_ACK   3
  volatile unsigned int negotiation_state; /* The negotiation state machine */
  ci_iscsi_lun negotiation_LUN;
  ci_uint32    negotiation_TTT;
  ci_uint32    negotiation_ITT; /* Set if negotiation_TTT != 0xffffffff */

#define NOP_STATE_IDLE               0
#define NOP_STATE_SEND               1
  ci_irqlock_t nop_lock;
  unsigned int nop_state;
  ci_iscsi_lun nop_LUN;
  ci_uint32    nop_TTT;

  /* Timeout for control operations */
  ci_timer     inactivity_timer;  /* Timer to spot logout inactivity */
  ci_timer     drop_timer;        /* Timer for recovery and drop timeouts */
  ci_uint32    drop_timeout;      /* Time to use for drop_timer */
  ci_uint32    timers_inited;     /* Have the timers been initialised */

  /* Statistics counters */
  ci_uint64    stats[CI_ISCSI_STATS_N];  /* Current connection counters */

  ci_boolean_t   is_authorised;

  /* The 2 pdus below total 104 bytes. Moved from sis_login code to be sociable  */
  ci_iscsi_login_pdu login_resp;
  ci_iscsi_login_pdu login_req;

  /* The chap_keys structure used to be alloc'd in-line. Moved into per-conn
   * structure as part of storport development where it's useful to have it
   * retained over the several steps of the CHAP exchange process.
   */
  ci_iscsi_chap_keys chap_keys;
  ci_uint32          keys_wanted;

#if CI_ISCSI_DEBUG_FAIL_TEST
  /* See interface.h for details */
  ci_uint32    fail_op;        /* The failure we're going to fake */
  ci_uint32    fail_param;     /* Test dependant parameter */
#endif

#ifndef NDEBUG
  int          rx_busy;        /* Used only for debug */
#endif

  /* System specific fields */

#if defined(__ci_wdm__) || defined(__ci_wdf__)
  USHORT src_port;       /* Local port number */

  ULONG src_ip_address;  /* Local IP address */
  ci_iscsi_wmi_req *request;  /* Active wmi request, if any */
  ULONG originalTarget;  /* Target address before redirection */
  ULONG originalPort;    /* Target port before redirection */
  UCHAR redirection;     /* Type of redirection (a CI_ISCSI_REDIRECT_ value) */
  ULONG pathWeight;      /* Load balancing info */
  ULONG primary;         /* Load balancing info */
  HW_DEVICE_EXTENSION *devEx; /* Windows device extension */

  ULONG LUN;  /* Temporary hack */

#elif defined __linux__

  /* We use a semaphore to allow ioctls to block until operations performed
   * by the tx thread are complete.
   */
  ci_semaphore_t ioctl_wait;

#elif defined __ci_storport__

/*! We use the per session scsi_handle to locate our device extension. */
  struct ci_iscsi_cmd_info * ci_iscsi_cmd_info;
  USHORT src_port;       /* Local port number */

 /*
  * When incoming data arrives, in stor_iscsi.c we plant it's address and
  * length here so that storport has access to resonse data as it steps 
  * through its 'state processing'.
  */
  PUCHAR pRawData;
  INT    nRawDataLen;

#else
# error Unsupported platform
#endif
} ci_iscsi_connection;


#define CI_ISCSI_MAX_ASYNC_HDRS     32

#if CI_ISCSI_COUNT_DATA_LENGTHS

typedef struct ci_iscsi_dlen_count_s {
  ci_uint32 zero;
  ci_uint32 lt512;
  ci_uint32 lt512_pad;
  struct {
    ci_uint32 eq;
    ci_uint32 gt;
    ci_uint32 gt_pad;
  } p2[8];  /* power of 2 lengths: 0=>512 .. 7=>64K */
} ci_iscsi_dlen_count;

extern void ci_iscsi_count_dlen(ci_iscsi_dlen_count *count, ci_uint32 dlen);

#define ISCSI_COUNT_DATA_IN(sess,dlen) \
  ci_iscsi_count_dlen(&sess->data_in_count, dlen)
#define ISCSI_COUNT_DATA_OUT(sess,dlen) \
  ci_iscsi_count_dlen(&sess->data_out_count, dlen)

#else

#define ISCSI_COUNT_DATA_IN(sess,dlen)
#define ISCSI_COUNT_DATA_OUT(sess,dlen)

#endif



/* LUN mapping table.
 * Note that we store 64 bit LUNs as network (ie big) endian.
 */
#define LUN_MAP_SIZE 255

typedef struct ci_iscsi_lun_map {
#ifdef __ci_storport__
  ULONGLONG value[LUN_MAP_SIZE];  /* iSCSI LUN value */
  BOOLEAN valid[LUN_MAP_SIZE];    /* is value set */
  ULONG bus;     /* LUN device specification */
  ULONG target;  /* " */
#else
  uint64_t value[LUN_MAP_SIZE];  /* iSCSI LUN value */
  int  valid[LUN_MAP_SIZE];    /* is value set */
  unsigned long bus;     /* LUN device specification */
  unsigned long target;  /* " */
#endif
} ci_iscsi_lun_map;

typedef void * HW_DEVICE_EXTENSION;

/* Session (one of these per SCSI device)
 */
typedef struct ci_iscsi_session_s {
  int  id;   /* This session's index into the array of sessions, or index of
                the next free session if this session is on the free-list */

  ci_iscsi_scsi_handle scsi_handle;  /* For SCSI driver's private use */

  ci_iscsi_session_params params;         /* post negotiation */
  ci_iscsi_session_params params_wanted;  /* pre negotiation */

#define CI_ISCSI_ISID_LEN 6
  char ISID[CI_ISCSI_ISID_LEN];

  int  last_errno;                      /* Any errors on this session */

#ifndef __ci_storport__
  cithread_t tx_thread;
  
  /* Right now we send and receive data using separate kernel threads.  To
   * synchronize the send thread with the code that calls in from the OS SCSI
   * layer, we use a semaphore (i.e. each time a command is sent the semaphore is
   * up-ed, and each time we actually send a command, the send thread downs it.
   * This is bad because we've got a Linux dependency in supposedly OS-neutral
   * part; however, it's only a temporary arrangement.
   * TODO [OPT2]: don't use separate send/receive threads with semaphore.  Rather
   *              send/recv using async non-blocking mechanisms.
   */
  ci_semaphore_t command_sem;
#endif

  /* CI_TRUE to request recovery.  The command semaphore must be up-ed to ensure
   * anything happens (recovery happens in the context of the tx thread) */
  ci_boolean_t   need_recovery; 

  /* CI_TRUE if recovery is currently happening.  Only change this with the
   * am_recovery_mutex held.
   */
  ci_boolean_t   am_recovering;
  ci_semaphore_t am_recovering_mutex;
  
  /* CI_TRUE if timeouts from the SCSI layer should be ignored */
  ci_boolean_t   retry_timed_out_commands;
  /* Flag to prevent excessive logging of timeout conditions */
  ci_boolean_t   scsi_timeouts_logged_flag;

  /* The async stuff needs to send args to the tx thread.  We support
   * QUEUE_SIZE concurrent outsanding commands.
   * On queue_command we write an entry into this array, and add it to the
   * pending FIFO (implemented by 2 stacks). When we've finished
   * with a command it's put back into the free list.
   * We index the array with the command ITT mod the list size. Each time we
   * add a command to the pending FIFO we assign it the ITT of the previous
   * command in that slot plus the queue size, modulo 2^31 (assuming the queue
   * size is a power of 2). Thus we will have atleast 2^31 / queue size
   * commands before we reuse an ITT.
   */
  struct command_args {
    struct ci_iscsi_session_s *sess;
    struct ci_iscsi_connection_s *conn;
    ci_iscsi_cmd_handle handle;
    char    cdb[16];
    size_t  cmd_len;
    int     direction;
    ci_iscsi_lun LUN;

    /* This member gives a set of reasons why the command cannot be
     * finished.  Whoever clears the last bit is responsible for calling
     * the completion routine and returning the command to the free list.
     * Getting a reference fails if this value is already zero.  While one
     * of these bits is set, we can be sure that the command won't be
     * reused, so the ITT is constant.
     */
#define CMD_REF_NOT_SENT   (1<<0)       /* Still things to send. */
#define CMD_REF_NO_RESULT  (1<<1)       /* The result hasn't arrived yet. */
#define CMD_REF_TX_THREAD  (1<<2)       /* For synchronous sends. */
#define CMD_REF_RECOVERY   (1<<3)       /* For recovery. */
#define CMD_REF_PURGE      (1<<4)       /* Purge commands at session close. */
#define CMD_REF_LU_RESET   (1<<5)       /* Windows LU reset debugging */
#define CMD_REF_RX(N)     ((1<<8)<<(N)) /* One bit per connection */
    int cmd_references;

    /* Total bytes for this command.
     * TODO: Remove this: redundnant with the two fields below
     */
    ci_uint32    buffer_len;

    /* We store the TTT for the current burst, plus the limit of the burst.
     * A TTT of 0xFFFFFFFF means this burst is unsolicited.
     * TODO [R2Ts] Currently this is sufficient because MaxOutstandingR2Ts
     * is always 1, and we don't support out-of-order data.  In order to
     * support >1 R2Ts outstanding we'll just need a queue of "burst" structs.
     */
    struct {
      ci_uint32 xfer_len;  /* len of this burst still to send */
      ci_uint32 TTT;       /* -1 if unsolicited, else TTT from busrt's R2T */
      ci_uint32 offset;    /* Redundant til we support out-of-order data */
      /* Next DataSN to use.  0xFFFFFFFF means the SCSI command PDU for this
       * command hasn't yet been sent.  (Note: theoretically 0xFFFFFFFF is a
       * valid DataSN, but in practice impossible since it would imply a
       * DataSegmentLength of 1)
       */
      ci_uint32 DataSN;
    } current_burst;

    /* Space to build a tx PDU for this command
     * TODO: allocate from hdrs array
     */
    ci_iscsi_cmd_pdu tx_pdu;

    ci_uint32 next;  /* we keep linked lists of free and pending commands */
    int       ITT;   /* needed to ensure this is still the right command */

    /* For async tx we construct headers for the command.  Since we use fire-
     * and-forget w/o AOI completion, these headers don't get 'freed' until
     * the command is completed and returned to the command free-list.
     * If we run out of these headers, the async-tx fails, and falls back to
     * the tx-thread.  Suboptimal use of memory and cpu, but correct.
     * TODO [HDR_CPY]: when async IO copies headers properly remove this hack
     * and forge the headers on the stack.
     */
    ci_iscsi_pdu   hdrs [CI_ISCSI_MAX_ASYNC_HDRS];
    int            hdrs_used;

    /* The current location in the scatter-gather list.  Use for rx or
     * tx depending on the direction of the command. */
    struct         memdesc_cursor cursor;

    /* The number of times the SCSI layer has tried to time out this command */
    unsigned int   timeouts;

  } command_queue [QUEUE_SIZE];

  /* Free command list head pointer is an (index, generation counter) tuple.
   * Top 16 bits are counter, bottom 16 are head index.
   * COMM_LIST_END is used as the index for an empty list.
   */
  /* Do we still need this concurrency protection? No point removing it now
   * it's in, but we should state here if it's not needed - aam
   */
#define COMM_LIST_END             0xFFFF
#define COMM_LIST_INDEX_MASK      0xFFFF
#define COMM_LIST_COUNT_MASK  0xFFFF0000
#define COMM_LIST_COUNT_SHIFT         16
#define COMM_LIST_COUNTER_INC    0x10000
  ci_uint32 free_command_list;

  iscsi_fifo_t unassigned_cmds_pending;
  iscsi_fifo_t assigned_cmds_pending;

  /* We use a separate space for ITTs for special commands such as logins.
   * This doesn't need any guarding since it's only used by the tx thread,
   * of which there is only one.
   */
  ci_uint32 next_special_ITT;

  int configured_conns; /* may not equal no. of active conns during recovery */
  ci_iscsi_connection connections[CI_ISCSI_MAX_CONNS_PER_SESS];
  int free_conn_list;

  /* State for connection selection, eg round-robin */
  int current_conn;

  ci_uint16 tsih;                 /* See TSIH in the RFC */
  ci_uint32 MaxCmdSN;             /* The max CmdSN we're allowed to use */
  ci_uint32 ExpCmdSN;             /* The lowest CmdSN the target expects */
  ci_uint32 current_CmdSN;

  /* When a command gets aborted by the SCSI layer, we need to make
   * sure that the command isn't being completed at the same time.
   * The command structure is returned to the free list with this lock
   * held, so the command can be aborted with this lock held.
   */
  ci_irqlock_t completion_lock;

  /* The abort code needs to wait for recovery and recovery should not start
   * during an abort. We use abort_state to keep track of our state:
   * none     - no abort in progress
   * aborting - do not start recovery, do not connect any sockets
   * wake     - abort needs to be woken when recovery occurs, by raising the
   *            semaphore
   */
  ci_semaphore_t abort_wakeup;
  volatile int abort_state;
#define ABORT_NONE     0
#define ABORT_ABORTING 1
#define ABORT_WAKE     2

  /* This protects the command sequencing. */
  ci_semaphore_t tx_mutex;

  /* Statistics counters */
  ci_uint64 stats[CI_ISCSI_STATS_N];  /* Counters for previous connections */

  /* We periodically attempt to reconnect connections that have ended up
   * Zombied, but never told to logout. This happens, for example, if a
   * recovery fails to log back in for any reason.
   */
  ci_timer zombie_timer;          /* Zombie reconnection timer */
  ci_boolean_t zombie_reconnect;  /* Zombie reconnection flag */
  ci_uint32    timers_inited;     /* Have the timers been initialised */

#ifndef __ci_storport__
# if CI_ISCSI_TXTHREAD_DEBUG
  /* Tx thread debugging */
  ci_uint64 t_txthr_sleep;    /* last time we slept */
  ci_uint64 t_txthr_wake;     /* last time we woke up */
  ci_uint64 t_txthr_lock1;    /* last time we obtained lock (1st time) */
  ci_uint64 t_txthr_dropconn; /* last time we dropped a connection */
  ci_uint64 t_txthr_unlock1;  /* last time we released lock (1st time) */
  ci_uint64 t_txthr_recovery; /* last time we performed a pending recovery */
  ci_uint64 t_txthr_zombie;   /* last time we checked for zombie reconnection */
  ci_uint64 t_txthr_lock2;    /* last time we obtained lock (2nd time) */
  ci_uint64 t_txthr_task;     /* last time we noticed a pending task */
  ci_uint64 t_txthr_assign;   /* last time we assigned a command */
  ci_uint64 t_txthr_r2t;      /* last time we serviced an R2T */
  ci_uint64 t_txthr_cmdpend;  /* last time we sent a pending command */
  ci_uint64 t_txthr_unlock2;  /* last time we released lock (2nd time) */
  ci_uint64 t_abort;          /* time when abort triggered */
# endif
#endif

#if CI_ISCSI_COUNT_SCSI_COMMANDS
  ci_uint32 scsi_cmd_count[256];
#endif

#if CI_ISCSI_COUNT_DATA_LENGTHS
  ci_iscsi_dlen_count data_out_count;
  ci_iscsi_dlen_count data_in_count;
#endif

  /* Each session can have a single send targets active at a time.
   * Requests are set up by the SCSI layer, and sent by the session TX thread.
   * The rx callback then tidies up.
   *
   * This is all synchronised by a state variable. The valid states are:
   *   none     - no request in progress
   *   reserved - request has been reserved by system call etc, and is being
   *              set up
   *   ready    - request is ready to be sent by the session TX thread
   *   sent     - request has been sent and is awaiting a response
   *   ceased   - session has stopped operation
   *
   * Most of these transitions have potential contentions due to the "ceased"
   * state. To counter this, transitions should be performed atomically
   * using a casl.
   */
#define TM_STATE_NONE     0
#define TM_STATE_RESERVED 1
#define TM_STATE_READY    2
#define TM_STATE_SENT     3
#define TM_STATE_CEASED   4

  struct {
    int state;
    char *data;
    ci_uint32 dataSize;
    ci_uint32 sizeWanted;
    ci_iscsi_connection *conn;
#if defined(__ci_wdm__) || defined(__ci_wdf__)
    ci_iscsi_wmi_req *request;
    unsigned char target[228];  /* incl space for terminations and padding */
#endif
  } st_req;

  /* System dependant fields */
#if defined(__ci_wdm__) || defined(__ci_wdf__)
  BOOLEAN PersistentLogin;
  ULONG LBPolicy;
  ULONG TargetPortalGroupTag;
  HW_DEVICE_EXTENSION *devEx;  /* Windows device extension */
  ci_iscsi_lun_map luns;
#endif

#if defined(__ci_storport__) || defined(__ci_wdm__) || defined(__ci_wdf__)
  /* Each session can have a single task management function active at a time.
   *
   * The mechanism used is exactly as for task management functions described
   * above.
   *
   * Note that this is currently only implemented for Windows.
   */
  struct {
    int state;
    ci_uint8 function;
    ci_iscsi_lun lun;
    ci_uint32 ITT;
    ci_iscsi_tm_handle *handle;
  } tm_req;
#endif

} ci_iscsi_session;


ci_inline void
tx_lock (ci_iscsi_session *sess) {
  ci_sem_down (&sess->tx_mutex);
}

ci_inline void
tx_unlock (ci_iscsi_session *sess) {
  ci_assert (ci_sem_trydown (&sess->tx_mutex));
  ci_sem_up (&sess->tx_mutex);
}

ci_inline int
tx_is_locked (ci_iscsi_session *sess) {
  if (ci_sem_trydown (&sess->tx_mutex))
    return 1;
  ci_sem_up (&sess->tx_mutex);
  return 0;
}
  
ci_inline int
tx_trylock (ci_iscsi_session *sess) {
  return ci_sem_trydown (&sess->tx_mutex);
}



/* ============================================================================
 *  Global data
 * ============================================================================
 */

extern ci_iscsi_session ci_iscsi_sessions[CI_ISCSI_MAX_SESSIONS];

extern ci_uint64 ci_iscsi_stats_global[CI_ISCSI_STATS_N];



/* ============================================================================
 *  Internal functions
 * ============================================================================
 */


/* Helpers for grabbing netif / tcp state / tcp helper resource from connection
 */
ci_inline tips_stack*
CONN_NI(ci_iscsi_connection *conn)
{
  ci_assert (conn);
  ci_assert (TIPS_SOCKET_STACK(&conn->ep));
  return TIPS_SOCKET_STACK(&conn->ep);
}

ci_inline tips_tcp_socket*
CONN_TS(ci_iscsi_connection *conn)
{
  ci_assert (conn);
  ci_assert (conn->ep.s);
  return TIPS_SOCKET_TCP_SOCKET(&conn->ep);
}


#ifdef __ci_storport__
ci_inline tcp_helper_resource_t*
CONN_TRS(ci_iscsi_connection *conn)
{
                //  Needs abstracting ??
  return NULL;  // "tcp_helper_resource_t" is an onload artefact.
}
#else
ci_inline tcp_helper_resource_t*
CONN_TRS(ci_iscsi_connection *conn)
{
  ci_assert (conn);
  ci_assert (conn->ep.netif);
  ci_assert (netif2tcp_helper_resource((conn)->ep.netif));

  return netif2tcp_helper_resource((conn)->ep.netif);
}
#endif


/* Get the connection ID from a connection struct.
 */
ci_inline int CONN_ID(ci_iscsi_connection *conn)
{
  int conn_id;

  ci_assert(conn!=NULL);
  ci_assert(conn->session!=NULL);

  conn_id = (int)(conn - conn->session->connections);
  
  ci_assert_ge(conn_id, 0);
  ci_assert_lt(conn_id, CI_ISCSI_MAX_CONNS_PER_SESS);

  return conn_id;
}

/*! Test whether a session is currently allocated
 * \param   sessId       id of session to test
 * \returns true if connection is allocated, false otherwise
 */
ci_inline int ci_iscsi_sess_alloced(int sessId)
{
  ci_assert_ge(sessId, 0);
  ci_assert_lt(sessId, CI_ISCSI_MAX_SESSIONS);
  return ci_iscsi_sessions[sessId].id==sessId;
}

/*! Test whether a connection is currently allocated
 * \param   sessId       id of session connection is in
 * \param   connId       id of connection to test
 * \returns true if connection is allocated, false otherwise
 */
ci_inline int ci_iscsi_conn_alloced(int sessId, int connId)
{
  ci_assert_ge(sessId, 0);
  ci_assert_lt(sessId, CI_ISCSI_MAX_SESSIONS);
  ci_assert_equal(ci_iscsi_sessions[sessId].id, sessId);
  ci_assert_ge(connId, 0);
  ci_assert_lt(connId, CI_ISCSI_MAX_CONNS_PER_SESS);
  return ci_iscsi_sessions[sessId].connections[connId].next_free==connId;
}

/* Get the session ID from a sess pointer.
 */
ci_inline int SESS_ID(ci_iscsi_session *sess)
{
  return (int)(sess - ci_iscsi_sessions);
}

/* Session / connection validity
 */
#ifndef NDEBUG

extern void ci_iscsi_assert_valid_sess(ci_iscsi_session *sess,
                                       const char *file, int line);
extern void ci_iscsi_assert_valid_conn(ci_iscsi_connection *conn,
                                       const char *file, int line);
extern void ci_iscsi_assert_valid_sess_conn(ci_iscsi_session *sess,
                                            ci_iscsi_connection *conn,
                                            const char *file, int line);

#define ASSERT_VALID_SESS(sess) \
          ci_iscsi_assert_valid_sess(sess, __FILE__, __LINE__)
#define ASSERT_VALID_CONN(conn) \
          ci_iscsi_assert_valid_conn(conn, __FILE__, __LINE__)
#define ASSERT_VALID_SESS_CONN(sess, conn) \
          ci_iscsi_assert_valid_sess_conn(sess, conn, __FILE__, __LINE__)

#else

#define ASSERT_VALID_SESS(sess)            (void)0
#define ASSERT_VALID_CONN(sess)            (void)0
#define ASSERT_VALID_SESS_CONN(sess,conn)  (void)0

#endif



#ifdef __ci_storport__
#define CI_MEM_DESC_TYPE_UNUSED      0
#define CI_MEM_DESC_TYPE_SCATTERLIST 1
#define CI_MEM_DESC_TYPE_POINTER     2
#define CI_MEM_DESC_TYPE_IMMEDIATE   3

/* In the following, length means either:
 - how long the scatterlist is 
 - how long the buffer pointed to by ptr is.
 - unused for the "immediate" type
*/
struct ci_mem_desc {
  int type;
  unsigned length;
  union {
    ci_os_scatterlist_t *scatter;
    void* ptr;
    unsigned immediate;
  } mem;
};

#endif


/* Handle "serial number arithmetic" for sequence numbers
 */
#define SN_EQ(sn1, sn2)   ((ci_uint32)((sn1) - (sn2)) == 0u)
#define SN_LT(sn1, sn2)   ((ci_int32)((sn1) - (sn2)) < 0)
#define SN_LE(sn1, sn2)   ((ci_int32)((sn1) - (sn2)) <= 0)
#define SN_GT(sn1, sn2)   ((ci_int32)((sn1) - (sn2)) > 0)
#define SN_GE(sn1, sn2)   ((ci_int32)((sn1) - (sn2)) >= 0)
#define SN_SUB(sn1, sn2)  ((ci_int32)((sn1) - (sn2)))


typedef enum {
  ISCSI_ACTION_DONE,
  ISCSI_ACTION_SOFT_ERROR,
  ISCSI_ACTION_HARD_ERROR
} ci_iscsi_action_code;


typedef enum {
  key_HeaderDigest,
  key_DataDigest,
  key_MaxConnections,
  key_SendTargets,
  key_TargetName,
  key_InitiatorName,
  key_TargetAlias,
  key_InitiatorAlias,
  key_TargetAddress,
  key_TargetPortalGroupTag,
  key_InitialR2T,
  key_ImmediateData,
  key_MaxRecvDataSegmentLength,
  key_MaxBurstLength,
  key_FirstBurstLength,
  key_DefaultTime2Wait,
  key_DefaultTime2Retain,
  key_MaxOutstandingR2T,
  key_DataPDUInOrder,
  key_DataSequenceInOrder,
  key_ErrorRecoveryLevel,
  key_SessionType,
  key_AuthMethod,
  key_OFMarker,
  key_IFMarker,
  key_OFMarkInt,
  key_IFMarkInt,
  /* CHAP keys are sent multiple times and so are handled separately */
  key_NUMBER_OF_KEYS

} ci_iscsi_text_key;


/* Character signedness.
 *
 * To keep internal consistency, and be nice to fussy compilers, we use
 * "char" when talking about iSCSI keys and all things human readable text
 * like, and "ci_uint8" for binary data, eg for authentication.
 */

/* Send login keys and get response. In and out buffers may be the same.
** out_len inputs max size of out_data and outputs size used.
**/
extern int ci_iscsi_login_key_exchange(ci_iscsi_session *sess,
				       ci_iscsi_connection *conn,
				       char *in_data, size_t in_len,
				       char *out_data, size_t *out_len);

/* Send text keys and get response. In and out buffers may be the same.
** out_len inputs max size of out_data and outputs size used.
**/
extern int ci_iscsi_text_key_exchange(ci_iscsi_session *sess,
				      ci_iscsi_connection *conn,
				      char *in_data, size_t in_len,
				      char *out_data, size_t *out_len);
/* Get key response.
** out_len inputs max size of out_data and outputs size used.
**/
extern int ci_iscsi_get_key_response(ci_iscsi_connection *conn,
                                     ci_iscsi_text_pdu *ack_header,
                                     int exp_opcode, ci_iscsi_text_pdu *out_header,
                                     char *out_data, size_t *out_len);

/*! Complete the send targets request on the given session
 * \param   sess               session request is on
 * \param   response           request error response
 */
void ci_iscsi_st_complete(ci_iscsi_session *sess, int response);

extern int ci_iscsi_login_keys_request(ci_iscsi_session *sess,
				       ci_iscsi_connection *conn);

extern int ci_iscsi_keys_parse_response(ci_iscsi_session *sess,
                                        ci_iscsi_connection *conn,
                                        char *data, size_t len);
extern int ci_iscsi_keys_verify(ci_iscsi_session *sess,
                                ci_iscsi_connection *conn);

extern void ci_iscsi_process_in_text_keys(ci_iscsi_connection *conn, int final);
extern void ci_iscsi_prepare_out_keys(ci_iscsi_session *sess,
				      ci_iscsi_connection *conn);

extern int ci_iscsi_conn_start_rx(ci_iscsi_connection *conn, ci_int32 rcvlowat);
extern int ci_iscsi_conn_stop_rx(ci_iscsi_connection *conn);
extern int ci_iscsi_conn_close(ci_iscsi_connection *conn);
extern int ci_iscsi_conn_abort(ci_iscsi_connection *conn,
                               ci_boolean_t netif_locked);
extern void send_logout_req(ci_iscsi_session *sess,ci_iscsi_connection *conn,
			    int reason_code);

/* Tell SCSI a command is finished and return its structure to the free list */
extern void finish_command(ci_iscsi_session *sess, struct command_args *command);

extern void ci_iscsi_put_command_reference(ci_iscsi_session *sess,
                                           struct command_args *command,
                                           int reference);
extern ci_boolean_t ci_iscsi_get_command_reference(ci_iscsi_session *sess,
                                                   struct command_args *command,
                                                   int reference);

extern void ci_iscsi_set_command_response(ci_iscsi_session *sess,
                                          struct command_args *command,
                                          size_t residual_count,
                                          ci_iscsi_action_code action,
                                          unsigned int status);

#ifdef _WIN32
/*! Add a task management request to our queue.
 * 
 * param   sess               session to send request over
 * param   function           task management function to send
 * param   LUN                LUN for functions that require it
 * param   handle             request handle
 * returns error code
 */
extern int
ci_iscsi_add_task_management_req(ci_iscsi_session *sess,
                                 ci_uint8 function,
                                 ci_iscsi_lun LUN,
                                 ci_iscsi_tm_handle *handle);
#endif

/* Result functions for parameter negotiation */
#define CI_ISCSI_RESULT_USE  0    /* use whatever target specifies         */
#define CI_ISCSI_RESULT_MIN  1    /* use minimum of our value and target's */
#define CI_ISCSI_RESULT_MAX  2    /* use maximum of our value and target's */
#define CI_ISCSI_RESULT_AND  3    /* use AND of our value and target's     */
#define CI_ISCSI_RESULT_OR   4    /* use OR of our value and target's      */

extern ci_boolean_t ci_iscsi_keys_parse_int(const char *key, const char *str,
                                            unsigned int *pval,
                                            unsigned int resultfn,
                                            unsigned int min_allowed,
                                            unsigned int max_allowed);

/* Parse an arbitrarilly large value encoded as hex or base 64 [rfc 2045].
 * We put the answer into a byte buffer provided by the caller, and report back
 * the number of bytes used. Note that data is in network order, ie most
 * significant byte at start of buffer.
 */
extern ci_boolean_t ci_iscsi_keys_parse_big_value(const char *key,
                                                  const char *str,
                                                  ci_uint8 *pbuf,
                                                  ci_uint32 max_len,
                                                  size_t *plen);

/* tx thread method */
extern void *ci_iscsi_sender (void *arg);

/* Front end to recovery for error level 0, called when it's all gone wrong.
 * May be called from any context, does not block or take locks.
 */
extern void recover0(ci_iscsi_session *sess);

/* The working part of recovery for error level 0.
 * May only be called from a session TX thread
 * Does nothing and returns -EBUSY if it cannot atomically set the recovery
 * state to REC_NEEDED, otherwise returns 0
 */
extern void do_recovery0(ci_iscsi_session *sess);

/* The working part of logout. May only be called from a session TX thread */
extern void ci_iscsi_conn_do_logout(ci_iscsi_session *sess,
                                    ci_iscsi_connection *conn);

/* Stop a connection and abort any commands assigned to it at the SCSI
 * layer.  Must be called with the session tx_lock held.  Need to call
 * ci_iscsi_conn_close if the socket is to be closed.  May only be
 * called from a session TX thread.
 */
extern void halt_conn(ci_iscsi_session *sess, ci_iscsi_connection *conn);

/* Allocate a special ITT. Must only be called from the tx thread */
extern int get_special_ITT(ci_iscsi_session *sess);


/* ============================================================================
 *  Startup/shutdown functions
 * ============================================================================
 */

/* iSCSI startup */
extern int ci_iscsi_ctor(void);

/* iSCSI cleanup */
extern void ci_iscsi_dtor(void);


/* ============================================================================
 *  CHAP functions
 * ============================================================================
 */

/* Perform CHAP authentication */
extern int ci_iscsi_chap(ci_iscsi_session *sess, ci_iscsi_connection *conn);

/* Setup the initiator secret */
extern int ci_iscsi_chap_set_secret(char *name, ci_uint32 secret_len,
                                    ci_uint8 *secret);

/* Empty our secrets dictionary */
extern void ci_iscsi_delete_secrets(void);


/* ============================================================================
 *  Socket interface
 * ============================================================================
 */

extern int  ci_iscsi_socket_resources_ctor(unsigned cpu_khz);
extern void ci_iscsi_socket_resources_dtor(void);
extern ci_boolean_t ci_iscsi_socket_netif_ready(void);
extern tips_stack *ci_iscsi_socket_get_stack(void);
extern int ci_iscsi_socket_create(tips_socket  *ep);
extern int ci_iscsi_socket_set_timeouts(tips_socket  *ep, ci_uint32 timeout);
extern int ci_iscsi_socket_connect(tips_socket  *ep, ci_uint32 ip_addr,
                                   ci_uint16 port
                                   CI_ARG_WIN(ci_uint16 localPort));
extern int ci_iscsi_socket_configure_digest_offload(tips_socket  *ep,
                                                    ci_boolean_t header_digest,
                                                    ci_boolean_t data_digest);
extern int  ci_iscsi_socket_close(tips_socket  *ep);
extern int  ci_iscsi_socket_destroy(tips_socket  *ep);

extern int ci_iscsi_socket_create_udp(tips_socket  *ep);
extern int  ci_iscsi_socket_destroy_udp(tips_socket  *ep);


/* ============================================================================
 *  CRC32C
 * ============================================================================
 */

extern size_t ci_iscsi_crc32c_iovec_insert(ci_iovec *iov, int iovlen);
extern size_t ci_iscsi_crc32c_iovec_tx_copy(ci_iscsi_connection *conn,
                                            ci_iovec *iov, int iovlen);


/* This is the value we test against on receive; calculating the CRC over the
 * header/data and the digest word should result in this value.  (It is the
 * polynomial form remainder 0x1c2d19ed which is specified in RFC 3270 section
 * 12.1, complemented and bit-reversed.)
 */
#define CI_ISCSI_CRC32C_OK_MAGIC_VALUE 0x48674bc7


/* ============================================================================
 * Some generally useful little support functions
 * ============================================================================
 */

/* Swap the bytes of a 24-bit word.
 * Works like htonl, but on 24 bits, not 32
 */
ci_inline ci_uint32
hton24 (ci_uint32 v) {
  ci_uint32 r;
  r =      (((v & 0x0000FF) << 16) |
             (v & 0x00FF00) |
            ((v & 0xFF0000) >> 16));
  return r;
}

/* Swap the bytes of a 24-bit word.
 * Works like ntohl, but on 24 bits, not 32
 */
ci_inline ci_uint32
ntoh24 (ci_uint32 v) {
  return hton24 (v);  /* Just swapping bytes, so same both ways */
}

/* Return i rounded up to a multiple of 4 */
ci_inline int
round4 (int i) {
  return (i + 3) & -4;
}

#define ci_assert_offset(offset, type, field) \
  ci_assert_equal(offset, CI_MEMBER_OFFSET(type, field));


/* ============================================================================
 *  Operations on PDUs (abstracting header and data digest insertion)
 * ============================================================================
 */

/* Get the correct header length for a PDU header on a given connection, by
 * checking the connection state and the header digest flag.
 */
ci_inline ci_uint32 pdu_hdr_len(ci_iscsi_connection *conn)
{
  return (conn->our_csg == FullFeaturePhase && conn->params.header_digest == 1)
           ? 52 : 48;
}

/* Fill in the header digest for a PDU header structure (if necessary).
 */
ci_inline void pdu_hdr_finish(ci_iscsi_connection *conn,
                                  ci_iscsi_pdu *pdu_hdr)
{
#ifdef __ci_storport__
  // TODO:: STANBELL is avoiding pulling in the citools library for crc32
#else
  if (conn->our_csg == FullFeaturePhase && conn->params.header_digest == 1)
  {
    if (ci_iscsi_tx_do_is_enabled) {
      pdu_hdr->generic.header_digest = 0;
    }
    else {
      ci_uint32 digest = ci_crc32c(pdu_hdr->generic.header, 48);
      pdu_hdr->generic.header_digest = CI_BSWAP_LE32(digest);
    }
  }
#endif
}

/* Take a data length and add the appropriate extra amount for the data digest,
 * according to the original length and the connection state.
 */
ci_inline size_t pdu_data_len_with_digest(ci_iscsi_connection *conn,
                                          size_t data_len)
{
  if (data_len > 0)
    return round4(data_len) + ((conn->our_csg == FullFeaturePhase
                                && conn->params.data_digest == 1) ? 4 : 0);
  else
    return 0;
    /* note that zero-length data => zero-length digest (RFC 3720, 10.2.3) */
}


/* Find the total PDU size, by adding the appropriate amount for the header and
 * any enabled digests to the supplied data length.
 */
ci_inline size_t pdu_total_from_data_len(ci_iscsi_connection *conn,
                                         size_t data_len)
{
  return pdu_hdr_len(conn) + pdu_data_len_with_digest(conn, data_len);
}


/* "Finish" an iovec.  On input, the iovec should be structured as follows:
 * - iov[0].iov_base points to an iSCSI PDU header structure
 * - iov[0].iov_len is ignored
 * - subsequent elements point to the data, except the last one
 * - iov[iovlen-1].iov_base points to a 4-byte area for storing the data digest
 * - iov[iovlen-1].iov_len must be 4
 * The header structure will be updated with a header digest if necessary, and
 * iov[0].iov_len will be filled in as 48 or 52 as appropriate.  If a data
 * digest is required, *iov[iovlen-1].iov_base will be updated with the data
 * digest, iov[iovlen-1].iov_len will be set to 4, and the original iovlen will
 * be returned.  If no digest is required, the original length minus one will
 * be returned.
 */
ci_inline int
pdu_iov_finish(ci_iscsi_connection *conn, ci_iovec *iov, int iovlen)
{
  ci_assert_ge(iovlen, 2);  /* at least header and space for data digest */
  CI_IOVEC_LEN(&iov[0]) = pdu_hdr_len(conn);
  if (conn->our_csg == FullFeaturePhase && conn->params.data_digest == 1)
  {
    if (ci_iscsi_tx_do_is_enabled) {
      /* If we're using hardware digest offload, we need a dummy 4-byte space in
       * the packet (if there is any data) so that the packet looks right to the
       * rest of the stack.  (We won't actually end up sending these 4 bytes to
       * the hardware, but they're removed at a much lower level -- just before
       * pushing the descriptors for the actual transmit.)
       */
      if (iovlen > 2) {
        /* Must have some data in this case, so keep dummy space for digest */
        return iovlen;
      }
      else {
        /* No data in this case, so remove dummy space for digest */
        CI_IOVEC_LEN(&iov[1]) = 0;
        return 1;
      }
    }
    else {
      size_t len;
      len = ci_iscsi_crc32c_iovec_tx_copy(conn, iov + 1, iovlen - 1);
      if (len > 0)
        return iovlen;
    }
  }
  /* No digest required, or ci_iscsi_crc32c_iovec_tx_copy() found datalen = 0 */
  return iovlen - 1;
}
 

/* Windows won't let us block in the kernel, so we have to spin instead of
 * sleep when doing various things:
 */
#if defined(__ci_wdm__) || defined(__ci_wdf__)

#define CI_ISCSI_WIN32_SPIN_USECS 20000

ci_inline void ci_iscsi_win32_delay(void)
{
  LARGE_INTEGER delay;
  delay.QuadPart = -CI_ISCSI_WIN32_SPIN_USECS;
  KeDelayExecutionThread (KernelMode, FALSE, &delay);
}

ci_inline int ci_iscsi_tcp_sendmsg(tips_stack *ni, ci_tcp_state *ts,
                                   const struct msghdr *msg, int flags)
{
  while (1) {
    int r;
    r = ci_tcp_sendmsg(ni, ts, msg, flags|MSG_DONTWAIT, CI_ADDR_SPC_KERNEL);
    if (r != -EAGAIN)
      return r;
    ci_iscsi_win32_delay();
  }
}

ci_inline int ci_iscsi_netif_lock(tips_stack *ni)
{
  while (1) {
    if (tips_stack_trylock(ni))
      return 0;
    ci_iscsi_win32_delay();
  }
}

ci_inline void ci_iscsi_netif_unlock(tips_stack *ni)
{
  tips_stack_unlock(ni);
}

ci_inline int ci_iscsi_netif_pkt_wait(tips_stack *ni, int lock_flags)
{
  while (1) {
    if (lock_flags & CI_SLEEP_NETIF_LOCKED)
      ci_iscsi_netif_unlock(ni);
    ci_iscsi_win32_delay();
    ci_iscsi_netif_lock(ni);
    lock_flags |= CI_SLEEP_NETIF_LOCKED;
    if (tips_stack_pkt_can_alloc_retrans(ni))
      return 0;
  }
}

#elif defined __ci_storport__

#include <ci/iscsi/tips_inline_lwip.h>


ci_inline int ci_iscsi_tcp_sendmsg(tips_stack *ni, tips_tcp_socket *ts,
                                   const struct msghdr *msg, int flags)
{
  while (1) {
    int r;
    r = tips_sendmsg(ni, ts, msg, flags|MSG_DONTWAIT, CI_ADDR_SPC_KERNEL);
    if (r != -EAGAIN)
      return r;
    /* StorPortDelay(); */
  }
}


#define __tips_stack_lock(x)    1
#define __tips_stack_unlock(x)

ci_inline int ci_iscsi_netif_lock(tips_stack *ni)
{
  return __tips_stack_lock(ni);
}

ci_inline void ci_iscsi_netif_unlock(tips_stack *ni)
{
  __tips_stack_unlock(ni);
}

ci_inline int ci_iscsi_netif_pkt_wait(tips_stack *ni, int lock_flags)
{
  /*
   * Only used by the DHCP_Client, STANBELL thinks we won't be activating.
   */
  // return tips_stack_pkt_wait(ni, lock_flags);
  return 0;
}

#else

ci_inline int ci_iscsi_tcp_sendmsg(tips_stack *ni, tips_tcp_socket *ts,
                                   const struct msghdr *msg, int flags)
{
  while (1) {
    int r;
    r = tips_sendmsg(ni, ts, msg, flags|MSG_DONTWAIT, CI_ADDR_SPC_KERNEL);
    if (r != -EAGAIN)
      return r;
    /* Need Linux equivalent of "ci_iscsi_win32_delay();" */
  }
}

ci_inline int ci_iscsi_netif_lock(tips_stack *ni)
{
  return tips_stack_lock(ni);
}

ci_inline void ci_iscsi_netif_unlock(tips_stack *ni)
{
  tips_stack_unlock(ni);
}

ci_inline int ci_iscsi_netif_pkt_wait(tips_stack *ni, int lock_flags)
{
#ifdef POLLING_RX_PACKETS
  return tips_stack_pkt_wait(ni, lock_flags);
#else
  return 0;
#endif
    // def POLLING_RX_PACKETS
}

#endif


/* Send a header only.  Headers are a fixed length (48 bytes, or 52 if a digest
 * is included) so there is no need to pass in a length.  This function sets
 * the digest if necessary, so there is no need to call pdu_hdr_finish() before
 * calling this.
 */
extern ssize_t pdu_send_hdr_only(ci_iscsi_connection *conn, void *buf);

/* Send header and data, where the data is in a single contiguous buffer.
 * Rounding up to a 4-byte boundary is performed if necessary; the supplied
 * buffer must be large enough to this to happen.  Also, header and data
 * digests will be calculated and inserted if necessary (but no extra space is
 * required for the data digest).
 */
extern ssize_t pdu_send_hdr_and_data(ci_iscsi_connection *conn,
                                     const void *hdr_buf, const void *data_buf,
                                     size_t data_len);


/* ============================================================================
 *  FIFO operations
 * ============================================================================
 */

extern iscsi_fifo_t iscsi_fifo_slice (iscsi_fifo_t *fifo);
/* Atomically returns a fifo with all the items on 'fifo' in a new fifo, such
 * that new items can be added to 'fifo'.  Like all the iscsi_fifo stuff, it's
 * atomic and thread-safe wrt to putting stuff on the queue, taking things off
 * the queue is not reentrant.
 */

extern int iscsi_fifo_get (iscsi_fifo_t *fifo, ci_iscsi_session *sess);
/* Returns one item from the fifo, doing the magic to reorder between stacks
 * if necessary.
 */

ci_inline void
iscsi_fifo_init (iscsi_fifo_t *fifo)
{
  ci_atomic_set(&fifo->in_queue, -1);
  fifo->reorder = -1;
}




ci_inline int
iscsi_fifo_empty (iscsi_fifo_t *fifo)
{
  return (ci_atomic_read (&fifo->in_queue) == -1) && (fifo->reorder == -1);
}




/* ============================================================================
 *  Logging macros
 * ============================================================================
 */

/* The flags are defined in ci/iscsi/interface.h, and we follow the same scheme
 * for debug vs. release builds as TP_LOG: error logging (ISCSI_LOG_E(...)) is
 * always compiled in; all other logging macros will compile away in release
 * builds.
 */

extern ci_uint32 ci_iscsi_log_bits;

#define ISCSI_LOG_C(c,x)   do{ if(c) do{x;}while(0); }while(0)
#define ISCSI_LOG_E(x)     ISCSI_LOG_C(ci_iscsi_log_bits & CI_ISCSI_LOG_E, x)

#ifdef NDEBUG
# define ISCSI_LOG_DC(c,x)
#else
# define ISCSI_LOG_DC      ISCSI_LOG_C
#endif

#define ISCSI_LOG_FL(f,x)  ISCSI_LOG_DC(ci_iscsi_log_bits & (f), x)

#define ISCSI_LOG_U(x)     ISCSI_LOG_FL(CI_ISCSI_LOG_U  , x)
#define ISCSI_LOG_S(x)     ISCSI_LOG_FL(CI_ISCSI_LOG_S  , x)
#define ISCSI_LOG_V(x)     ISCSI_LOG_FL(CI_ISCSI_LOG_V  , x)
#define ISCSI_LOG_KS(x)    ISCSI_LOG_FL(CI_ISCSI_LOG_KS , x)
#define ISCSI_LOG_SD(x)    ISCSI_LOG_FL(CI_ISCSI_LOG_SD , x)
#define ISCSI_LOG_PN(x)    ISCSI_LOG_FL(CI_ISCSI_LOG_PN , x)
#define ISCSI_LOG_PDU(x)   ISCSI_LOG_FL(CI_ISCSI_LOG_PDU, x)
#define ISCSI_LOG_RX(x)    ISCSI_LOG_FL(CI_ISCSI_LOG_RX , x)
#define ISCSI_LOG_TX(x)    ISCSI_LOG_FL(CI_ISCSI_LOG_TX , x)
#define ISCSI_LOG_CO(x)    ISCSI_LOG_FL(CI_ISCSI_LOG_CO , x)
#define ISCSI_LOG_SC(x)    ISCSI_LOG_FL(CI_ISCSI_LOG_SC , x)
#define ISCSI_LOG_LOCK(x)  ISCSI_LOG_FL(CI_ISCSI_LOG_LOCK, x)
#define ISCSI_LOG_ATX(x)   ISCSI_LOG_FL(CI_ISCSI_LOG_ATX, x)
#define ISCSI_LOG_SG(x)    ISCSI_LOG_FL(CI_ISCSI_LOG_SG, x)
#define ISCSI_LOG_STM(x)   ISCSI_LOG_FL(CI_ISCSI_LOG_STM, x)
#define ISCSI_LOG_DIG(x)   ISCSI_LOG_FL(CI_ISCSI_LOG_DIG, x)
#define ISCSI_LOG_WMIC(x)  ISCSI_LOG_FL(CI_ISCSI_LOG_WMIC, x)
#define ISCSI_LOG_WMI(x)   ISCSI_LOG_FL(CI_ISCSI_LOG_WMI, x)
#define ISCSI_LOG_DHCP(x)  ISCSI_LOG_FL(CI_ISCSI_LOG_DHCP, x)

/* Temporary logging, removed before checkin */
#define ISCSI_LOG_TEMP(x)  x

#define log                ci_log
#define iscsi_log          ci_log

/* Format string to log a string that may be very long */
#define FMT_BIG_STR(pre, s, post) ((strlen(s)<100) ? pre "%s" post \
                                                   : pre "%.30s..." post)


/* ============================================================================
 *  Debugging functions
 * ============================================================================
 */

extern void ci_iscsi_dump_sess_params(ci_iscsi_session_params *p);
extern void ci_iscsi_dump_conn_params(ci_iscsi_connection_params *p);
extern void ci_iscsi_dump_sess_state(ci_uint32 sess_id);
extern void ci_iscsi_dump_conn_sef(ci_uint32 sess_id, ci_uint32 conn_id);
extern void ci_iscsi_dump_conn_state(ci_uint32 sess_id, ci_uint32 conn_id);
extern void ci_iscsi_dump_control_params(ci_iscsi_control_params *params);
extern void ci_iscsi_dump_secrets(void);
extern const char *ci_iscsi_scsi_cmd_name(ci_uint8 opcode);
extern const char *ci_iscsi_scsi_response_name(ci_uint8 response);
extern const char *ci_iscsi_scsi_status_name(ci_uint8 status);
extern const char *ci_iscsi_conn_state_name(int state);
extern const char *ci_iscsi_conn_event_name(int event);

extern void ci_iscsi_dump_command_queue(ci_iscsi_session *sess);
extern void ci_iscsi_dump_command_queue_ITTs(ci_iscsi_session *sess);
extern void ci_iscsi_dump_conn_queue(ci_iscsi_session *sess);
extern void ci_iscsi_dump_command(struct command_args *cmd);

extern void ci_iscsi_dump_stats_buildinfo(void);
extern void ci_iscsi_dump_stats_initiator(void);
extern void ci_iscsi_dump_stats_sessions(ci_iscsi_session *sess);
extern void ci_iscsi_dump_stats_scsicmds(ci_iscsi_session *sess);
extern void ci_iscsi_dump_stats_datalens(ci_iscsi_session *sess);

#endif





/*
 * These are callbacks located in the TCP provider that iSCSI is running on.
 * The 'efab_...' names below are the names used in the iSCSI code itself.
 * They originate in a linux onload implementation. The 'tips_..' names
 * are the preferred generic names.
 */
#define efab_tcp_helper_sock_callback_arm     tips_sock_callback_arm
#define efab_tcp_helper_sock_callback_disarm  tips_sock_callback_disarm
#define efab_tcp_helper_sock_callback_set     tips_sock_callback_set
#define efab_tcp_helper_close_endpoint        tips_close_endpoint


#if defined(__ci_storport__)
/*
 * For storport, the callback names are #define-mangled into lwIp-facing
 * functions.
 */

# define tips_sock_callback_arm                 lwip_sock_callback_arm
# define tips_sock_callback_disarm              lwip_sock_callback_disarm
# define tips_sock_callback_set                 lwip_sock_callback_set
# define tips_close_endpoint                    lwip_close_endpoint


struct tcp_helper_resource_s;
typedef struct tcp_helper_resource_s  tcp_helper_resource_t;

extern int lwip_sock_callback_arm(
    tcp_helper_resource_t*, oo_sp, void* arg);
extern int lwip_sock_callback_disarm(
    tcp_helper_resource_t*, oo_sp);
extern int lwip_sock_callback_set(
    tcp_helper_resource_t*, void (*fn)(void* arg, int why));

extern void lwip_close_endpoint(tcp_helper_resource_t* trs,
                                           oo_sp ep_id);



#else

#define tips_sock_callback_arm            efab_tcp_helper_sock_callback_arm
#define tips_sock_callback_disarm         efab_tcp_helper_sock_callback_disarm
#define tips_sock_callback_set            efab_tcp_helper_sock_callback_set

extern int efab_tcp_helper_sock_callback_arm(
    tcp_helper_resource_t*, oo_sp, void* arg);
extern int efab_tcp_helper_sock_callback_disarm(
    tcp_helper_resource_t*, oo_sp);
extern int efab_tcp_helper_sock_callback_set(
    tcp_helper_resource_t*, void (*fn)(void* arg, int why));

extern void efab_tcp_helper_close_endpoint(tcp_helper_resource_t* trs,
                                           oo_sp ep_id);



#endif


/*! \cidoxg_end */

