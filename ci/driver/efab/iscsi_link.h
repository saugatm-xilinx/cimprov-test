/**************************************************************************\
*//*! \file iscsi_link.h
** <L5_PRIVATE L5_HEADER >
** \author  aam
**  \brief  Common header for "direct function call" iscsi->wdm link API.
**   \date  2006/2/21
**    \cop  (c) Level 5 Networks Limited.
** </L5_PRIVATE>
*//*
\**************************************************************************/

#ifndef EFAB_ISCSI_LINK_H
#define EFAB_ISCSI_LINK_H

#if defined(__ci_wdm__) || defined(__ci_iscsi__)

/* The iSCSI driver passes a ci_il_wdm_fns structure to the wdm driver, which
 * fills it with pointers to the various wdm functions that iSCSI needs.
 * The iSCSI driver gets the address of the initial function to call from the
 * wdm drivers device extension. The iSCSI driver put the address of its ioctl
 * handler into the device extnesion.
 */

#include <ci/driver/efab/iscsi_link_fns.h> /* for ci_il_fns_t */

typedef ci_il_fns_t ci_il_wdm_fns;

/* These functions allow us to register a callback invoked just before
   the iSCSI link interface is withdrawn */
extern int /*rc*/
ci_register_unload_event(ci_event_handle_t *out_event,
	                 ci_event_fn_t *fn, void *arg);
extern int /*rc*/
ci_unregister_unload_event(ci_event_handle_t event);


#endif /* wdm or iscsi */

#ifdef __ci_iscsi__


/* This links our ioctl handler into the wdm driver */
extern void link_to_wdm_ctor(PDRIVER_OBJECT driver_object);
extern NTSTATUS link_to_wdm(PDEVICE_OBJECT device_object);
extern NTSTATUS unlink_from_wdm();
extern NTSTATUS wait_for_wdm_link();

/* Check that the bus driver has set things up for us to send packets */
extern int is_mac_setup(void);

#endif


#endif /* EFAB_ISCSI_LINK_H */
