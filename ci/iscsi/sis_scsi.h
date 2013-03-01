/**************************************************************************\
*//*! \file scsi.h
** <L5_PRIVATE L5_HEADER >
** \author  mjs
**  \brief  ISCSI interface to SCSI devices
**   \date  2005/07/06
**    \cop  (c) Level 5 Networks Limited.
** </L5_PRIVATE>
*//*
\**************************************************************************/

/*! \cidoxg_include_ci_iscsi */

#ifndef __CI_ISCSI_SCSI_H__
#define __CI_ISCSI_SCSI_H__


// #include <ci/iscsi/internal.h>


/* ============================================================================
 *  Generic OS-independent interface to OS-dependent SCSI driver
 * ============================================================================
 */

/* SCSI driver -> iSCSI protocol layer: command */
extern int ci_iscsi_command(ci_iscsi_session *sess, ci_iscsi_lun lun,
                            ci_iscsi_cmd_handle scsi_handle,
                            unsigned char *cmd, size_t cmd_len,
                            size_t bufferlen, int data_direction,
                            void ** iscsi_handle_out);

/* SCSI driver -> iSCSI protocol layer: abort
 *
 * Abort a commands on the specified session.
**/
void ci_iscsi_abort_commands (ci_iscsi_session *sess);


/*! SCSI driver -> iSCSI protocol layer: task management
 * param   sess              session to perform task management on
 * param   function          tm function to perform (ISCSI_TMFUNC_*)
 * param   lun               lun to perform function on, if required
 * return  error code
 */
extern int ci_iscsi_tm(ci_iscsi_session *sess, ci_uint8 function,
                       ci_iscsi_lun lun);

extern void ci_iscsi_copy_data (ci_iscsi_cmd_handle handle,
                                void *buf, size_t dlen);

extern int ci_iscsi_build_memdesc (ci_iscsi_cmd_handle sc,
                                   struct ci_mem_desc *desc,
                                   ci_uint32 offs, ci_uint32 len,
                                   struct memdesc_cursor *cursor);

extern void ci_iscsi_repair_memdesc (struct memdesc_cursor *cursor);

extern ci_uint32 ci_iscsi_checksum_memdesc(struct ci_mem_desc *desc);

extern int ci_iscsi_get_iov (ci_iscsi_cmd_handle handle, ci_iovec *iov,
                             int iovlen, void *bases[],
                             ci_uint32 offs, ci_uint32 len,
                             struct memdesc_cursor *cursor, ci_boolean_t atomic);

extern void ci_iscsi_put_iov (ci_iscsi_cmd_handle handle, int iovlen, void *bases[],
                              struct memdesc_cursor *cursor, 
                              ci_boolean_t atomic);

enum {CI_SCSI_NODATA, CI_SCSI_RDONLY, CI_SCSI_WRONLY, CI_SCSI_RDWR};


/* iSCSI protocol layer -> SCSI driver: update sense buffer data */
extern int ci_iscsi_command_sense(ci_iscsi_session *sess, ci_iscsi_lun lun,
                                  ci_iscsi_cmd_handle handle,
                                  unsigned char *sense_buf, size_t sense_len);

/* iSCSI protocol layer -> SCSI driver: update command result */
extern void ci_iscsi_command_result(ci_iscsi_session *sess, ci_iscsi_lun lun,
                                    ci_iscsi_cmd_handle handle,
                                    size_t residual_count,
                                    ci_iscsi_action_code response,
                                    unsigned int status);

/* iSCSI protocol layer -> SCSI driver: command complete */
extern void ci_iscsi_command_complete(ci_iscsi_session *sess, ci_iscsi_lun lun,
                                      ci_iscsi_cmd_handle handle);

#ifdef _WIN32
/*! iSCSI protocol layer -> SCSI driver: task management complete
 * param   sess              session request was sent over
 * param   handle            task management instance
 * param   response          target response
 */
extern void ci_iscsi_tm_complete(ci_iscsi_session *sess,
                                 ci_iscsi_tm_handle *handle,
                                 ci_uint8 response);
#endif

/* iSCSI protocol layer -> SCSI driver: command complete */
extern int ci_iscsi_async_event(ci_iscsi_session *sess, ci_iscsi_lun lun,
				unsigned char *sense_buf, size_t sense_len);

/* iSCSI protocol layer -> SCSI driver: create SCSI device for session */
extern int ci_iscsi_session_attach(ci_iscsi_session *sess);

/* iSCSI protocol layer -> SCSI driver: destroy SCSI device for session */
extern void ci_iscsi_session_detach(ci_iscsi_session *sess);

/* iSCSI protocol layer -> SCSI driver: startup */
extern int ci_iscsi_scsi_ctor(void);

/* iSCSI protocol layer -> SCSI driver: shutdown */
extern void ci_iscsi_scsi_dtor(void);

#endif

/*! \cidoxg_end */

