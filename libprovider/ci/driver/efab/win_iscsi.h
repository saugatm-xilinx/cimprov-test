/**************************************************************************\
*//*! \file linux_scsi.h
** <L5_PRIVATE L5_HEADER >
** \author  aam
**  \brief  ISCSI interface to SCSI devices
**   \date  2006/01/17
**    \cop  (c) Level 5 Networks Limited.
** </L5_PRIVATE>
*//*
\**************************************************************************/

/*! \cidoxg_include_ci_driver_efab */

#ifndef __CI_DRIVER_EFAB_WIN_ISCSI_H__
#define __CI_DRIVER_EFAB_WIN_ISCSI_H__

#ifndef __ci_driver__
#error "This is a driver module."
#endif


/* ============================================================================
 *  iSCSI OS abstraction implementation header for Win32
 * ============================================================================
 */

/*! We use the per session scsi_handle to locate our device extension.
 * Unfortunately we run into dependency nasties defining HW_DEVICE_EXTENSION
 * here, so we use a void * instead.
 */
typedef void *ci_iscsi_scsi_handle;

struct ci_iscsi_cmd_info;
typedef struct ci_iscsi_cmd_info *ci_iscsi_cmd_handle;
typedef void *ci_iscsi_tm_handle;  /* SCSI_REQUEST_BLOCK * */

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
  /* Not needed for Windows */
}

#endif

/*! \cidoxg_end */
