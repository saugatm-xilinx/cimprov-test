/**************************************************************************\
*//*! \file
** <L5_PRIVATE L5_HEADER >
** \author  stg
**  \brief  error codes used in Windows Ul & drivers
**   \date  2005/11/23
**    \cop  (c) Level 5 Networks Limited.
** </L5_PRIVATE>
*//*
\**************************************************************************/

/*! \cidoxg_include_ci_tools_platform  */

#ifndef __CI_TOOLS_WIN__COMMON_H__
#define __CI_TOOLS_WIN__COMMON_H__

#include <ci/tools/platform/win_errno.h>

#define inline __inline

/*--------------------------------------------------------------------
 *
 *  Status values are 32 bit values layed out as follows:
 *
 *   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
 *   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
 *  +---+-+-------------------------+-------------------------------+
 *  |Sev|C|       Facility          |               Code            |
 *  +---+-+-------------------------+-------------------------------+
 *
 *  where
 *
 *      Sev - is the severity code
 *
 *          00 - Success
 *          01 - Informational
 *          10 - Warning
 *          11 - Error
 *
 *      C - is the Customer code flag
 *
 *      Facility - is the facility code
 *
 *      Code - is the facility's status code
 *
 *--------------------------------------------------------------------*/


/* need top 2 bits set to indicate an error, 
    plus next bit set to indicate a "customer code" error
  */
#define CI_NTSTATUS_CUSTOMER_ERROR      0xe0000000
#define CI_NTSTATUS_CODE_MASK           0x0000ffff


ci_inline int
ci_winstatus_to_rc(int win_status)
{
  int rc;
  if ((win_status & CI_NTSTATUS_CUSTOMER_ERROR) == CI_NTSTATUS_CUSTOMER_ERROR) {
    rc = -(win_status & CI_NTSTATUS_CODE_MASK);
    ci_assert_lt(rc, 0);
  }
  else if (win_status == 0)
      rc = 0;
  else {
    /* don't expect to be called with pending error codes
        - if so will need to special case them */
    ci_assert(win_status != ERROR_IO_PENDING);
    rc = -1;
  }
  return rc;
}

#ifdef __KERNEL__

ci_inline NTSTATUS
ci_rc_to_ntstatus(int rc)
{
  NTSTATUS status;
  ci_assert(rc <= 0);
  ci_assert(-rc < 0x10000);
  if (rc < 0) {
    status = CI_NTSTATUS_CUSTOMER_ERROR | (-rc);
  }
  else {
    ci_assert(rc == 0);
    status = STATUS_SUCCESS;
  }
  return status;     
}
#endif

/*--------------------------------------------------------------------
 *
 *
 *--------------------------------------------------------------------*/

#endif  /* __CI_TOOLS_WIN__COMMON_H__ */

/*! \cidoxg_end */
