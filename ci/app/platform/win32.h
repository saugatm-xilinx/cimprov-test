/**************************************************************************\
*//*! \file
** <L5_PRIVATE L5_OPEN>
** \author  
**  \brief  
**   \date  
**    \cop  (c) Level 5 Networks Limited.
** </L5_PRIVATE>
*//*
\**************************************************************************/

/*! \cidoxg_include_ci_app_platform */

#ifndef __CI_APP_PLATFORM_WIN32_H__
#define __CI_APP_PLATFORM_WIN32_H__

/****************************************************************************
 *
 * multi-platform support for sockets error handling
 * (also see app/net.h)
 *
 ****************************************************************************/

# include <io.h>

# define CI_SOCK_INV(x)  ((x) == INVALID_SOCKET)
# define CI_SOCK_ERR(x)  ((x) == SOCKET_ERROR)
# define CI_SOCK_ERRNO() (WSAGetLastError())

# define CI_SOCK_ERR_INIT() static LPVOID _ci_lpMsgBuf;
# define CI_SOCK_ERR_GET(err) 				\
{							\
  err = WSAGetLastError();				\
  FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |	\
	    FORMAT_MESSAGE_FROM_SYSTEM | 		\
	    FORMAT_MESSAGE_IGNORE_INSERTS,		\
	    NULL,					\
	    err,					\
	    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),	\
	    (LPTSTR) &_ci_lpMsgBuf,			\
	    0,						\
	    NULL);					\
}
# define CI_SOCK_ERR_STR() (LPTSTR) &_ci_lpMsgBuf
# define CI_SOCK_ERR_PUT() LocalFree( _ci_lpMsgBuf );


#define STDIN_FILENO   0
#define STDOUT_FILENO  1
#define STDERR_FILENO  2

#define SHUT_RD    SD_RECEIVE
#define SHUT_WR    SD_SEND
#define SHUT_RDWR  SD_BOTH

typedef int socklen_t;
typedef int ssize_t;

#endif

/*! \cidoxg_end */
