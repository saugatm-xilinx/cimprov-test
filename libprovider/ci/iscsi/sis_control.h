/**************************************************************************\
*//*! \file control.h
** <L5_PRIVATE L5_HEADER >
** \author  mjs
**  \brief  ISCSI control plane interface
**   \date  2005/07/11
**    \cop  (c) Level 5 Networks Limited.
** </L5_PRIVATE>
*//*
\**************************************************************************/

/*! \cidoxg_include_ci_iscsi */

#ifndef __CI_ISCSI_CONTROL_H__
#define __CI_ISCSI_CONTROL_H__


// #include <ci/iscsi/internal.h>


/* ============================================================================
 *  Control plane functions: OS-dependent layer -> common control layer
 * ============================================================================
 */

/* Find the next connection to send a command on */
extern ci_iscsi_connection *ci_iscsi_get_next_conn(ci_iscsi_session *sess);

/*! Allocate and initialise a new session
 * \param   pSess      location to store allcoated session
 * \param   params     session parameters
 *
 * Windows only parameters
 * \param   luns       lun mappings
 * \param   devEx      device extension - Windows only
 *
 * \return  error code
 */
int ci_iscsi_alloc_session(ci_iscsi_session **psess,
                           ci_iscsi_session_params *params
#if defined(__ci_wdm__) || defined(__ci_wdf__)
                           , ci_iscsi_lun_map *luns,
                           HW_DEVICE_EXTENSION *devEx
#elif defined(__ci_storport__)
                           , PVOID pAdapter
#endif
                           );

/* Tidy up and deallocate a session
 * This can be called to tidy up a session however far through the creation
 * and use processes we have got
 * Must only be called from an ioctl context
 */
extern void ci_iscsi_session_destroy(ci_iscsi_session *sess);

/*! Get a free connection from a session and initialise it. Note that each
 * connection lives within exactly 1 session for its entire existence.
 * \param   sess         session connection is in
 * \param   pConn        location to store allocated connection
 * \param   params       connection parameters
 * \return  error code
 */
extern int ci_iscsi_conn_alloc(ci_iscsi_session *sess,
                               ci_iscsi_connection **pConn,
                               ci_iscsi_connection_params *params);

/* (Re)initialise a connection ready for logging in */
extern void ci_iscsi_conn_init(ci_iscsi_session *sess,
                               ci_iscsi_connection *conn);

/* Return a connection to the free list */
extern int ci_iscsi_conn_free(ci_iscsi_session *sess,
                              ci_iscsi_connection *conn);

/* Log in a connection
 * Must only be called from ci_iscsi_sender() in the session tx thread.
 */
extern int ci_iscsi_conn_do_login(ci_iscsi_session *sess,
                                  ci_iscsi_connection *conn);

/* Check whether we can assign a command to a connection */
ci_inline int
ci_iscsi_can_assign_cmd(ci_iscsi_connection *conn) {
  ci_assert(conn!=NULL);
  ci_assert(conn->session!=NULL);
  return (conn->state==CONN_STATE_ACTIVE) &&
    (conn->session->params.session_type==CI_ISCSI_SESSION_NORMAL);
}

/* Set / clear the specified connection event flag(s).
 * Returns the flag state we were in before our change.
 */
#define CONN_EVENT_LOGIN            1
#define CONN_EVENT_LOGOUT           2
#define CONN_EVENT_GOT_RESP         4
#define CONN_EVENT_ACTIVE           8
#define CONN_EVENT_LAST_COMMAND  0x10
#define CONN_EVENT_IN_IOCTL      0x20
#define CONN_EVENT_OUT_IOCTL     0x40
#define CONN_EVENT_DROP          0x80
#define CONN_EVENT_TIMEOUT      0x100
#define CONN_EVENT_ALL          0x1FF
#define CONN_EVENT_COUNT            9

extern
ci_uint32 ci_iscsi_conn_set_event_flag(ci_iscsi_connection *conn,
                                       ci_uint32 events);

extern
ci_uint32 ci_iscsi_conn_clear_event_flag(ci_iscsi_connection *conn,
                                         ci_uint32 events);

/* Kick the tx thread into action for the given session */
extern void ci_iscsi_sess_kick_tx_thread(ci_iscsi_session *sess);

/* Kick the tx thread into action for the given connection */
extern void ci_iscsi_conn_kick_tx_thread(ci_iscsi_connection *conn);

/* Signal the ioctl waiting for this connection, if any.
 * ioctl-type should be:
 * CONN_EVENT_IN_IOCTL   - only signal a login ioctl
 * CONN_EVENT_OUT_IOCTL  - only signal a logout ioctl
 * CONN_EVENT_IN_IOCTL | CONN_EVENT_OUT_IOCTL - signal either
 */
extern void ci_iscsi_conn_signal_ioctl(ci_iscsi_connection *conn,
                                       ci_uint32 ioctl_type);

/* Complete the waiting ioctl for this connection
 * Must only be called by session TX thread
 * \param   conn                  connection to complete ioctl of
 */
extern void ci_iscsi_conn_complete_ioctl(ci_iscsi_connection *conn);

/* Flag a connection to be logged out.
 * May be called from any context, does not block or take locks.
 * reason is an iSCSI connection close reason and must currently be 1 (close
 * the connection)
 * Flags are state event flags required in addition to CONN_EVENT_LOGOUT,
 * eg CONN_EVENT_OUT_IOCTL.
 */
extern int ci_iscsi_conn_flag_logout(ci_iscsi_connection *conn, ci_uint8 reason,
                                     ci_uint32 flags);

/* Flag a connection to be dropped.
 * May be called from any context, does not block or take locks.
 * Delay is the minimum time before reconnection is permitted.
 */
extern void ci_iscsi_conn_flag_drop(ci_iscsi_connection *conn,
                                    ci_uint32 delay_sec);

/* Send a logout request.
 * Must only be called from ci_iscsi_sender() in the session tx thread.
 */
extern int ci_iscsi_send_logout_req(ci_iscsi_session *sess,
                                    ci_iscsi_connection *conn);

/* Perform a control operation */
extern int ci_iscsi_perform_control_op(ci_iscsi_control_params *params);


/* ============================================================================
 *  Control plane functions: iSCSI core -> common control layer
 * ============================================================================
 */

/* Handle an Asynchronous Message PDU (non-SCSI, i.e. not code 0) */
extern int ci_iscsi_handle_async_message(ci_iscsi_session *sess,
                                         ci_iscsi_connection *conn,
                                         unsigned char *data, size_t len,
                                         unsigned int event,
                                         unsigned int param1,
                                         unsigned int param2,
                                         unsigned int param3);


/* ============================================================================
 *  Core interface: common control layer -> iSCSI core
 * ============================================================================
 */

/* Send a Text Request PDU */
extern int ci_iscsi_send_text_request(ci_iscsi_session *sess,
                                      ci_iscsi_connection *conn, size_t len);

extern ci_iscsi_session ci_iscsi_sessions[];

ci_inline ci_iscsi_session*
ci_iscsi_id2session (unsigned id)
{
  ci_assert_lt(id, CI_ISCSI_MAX_SESSIONS);
  ci_assert_equal (ci_iscsi_sessions [id].id, id);
  ISCSI_LOG_V(log("%s(%d) returns %p", __FUNCTION__, id,
                  ci_iscsi_sessions + id));
  return ci_iscsi_sessions + id;
}

ci_inline ci_iscsi_connection*
ci_iscsi_id2connection(unsigned sessId, int connId)
{
  ci_assert_lt(sessId, CI_ISCSI_MAX_SESSIONS);
  ci_assert_equal(ci_iscsi_sessions[sessId].id, sessId);
  ci_assert_ge(connId, 0);
  ci_assert_lt(connId, CI_ISCSI_MAX_CONNS_PER_SESS);
  ISCSI_LOG_V(log("%s(%d, %d) returns %p", __FUNCTION__, sessId, connId,
                  &ci_iscsi_sessions[sessId].connections[connId]));
  return &ci_iscsi_sessions[sessId].connections[connId];
}

/* ============================================================================
 *  Control plane functions: startup / shutdown
 * ============================================================================
 */

/* Startup */
extern int ci_iscsi_control_ctor(void);

/* Shutdown */
extern void ci_iscsi_control_dtor(void);
extern void ci_iscsi_shutdown_all(void);

#endif

/*! \cidoxg_end */

