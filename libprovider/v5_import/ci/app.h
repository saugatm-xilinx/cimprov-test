/**************************************************************************\
*//*! \file
** <L5_PRIVATE L5_HEADER>
** \author  
**  \brief  
**   \date  
**    \cop  (c) Level 5 Networks Limited.
** </L5_PRIVATE>
*//*
\**************************************************************************/

/*! \cidoxg_include_ci */

#ifndef __CI_APP_H__
#define __CI_APP_H__

#ifdef __KERNEL__
# error This header should not be included in __KERNEL__ builds.
#endif

#ifndef __CI_TOOLS_H__
# include <ci/tools.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__unix__) || defined(__MACH__)
# include <ci/app/platform/unix.h>
#elif defined(_WIN32)
# include <ci/app/platform/win32.h>
#else
#  error Unknown platform.
#endif

#include <ci/app/utils.h>
#include <ci/app/testapp.h>
#include <ci/app/net.h>
#include <ci/app/ctimer.h>
#include <ci/app/stats.h>
#include <ci/app/testpattern.h>

#ifdef __cplusplus
}
#endif

#endif  /* __CI_APP_H__ */

/*! \cidoxg_end */
