/**************************************************************************\
*//*! \file wdm_idc.h - header file for WDM interface table setup.
** <L5_PRIVATE L5_HEADER >
** \author  mch
**  \brief  header file for WDM interface table setup.
**   \date  2005/10
**    \cop  (c) Level 5 Networks Limited.
** </L5_PRIVATE>
*//*
\**************************************************************************/

#ifndef _WDM_IDC_H
#define _WDM_IDC_H

#include <ci/driver/platform/win_idc.h>
#include <ci/driver/efab/driverlink_win_api.h>

/*! \cidoxg_driver_win */

/* Forward declaration (later picked up by internal.h) */
struct efhw_nic_s;

/* Internal declarations of datastructures */

/* Callbacks into WDM driver to indicate NDIS existence changes. */
typedef VOID (*wdm_idc_new_ndis)(PVOID handle,
                                 LINK_IDC_LINK_ID id,
                                 struct efhw_nic_s *ndis_nic);

typedef VOID (*wdm_idc_ndis_for_wdm)(PVOID handle,
                                     LINK_IDC_LINK_ID id,
                                     struct efhw_nic_s *ndis_nic,
                                     struct efhw_nic_s *wdm_nic);

typedef VOID (*wdm_idc_ndis_closing)(PVOID handle,
                                     LINK_IDC_LINK_ID id,
                                     struct efhw_nic_s *ndis_nic,
                                     struct efhw_nic_s *wdm_nic);

typedef struct wdm_idc_handlers_s {
  wdm_idc_new_ndis      new_ndis;
  wdm_idc_ndis_for_wdm  ndis_for_wdm;
  wdm_idc_ndis_closing  ndis_closing;
} WDM_IDC_HANDLERS_T, *PWDM_IDC_HANDLERS_T;

#define WDM_IDC_STATE_MAGIC     (0x12488421)

typedef struct wdm_idc_state_s {
  ULONG                 magic;
  PIDC_CLI_STRUCT_T     p_cli;
  KSPIN_LOCK            lock;
  LIST_ENTRY            nw_links;     /* use lock */
  WDM_IDC_HANDLERS_T    handlers;     
  NPAGED_LOOKASIDE_LIST nw_alloc;     /* do not need sync under lock, but tends to be anyway */
} WDM_IDC_STATE_T, *PWDM_IDC_STATE_T;

typedef struct wdm_idc_srv_state_t {
  PIDC_SRV_STRUCT_T     idc_srv;
  /* TODO - anything else you need here, perhaps protected by a lock */
} WDM_IDC_SRV_STATE_T, *PWDM_IDC_SRV_STATE_T;

/* External declarations of datastructures and functions */
/* Functions to start/stop "client side" IDC (import of fn's from NDIS) */
NTSTATUS wdm_idc_start(PDRIVER_OBJECT driver_object, PWDM_IDC_HANDLERS_T handlers);
NTSTATUS wdm_idc_stop();

/* Functions to start/stop "server side" IDC (export of functions to iSCSI) */
NTSTATUS wdm_idc_srv_start(PDEVICE_OBJECT pdo);
NTSTATUS wdm_idc_srv_stop(PDEVICE_OBJECT pdo);

/* Will allow registration of WDM nic whether or not an existing NDIS entry is present. */
NTSTATUS wdm_idc_register_wdm_nic(PVOID handle OPTIONAL,
                                  LINK_IDC_LINK_ID id OPTIONAL,
                                  struct efhw_nic_s *wdm_nic,
                                  struct efhw_nic_s **ndis_nic OPTIONAL,
                                  PVOID *final_handle OPTIONAL);

/* Will allow deregistration of WDM nic whether or not an existing NDIS entry is present. */
NTSTATUS wdm_idc_deregister_wdm_nic(PVOID handle OPTIONAL,
                                    LINK_IDC_LINK_ID id OPTIONAL);

/* Function to allow translation between opaque link handle and WDM nic. */
struct efhw_nic_s *wdm_idc_link_handle_to_nic(idc_handle handle);

#endif
