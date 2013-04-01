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

/*! \cidoxg_include_ci_driver  */

#ifndef __CI_DRIVER_INTERNAL__
#define __CI_DRIVER_INTERNAL__

#ifndef __CI_TOOLS_H__
# include <ci/tools.h>
#endif
#include <ci/tools/timeval.h>


/* Needed by the platform code. */
#define ci_waitq_wait_forever(timevalp)     \
  (!(timevalp) || !((timevalp)->tv_sec + (timevalp)->tv_usec))


#ifndef NDEBUG
#include <ci/tools/memleak_debug.h>
#endif

#ifndef __ci_driver__
# error ci/driver/internal.h included, but __ci_driver__ is not defined.
#endif

#if defined(__ci_ul_driver__)

# include <ci/driver/platform/ul_driver_common.h>

# if defined(_WIN32)
#  include <ci/driver/platform/win32_common.h>
#  include <ci/driver/platform/win32_ul_driver.h>
# else
#  include <ci/driver/platform/unix_common.h>
#  include <ci/driver/platform/unix_ul_driver.h>
# endif

#elif !defined(__KERNEL__)

# error Ooops.

#elif defined(__linux__)

# include <ci/driver/platform/linux_kernel.h>

#elif defined(__sun__)

# include <ci/driver/platform/sunos_kernel.h>

#elif  defined(_WIN32)

# if defined(__ci_wdm__)
#  include <ci/driver/platform/win32_common.h>
#  include <ci/driver/platform/win32_kernel.h>
/* TODO - Sort out the new waiter implementation required... */
#  if defined (__ci_wdf__)
#   include <ci/driver/platform/bus_driver.h>
#  else
#   include <ci/driver/platform/wdm_driver.h>
#  endif
#  include <ci/driver/platform/wdm_idc.h>
#  include <ci/driver/platform/win32_apc.h>
# elif defined(__ci_iscsi__)
#  include <ci/driver/platform/win32_common.h>
#  include <ci/driver/platform/win32_kernel.h>
#  if defined (__ci_wdf__)
#   include <ci/driver/platform/bus_driver.h>
#  else
#   include <ci/driver/platform/wdm_driver.h>
#  endif
#  include <ci/driver/platform/wdm_idc.h>
# else
#  error What sort of _WIN32 driver is this?
# endif

#else

# error Unknown platform.

#endif


#endif  /* __CI_DRIVER_INTERNAL__ */
/*! \cidoxg_end */
