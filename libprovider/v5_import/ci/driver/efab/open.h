/**************************************************************************\
*//*! \file
** <L5_PRIVATE L5_HEADER >
** \author  djr
**  \brief  Public types and defs for driver & h/w interface.
**   \date  2002/02/04
**    \cop  (c) Level 5 Networks Limited.
** </L5_PRIVATE>
*//*
\**************************************************************************/

/*! \cidoxg_include_ci_driver_efab  */
#ifndef __CI_DRIVER_EFAB_OPEN_H__
#define __CI_DRIVER_EFAB_OPEN_H__

#ifndef __CI_TOOLS_H__
# include <ci/tools.h>
#endif


# ifdef __KERNEL__
     struct efhw_nic_s;
     typedef struct efhw_nic_s* ci_fd_t;
#  define CI_FD_BAD ((ci_fd_t)(NULL))
#  ifdef _WIN32
     typedef HANDLE ci_descriptor_t;
#  else
     typedef int ci_descriptor_t;
#  endif
# else /* Userland */
#  ifdef _WIN32
     typedef HANDLE ci_fd_t;
#    define CI_FD_BAD ((ci_fd_t)(NULL))
     typedef HANDLE ci_descriptor_t;
#  else
     typedef int ci_fd_t;
#    define CI_FD_BAD ((ci_fd_t)(-1))
     typedef int ci_descriptor_t;
#  endif
# endif

/* identical in UL & kernel */
#  ifdef _WIN32
#    define  CI_INVALID_DESCRIPTOR ((ci_descriptor_t)-1)
#    define  from_descriptor( desc ) desc
#    define  to_descriptor( desc ) ((ci_descriptor_t)(ULONG_PTR)(desc))
#    define  IS_VALID_DESCRIPTOR(fd) ((fd)!=CI_INVALID_DESCRIPTOR)
#    ifdef _WIN64
#      define  DESCRIPTOR_FMT "%I64u"
#    else
#      define  DESCRIPTOR_FMT "%u"
#    endif
#  else
#    define  from_descriptor( desc ) desc
#    define  to_descriptor( desc ) ((ci_descriptor_t)(desc))
#    define  IS_VALID_DESCRIPTOR(fd) ((fd)>=0)
#    define  DESCRIPTOR_FMT "%d"
#  endif

#define  DESCRIPTOR_PRI_ARG(fd) fd


/*----------------------------------------------------------------------------
 *
 * Open hardware API - Internally this is compile time selectable 
 *
 *---------------------------------------------------------------------------*/


#endif  /* __CI_DRIVER_EFAB_OPEN_H__ */
/*! \cidoxg_end */
