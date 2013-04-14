/**************************************************************************\
*//*! \file
** <L5_PRIVATE L5_HEADER>
** \author  djr
**  \brief  Hook to select a particular sockets implementation.
**   \date  2004/04/06
**    \cop  (c) Level 5 Networks Limited.
** </L5_PRIVATE>
*//*
\**************************************************************************/

/*! \cidoxg_include_ci_app */
#ifndef __CI_APP_SOCKET_H__
#define __CI_APP_SOCKET_H__


#ifndef CIS_SOCK_MK_ID
# ifdef __unix__
#  include <sys/socket.h>
#  include <sys/poll.h>
#  define closesocket		close
# endif
# define CIS_SOCK_MK_ID(x)	x
# define CIS_SOCK_HAVE_SOCKOPT	1
# define CIS_SOCK_HAVE_SOCKNAME	1
# define CIS_SOCK_HAVE_SELECT	1
# ifdef __unix__
#  define CIS_SOCK_HAVE_POLL	1
# else
#  define CIS_SOCK_HAVE_POLL	0
# endif
# define CIS_SOCK_HAVE_EPOLL	0
# define setsockblocking	ci_setfdblocking

#endif


#define cis_socket		CIS_SOCK_MK_ID(socket)
#define cis_bind		CIS_SOCK_MK_ID(bind)
#define cis_connect		CIS_SOCK_MK_ID(connect)
#define cis_listen		CIS_SOCK_MK_ID(listen)
#define cis_accept		CIS_SOCK_MK_ID(accept)
#define cis_shutdown		CIS_SOCK_MK_ID(shutdown)
#define cis_close		CIS_SOCK_MK_ID(closesocket)
#define cis_dup 		CIS_SOCK_MK_ID(dup)
#define cis_dup2 		CIS_SOCK_MK_ID(dup2)
#define cis_setsockblocking	CIS_SOCK_MK_ID(setsockblocking)
#define cis_sendfile		CIS_SOCK_MK_ID(sendfile)

#ifdef __unix__
#define cis_recv		CIS_SOCK_MK_ID(recv)
#define cis_send		CIS_SOCK_MK_ID(send)
#ifdef __linux__
#define cis_accept4		CIS_SOCK_MK_ID(accept4)
#endif
#endif
#ifdef _WIN32
#define _cis_recv		CIS_SOCK_MK_ID(recv)
#define _cis_send		CIS_SOCK_MK_ID(send)

/* Wrapper to add support for MSG_WAITALL on windows */
ci_inline ssize_t cis_recv(int s, void *buf, size_t len, int flags){
#if 0 /* (some versions of) windows seem to provide this already, so
         no need, but leaving in CVS incase needed in future */
  if(flags & MSG_WAITALL){
    unsigned bytes = 0;
    int rc, new_flags, iter = 0;

    new_flags = flags &~ MSG_WAITALL;

    while(bytes < len){
      if((rc = _cis_recv(s, ((char *)buf)+bytes, len-bytes, new_flags)) < 0)
        return rc;
      bytes += rc;
    }
    return bytes;
  }
#endif
  if(flags & MSG_DONTWAIT){
    int nonblock, new_flags, rc, error;
    /* TODO should find out if it's already non-blocking, and restore
       that afterwards, but no idea how to read the current status on
       Windows, so for now assume that it's a blocking socket */

    /* Set non-blocking */
    nonblock = 1;
    ioctlsocket(s, FIONBIO, &nonblock);

    new_flags = flags &~ MSG_DONTWAIT;
    rc = _cis_recv(s, buf, len, new_flags);

    error =  WSAGetLastError();

    /* Set blocking */
    nonblock = 0;
    ioctlsocket(s, FIONBIO, &nonblock);
    
    WSASetLastError(error);

    return rc;
  }
  return _cis_recv(s, buf, len, flags);
}

/* Wrapper to add support for MSG_WAITALL on windows */
ci_inline ssize_t cis_send(int s, const void *buf, size_t len, int flags){
#if 0 /* WAITALL on a send is not particularly meaningful, so not
         including for now */
  if(flags & MSG_WAITALL){
    unsigned bytes = 0;
    int rc, new_flags, iter = 0;

    new_flags = flags &~ MSG_WAITALL;

    while(bytes < len){
      if((rc = _cis_send(s, ((char *)buf)+bytes, len-bytes, new_flags)) < 0)
        return rc;
      bytes += rc;
    }
    return bytes;
  }
#endif
  if(flags & MSG_DONTWAIT){
    int nonblock, new_flags, rc, error;
    /* TODO should find out if it's already non-blocking, and restore
       that afterwards, but no idea how to read the current status on
       Windows, so for now assume that it's a blocking socket */

    /* Set non-blocking */
    nonblock = 1;
    ioctlsocket(s, FIONBIO, &nonblock);

    new_flags = flags &~ MSG_DONTWAIT;
    rc = _cis_send(s, buf, len, new_flags);

    error =  WSAGetLastError();

    /* Set blocking */
    nonblock = 0;
    ioctlsocket(s, FIONBIO, &nonblock);
    
    WSASetLastError(error);

    return rc;  
  }
  return _cis_send(s, buf, len, flags);
}

#undef _cis_recv
#undef _cis_send
#endif
#define cis_recvfrom		CIS_SOCK_MK_ID(recvfrom)
#define cis_sendto		CIS_SOCK_MK_ID(sendto)
#define cis_recvmsg		CIS_SOCK_MK_ID(recvmsg)
#define cis_sendmsg		CIS_SOCK_MK_ID(sendmsg)
#define cis_read		CIS_SOCK_MK_ID(read)
#define cis_write		CIS_SOCK_MK_ID(write)
#define cis_writev		CIS_SOCK_MK_ID(writev)


#ifdef CIS_SOCK_HAVE_SOCKOPT
# define cis_getsockopt		CIS_SOCK_MK_ID(getsockopt)
# define cis_setsockopt		CIS_SOCK_MK_ID(setsockopt)
#endif
#ifdef CIS_SOCK_HAVE_SOCKNAME
# define cis_getsockname	CIS_SOCK_MK_ID(getsockname)
# define cis_getpeername	CIS_SOCK_MK_ID(getpeername)
#endif
#ifdef CIS_SOCK_HAVE_SELECT
# define cis_select		CIS_SOCK_MK_ID(select)
#endif
#ifdef CIS_SOCK_HAVE_POLL
# define cis_poll		CIS_SOCK_MK_ID(poll)
#endif
#ifdef CIS_SOCK_HAVE_EPOLL
# define cis_epoll_ctl		CIS_SOCK_MK_ID(epoll_ctl)
# define cis_epoll_wait		CIS_SOCK_MK_ID(epoll_wait)
#endif


#endif  /* __CI_APP_SOCKET_H__ */
/*! \cidoxg_end */
