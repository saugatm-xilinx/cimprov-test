/**************************************************************************\
*//*! \file
** <L5_PRIVATE L5_HEADER >
** \author  
**  \brief  
**   \date  
**    \cop  (c) Level 5 Networks Limited.
** </L5_PRIVATE>
*//*
\**************************************************************************/

/*! \cidoxg_include_ci_tools */

#ifndef CUSTOMER_EVENT_LOG_H
#define CUSTOMER_EVENT_LOG_H

typedef int ci_cevtlog_cat_id_t;

#define CI_CEVTLOG_BUF_SIZE 80
#define CI_CEVTLOG_ARGS_MAX 8

#define CI_CEVTLOG_MAX_PATH 260

#ifdef _WIN32
#ifdef __KERNEL__
typedef WCHAR ci_cevtlog_char_t;
#else
typedef TCHAR ci_cevtlog_char_t;
#endif
#else
typedef char ci_cevtlog_char_t;
#endif


typedef struct _ci_cevtlog_format_cat_t {
  ci_cevtlog_cat_id_t id;
  int type;
  int facility; /* App / system */
  int numargs;
  ci_cevtlog_char_t* arglist;
#ifndef _WIN32
  ci_cevtlog_char_t* default_msg;
#endif
} ci_cevtlog_format_cat_t;

#ifdef _WIN64
#define CI_EVTLOG_NARG_TYPE ci_int64
/* this is a bad hack to deal with microsoft compiler bug; basically variadic
   functions args are broken on 64 bit windows (2003 compiler); one must ensure
   that the args start on a 64bit arg boundary so we can change the size of the
   previous arg to alter the alignment! SORRY :-(
*/
#else
#define CI_EVTLOG_NARG_TYPE int

#endif

struct ci_netif_s; /* forward declr */

/************************************************************************/
/* Public APIs for generating an event from driver or userspace
 */
extern void ci_cevtlog_app(
   struct ci_netif_s* netif,
   ci_cevtlog_cat_id_t cat_id, 
   CI_EVTLOG_NARG_TYPE num_args,...);


#ifdef __KERNEL__ /* we can only send a system event log from kernel */
extern void ci_cevtlog_sys(
   void* device_obj, 
   ci_cevtlog_cat_id_t cat_id, 
   CI_EVTLOG_NARG_TYPE num_args,...);
#endif

/************************************************************************/
/* Public API for emitting event from iSCSI driver
 */

extern void ci_vcevtlog_iscsi_nic(
   void *logger_handle, /* O/S dependent value */
   ci_cevtlog_cat_id_t cat_id, 
   CI_EVTLOG_NARG_TYPE num_args,
   va_list arg);


/************************************************************************/
/* Public API for emitting event from Control Plane
 */
extern void ci_cevtlog_cplane(
   void* device_obj /* set to NULL from user space */, 
   ci_cevtlog_cat_id_t cat_id, 
   CI_EVTLOG_NARG_TYPE num_args,...);

/************************************************************************/
/* Private APIs 
 */

extern int _ci_cevtlog_do_formatting(ci_cevtlog_cat_id_t cat_id, 
   int numargs,
   va_list args,
   ci_cevtlog_char_t** bufptrlist, 
   int facility,
   int* final_buf_size);

extern const ci_cevtlog_format_cat_t _ci_cevtlog_format_cat[];

#if !defined(_WIN32) || defined (__KERNEL__)

#define EVENTLOG_INFORMATION_TYPE 1
#define EVENTLOG_ERROR_TYPE 2
#define EVENTLOG_WARNING_TYPE 3
#endif

#ifndef _WIN32
#define FACILITY_SYSTEM 0
#define FACILITY_APP    1
#define FACILITY_CPLANE 2
#define FACILITY_ISCSI  3
#endif

#include "customer_event_log_msgs.h"

#endif
/*! \cidoxg_end */
