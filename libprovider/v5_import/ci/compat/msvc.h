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

/*! \cidoxg_include_ci_compat  */

#ifndef __CI_COMPAT_MSVC_H__
#define __CI_COMPAT_MSVC_H__


/* To specify the build platform both WINVER and _WIN32_WINNT must be
** defined before any Windows/Plaform SDK headers are included.  If not
** otherwise defined, default to "Windows Server 2003".
*
*/
#ifndef WINVER
# ifdef __ci_storport__
# error WINVER must be defined for STORPORT
# endif
# define WINVER 0x0502
#endif

#ifndef _WIN32_WINNT
# ifdef __ci_storport__
# error _WIN32_WINNT must be defined for STORPORT
# endif
# define _WIN32_WINNT 0x0502
#endif


#define CI_HAVE_INT64


/* No harm pulling in the main system header on windows.  In fact we
** have to if we want 64 int support.
*/
#if defined(__ci_wdm__)
# include <wdm.h>
# if defined(__ci_wdf__)
#  define  WDF_DEVICE_NO_WDMSEC_H
#  include <wdf.h>
# endif
# if defined(__ci_ndis__)
#  include <ndis.h>
# endif
#elif defined(__ci_storport__)
/*
 * No need to do anything here.  Storport drivers should include
 * "miniport.h" and "storport.h" at an outer level.  If they didn't
 * we'll do it now.
 */
# ifndef _MINIPORT_
#  include <miniport.h>
# endif

# if defined(_BUILD_STORPORT_DRIVER)
#  ifndef _NTSTORPORT_
#   include <storport.h>
#  endif
# endif

# if defined(_BUILD_SCSIPORT_DRIVER)
#  include <scsi.h>
# endif

#elif !defined(__KERNEL__)

/* Must define WIN32 before including "winsock2.h" so that structures are
 * correctly packed.  Must not include "windows.h" before "winsock2.h" as that
 * would result in both "winsock.h" and "winsock2.h" being included.
 * "winsock2.h" automatically includes "windows.h".
 */
# include <winsock2.h>

#else
  /* This is a driver build, but we are wdm/ndis agnostic.  Since we haven't
  ** included a system header, we can't have 64-bit support.
  */
# undef CI_HAVE_INT64
#endif


#define ci_inline  static __inline

#include <limits.h>

#if defined(CI_HAVE_INT64)
  typedef ULONGLONG            ci_uint64;
  typedef LONGLONG             ci_int64;

# define CI_PRId64             "I64d"
# define CI_PRIi64             "I64i"
# define CI_PRIo64             "I64o"
# define CI_PRIu64             "I64u"
# define CI_PRIx64             "I64x"
# define CI_PRIX64             "I64X"
#endif

# define CI_PRId32             "I32d"
# define CI_PRIi32             "I32i"
# define CI_PRIo32             "I32o"
# define CI_PRIu32             "I32u"
# define CI_PRIx32             "I32x"
# define CI_PRIX32             "I32X"

/**********************************************************************
 * Safe types for handling pointers as integers
 */
#if !defined(LONG_PTR) || !defined(ULONG_PTR)
# if defined __x86_64__
#define LONG_PTR long long
#define ULONG_PTR unsigned long long
#else
#define LONG_PTR long
#define ULONG_PTR unsigned long
#endif
#endif

typedef LONG_PTR                       ci_ptr_arith_t;
typedef ULONG_PTR                    ci_uintptr_t;

/* and for handling HANDLES */

typedef ci_uint64 ci_fixed_descriptor_t;

#define from_fixed_descriptor( desc ) ((HANDLE)(ci_uintptr_t)(desc))
#define to_fixed_descriptor( desc ) ((ci_fixed_descriptor_t)(desc))

/* WIN32 DDK seems to think its funny to -DNDEBUG when doing a checked
** build.  We disagree.
*/
#if defined(DBG) && (DBG == 1) && defined(NDEBUG)
# undef NDEBUG
#endif


#if !defined(__KERNEL__)
# define CI_HAVE_NPRINTF  1
# include <stdio.h>
# define vsnprintf _vsnprintf
# define snprintf  _snprintf
#endif


/*! MSVC totally sucks, and doesn't support C99 named structure member
** initialisation.
*/
# define CI_STRUCT_MBR(name, val)	val

#define CI_HF
#define CI_HV

#define CI_ALIGN(x) 
#define CI_HIDDEN

#define CI_PRINTF_LIKE(a,b)

typedef unsigned short sa_family_t;

#endif  /* __CI_COMPAT_MSVC_H__ */

/*! \cidoxg_end */
