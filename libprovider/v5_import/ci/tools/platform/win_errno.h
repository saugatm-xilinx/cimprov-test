/**************************************************************************\
*//*! \file
** <L5_PRIVATE L5_HEADER >
** \author  stg/cgg
**  \brief  error codes used in Windows Ul & drivers
**   \date  2007/01/09
**    \cop  (c) Solarflare Communications
** </L5_PRIVATE>
*//*
\**************************************************************************/

/*! \cidoxg_include_ci_tools_platform  */

#ifndef __CI_TOOLS_WIN__ERRNO_H__
#define __CI_TOOLS_WIN__ERRNO_H__

#include <winerror.h>

#ifdef _INC_ERRNO
/* errno.h has been included */
#pragma message( "Included errno.h before win32.h or win32_kernel.h" )

#undef EPERM
#undef ENOENT
#undef ESRCH
#undef EINTR
#undef EIO
#undef ENXIO
#undef E2BIG
#undef ENOEXEC
#undef EBADF
#undef ECHILD
#undef EAGAIN
#undef ENOMEM
#undef EACCES
#undef EFAULT
#undef EBUSY
#undef EEXIST
#undef EXDEV
#undef ENODEV
#undef ENOTDIR
#undef EISDIR
#undef EINVAL
#undef ENFILE
#undef EMFILE
#undef ENOTTY
#undef EFBIG
#undef ENOSPC
#undef EMLINK
#undef EPIPE
#undef ENETDOWN
#undef ENETUNREACH
#undef ENETRESET
#undef ECONNABORTED
#undef ECONNRESET
#undef ENOBUFS
#undef EISCONN
#undef ENOTCONN
#undef ESHUTDOWN
#undef ETOOMANYREFS
#undef ETIMEDOUT
#undef ECONNREFUSED
#undef ENOTSUP
#undef EDOM
#undef ERANGE
#undef EDEADLK
#undef ENAMETOOLONG
#undef ENOLCK
#undef ENOSYS
#undef ENOTEMPTY
#undef EILSEQ
#undef EDEADLOCK

#endif /* _INC_ERRNO */


#ifdef _INC_MATH
/* math.h has been included */
#pragma message( "Included math.h before win32.h or win32_kernel.h" )

#undef EDOM
#undef ERANGE

#endif /* _INC_MATH */


/* Internally we use the same error symbols as Linux/Unix */
/* Default xlat.  One entry for every value from 0 to 125 */
#define EPERM     WSAEACCES
#define ENOENT    ERROR_FILE_NOT_FOUND
#define ESRCH     ERROR_PATH_NOT_FOUND /*??*/
#define EINTR     WSAEINTR
#define EIO       ERROR_IO_DEVICE
#define ENXIO     ERROR_BAD_UNIT
#define E2BIG     WSASYSCALLFAILURE /*??*/
#define EFBIG     WSASYSCALLFAILURE /*??*/
#define ENOEXEC   WSASYSCALLFAILURE
#define EBADF     WSAEBADF
#define EAGAIN    WSAEWOULDBLOCK
#define EWOULDBLOCK WSAEWOULDBLOCK
#define ENOMEM    ERROR_NOT_ENOUGH_MEMORY
#define EACCES    WSAEACCES
#define EFAULT    WSAEFAULT
#define EBUSY     ERROR_BUSY
#define ENODEV    ERROR_DEV_NOT_EXIST
#define EINVAL    WSAEINVAL
#define EEXIST    ERROR_ALREADY_EXISTS
#define EMFILE    WSAEMFILE
#define ENOTTY    ERROR_BAD_UNIT
#define ENOSPC    ERROR_DISK_FULL
#define EMLINK    WSASYSCALLFAILURE /*! \todo iSCSI */
#define EPIPE     WSAESHUTDOWN
#define ETOOMANYREFS    WSAETOOMANYREFS /* Too many references */
#define EDOM            ERROR_INVALID_PARAMETER
#define ENAMETOOLONG    ERROR_BUFFER_OVERFLOW
#define ENOSYS          WSASYSCALLFAILURE /*!\todo iSCSI, cplace */
#define ELOOP           WSASYSCALLFAILURE
#define EUNATCH         WSASYSCALLFAILURE /*!\todo iSCSI, cplace */
#define ENODATA         WSANO_DATA
#define EREMOTE         WSAEREMOTE
#define ENOLINK         WSASYSCALLFAILURE /*!\todo iSCSI, cplace */
#define ECOMM           WSASYSCALLFAILURE /*!\todo iSCSI, cplace */
#define EPROTO          WSAEPROTONOSUPPORT
#define EBADMSG         WSASYSCALLFAILURE
#define EOVERFLOW       ERROR_INVALID_DATA
#define ELIBACC         ERROR_INVALID_DLL
#define EILSEQ          WSASYSCALLFAILURE
#define ERESTART        WSASYSCALLFAILURE
#define EUSERS          WSAEUSERS
#define ENOTSOCK        WSAENOTSOCK
#define EDESTADDRREQ    WSAEDESTADDRREQ
#define EMSGSIZE        WSAEMSGSIZE
#define EPROTOTYPE      WSAEPROTOTYPE
#define ENOPROTOOPT     WSAENOPROTOOPT
#define EPROTONOSUPPORT WSAEPROTONOSUPPORT
#define ESOCKTNOSUPPORT WSAESOCKTNOSUPPORT
#define EOPNOTSUPP      WSAEOPNOTSUPP
#define ENOTSUP         WSAEOPNOTSUPP
#define EPFNOSUPPORT    WSAEPFNOSUPPORT
#define EAFNOSUPPORT    WSAEAFNOSUPPORT
#define EADDRINUSE      WSAEADDRINUSE
#define EADDRNOTAVAIL   WSAEADDRNOTAVAIL
#define ENETDOWN        WSAENETDOWN
#define ENETUNREACH     WSAENETUNREACH
#define ENETRESET       WSAENETRESET
#define ECONNABORTED    WSAECONNABORTED
#define ECONNRESET      WSAECONNRESET
#define ENOBUFS         WSAENOBUFS
#define EISCONN         WSAEISCONN
#define ENOTCONN        WSAENOTCONN
#define ESHUTDOWN       WSAESHUTDOWN
#define ETIMEDOUT       WSAETIMEDOUT
#define ECONNREFUSED    WSAECONNREFUSED
#define EHOSTDOWN       WSAEHOSTDOWN
#define EHOSTUNREACH    WSAEHOSTUNREACH
#define EALREADY        WSAEALREADY
#define EINPROGRESS     WSAEINPROGRESS
#define ESTALE          WSAESTALE
#define EDQUOT          WSAEDQUOT
#define ECANCELED       WSAECANCELLED

#endif  /* __CI_TOOLS_WIN__ERRNO_H__ */

/*! \cidoxg_end */
