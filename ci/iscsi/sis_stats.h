/**************************************************************************\
*//*! \file stats.h
** <L5_PRIVATE L5_HEADER >
** \author  aam
**  \brief  ISCSI statistics gathering / reporting
**   \date  2006/04/12
**    \cop  (c) Level 5 Networks Limited.
** </L5_PRIVATE>
*//*
\**************************************************************************/

/*! \cidoxg_include_ci_iscsi */

#ifndef __CI_ISCSI_STATS_H__
#define __CI_ISCSI_STATS_H__

struct ci_iscsi_connection_s;
struct ci_iscsi_session_s;


/* Values that we count */

/* Connection specific values */
typedef enum {
  CI_ISCSI_STATS_BYTE_SENT = 0,         /* bytes sent */
  CI_ISCSI_STATS_BYTE_RECVD,            /* bytes received */
  CI_ISCSI_STATS_PDU_SENT,              /* PDUs sent */
  CI_ISCSI_STATS_PDU_RECVD,             /* PDUs received */
  CI_ISCSI_STATS_RX_HW_HDIG,            /* RXes with hardware header digest */
  CI_ISCSI_STATS_RX_SW_HDIG,            /* RXes with software header digest */
  CI_ISCSI_STATS_RX_HW_DDIG,            /* RXes with hardware data digest */
  CI_ISCSI_STATS_RX_SW_DDIG,            /* RXes with software header digest */
  /* removed MAX_TIME and AVG_TIME, these are not just simple counters... */
  CI_ISCSI_STATS_DIGEST_ERR,            /* digest errors */
  CI_ISCSI_STATS_TIMEOUT,               /* connection timeouts */
  CI_ISCSI_STATS_FORMAT_ERR,            /* format errors */
  CI_ISCSI_STATS_LOGIN_OK,              /* successful logins */
  CI_ISCSI_STATS_LOGIN_FAIL,            /* login failures, any reason */
  CI_ISCSI_STATS_LOGIN_FAIL_MISC,       /* login failures, misc reason */
  CI_ISCSI_STATS_LOGIN_FAIL_AUTH_TYPE,  /* login auth type failure */
  CI_ISCSI_STATS_LOGIN_FAIL_AUTH,       /* login authentication failures */
  CI_ISCSI_STATS_LOGIN_FAIL_NEGOTIATE,  /* login negotiation failures */
  CI_ISCSI_STATS_LOGIN_REDIRECT,        /* login redirections */
  CI_ISCSI_STATS_LOGOUT_NORMAL,         /* logout reqs, normal reason */
  CI_ISCSI_STATS_LOGOUT_ABNORMAL,       /* logout reqs, other reason */
  CI_ISCSI_STATS_N                      /* (number of session stat IDs) */
} ci_iscsi_stats_id;


/* Stats must be locked to prevent races between reading and flushing causing
 * large jumps.  The flush functions acquire the lock internally, but the
 * caller must lock for reads (typically, multiple counters will be read at
 * once).
 *
 * Increments of per-connection stats do not require any lock.
 */
extern ci_irqlock_t ci_iscsi_stats_lock;

/* Read a counter for a connection - must be called with stats lock held */
extern ci_uint64 ci_iscsi_stats_conn_get(struct ci_iscsi_connection_s *conn,
                                         ci_iscsi_stats_id id);

/* Read a counter for a session  - must be called with stats lock held */
extern ci_uint64 ci_iscsi_stats_sess_get(struct ci_iscsi_session_s *sess,
                                         ci_iscsi_stats_id id);

/* Read a global counter - must be called with stats lock held */
extern ci_uint64 ci_iscsi_stats_global_get(ci_iscsi_stats_id id);


/* Flush connection counters to session - acquires lock itself */
extern void ci_iscsi_stats_conn_flush(struct ci_iscsi_connection_s *conn);

/* Flush session counters to globals - acquires lock itself */
extern void ci_iscsi_stats_sess_flush(struct ci_iscsi_session_s *sess);


/* Support for dumping additional stats in a generic way */
typedef void (*ci_iscsi_stats_writer)(void *ctx, const char *fmt, ...);
extern void ci_iscsi_stats_write_buildinfo(ci_iscsi_stats_writer wr,
                                           void *ctx);
extern void ci_iscsi_stats_write_initiator(ci_iscsi_stats_writer wr,
                                           void *ctx);
extern void ci_iscsi_stats_write_session(struct ci_iscsi_session_s *sess,
                                         ci_iscsi_stats_writer wr, void *ctx);
extern void ci_iscsi_stats_write_scsicmds(struct ci_iscsi_session_s *sess,
                                          ci_iscsi_stats_writer wr, void *ctx);
extern void ci_iscsi_stats_write_datalens(struct ci_iscsi_session_s *sess,
                                          ci_iscsi_stats_writer wr, void *ctx);


#endif

/*! \cidoxg_end */

