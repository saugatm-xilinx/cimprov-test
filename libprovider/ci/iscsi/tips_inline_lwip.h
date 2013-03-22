/**************************************************************************\
*//*! \file tips.h
** <L5_PRIVATE L5_HEADER >
** \author  kjm
**  \brief  API for accessing TCP stack
**   \date  2008/08/11
**    \cop  (c) Solarflare Communications
** </L5_PRIVATE>
*//*
\**************************************************************************/


#ifndef __CI_INTERNAL_TIPS_INLINE_LWIP_H__
#define __CI_INTERNAL_TIPS_INLINE_LWIP_H__

#ifdef __ci_storport
#include <lwip/ip.h>
#include <lwip/tcp.h>
#else
#include <lwip/ip.h>
#include <lwip/tcp.h>
#endif
    // def __ci_storport

#include <ci/iscsi/sis_internal.h>

ci_inline int tips_stack_ctor(tips_stack *stack, ef_driver_handle fd, 
                              unsigned flags)
{
  return 0;
}

ci_inline int tips_stack_dtor(tips_stack *stack)
{
  return 0;
}

ci_inline int tips_rx_callback_set(tips_stack *stack, 
                                   void (*fn)(void *arg, int why))
{
  stack->RxEvent = fn;
  return 0;
}

ci_inline int tips_rx_callback_arm(tips_stack *stack, tips_tcp_socket *ts, 
                                   void *arg)
{
  return 0;
}

ci_inline int tips_rx_callback_disarm(tips_stack *stack, tips_tcp_socket *ts)
{
  return 0;
}

extern tips_socket TipsSocket;

ci_inline int tips_ep_ctor(tips_socket *ep, tips_stack *stack, 
                           ci_fixed_descriptor_t os_sock_fd)
{
  ep->netif = stack;
  ep->s = TipsSocket.s;

  return 0;
}

ci_inline int tips_sendmsg (tips_stack *ni, tips_tcp_socket *ts,
                            const struct msghdr *msg,
                            int flags, ci_addr_spc_t addr_spc )
{
  int nRetVal = 0;
  int i;

  for( i=0 ; i<(int)msg->msg_iovlen ; i++ ) {
    if( 0 < CI_IOVEC_LEN(&msg->msg_iov[i]) ) {
      u8_t write_flags = TCP_WRITE_FLAG_COPY;
      if( (i+1)!=msg->msg_iovlen ) {
        write_flags |= TCP_WRITE_FLAG_MORE;
      }
      if( !tcp_write((struct tcp_pcb *)ts,
                            CI_IOVEC_BASE(&msg->msg_iov[i]),
                            (u16_t)CI_IOVEC_LEN(&msg->msg_iov[i]),
                            write_flags ) ) {
        nRetVal += CI_IOVEC_LEN(&msg->msg_iov[i]);
      } else {
        return -1;
      }
    }
  }

  tcp_output((struct tcp_pcb *)ts);

  return nRetVal;
}

extern ci_iscsi_connection *
StorIscsiGetConnection(
  IN  PSTOR_ADAPTER_CONTEXT  pAdapter);


ci_inline int tips_tcp_rcv_usr(tips_tcp_socket *ts, tips_stack *ni)
{
  ci_iscsi_connection *pConn = StorIscsiGetConnection(ni->pAdapter);

  return pConn->nRawDataLen;
}

ci_inline int tips_recvmsg_args_init (tips_recvmsg_args *args, tips_stack *ni,
                            tips_tcp_socket *ts,
                            const struct msghdr *msg,
                            int flags, ci_addr_spc_t addr_spc )
{
  args->ni  = ni;
  args->ts  = ts;
  args->msg = (struct msghdr *)msg;
  args->flags = flags;

  return 0;
}

ci_inline int tips_recvmsg (tips_recvmsg_args *args )
{
  ci_iscsi_connection *pConn = StorIscsiGetConnection(args->ni->pAdapter);
  int i;
  int nRetVal = 0;

  for ( i=0 ; i<(int)args->msg->msg_iovlen ; i++ ) {
    ci_iovec *iov = &args->msg->msg_iov[i];

    if( pConn->nRawDataLen <= (int)CI_IOVEC_LEN(iov) ) {
      nRetVal += pConn->nRawDataLen;
      memcpy( CI_IOVEC_BASE(iov), pConn->pRawData, pConn->nRawDataLen );
      pConn->pRawData = NULL;
      pConn->nRawDataLen = 0;
      break;                            // all data consumed
    } else {
      nRetVal += CI_IOVEC_LEN(iov); 
      memcpy( CI_IOVEC_BASE(iov), pConn->pRawData, CI_IOVEC_LEN(iov) ); 
      pConn->pRawData += CI_IOVEC_LEN(iov); 
      pConn->nRawDataLen -= CI_IOVEC_LEN(iov); 
    }
  }

  return nRetVal;
}

ci_inline int tips_recvmsg_get (tips_recvmsg_args *args, ci_iovec_ptr *pIov )
{
  ci_iscsi_connection *pConn = StorIscsiGetConnection(args->ni->pAdapter);
  int nRetVal = 0;
  ci_iovec *iov = &pIov->io;

  if( pConn->nRawDataLen <= (int)CI_IOVEC_LEN(iov) ) {
    nRetVal += pConn->nRawDataLen;
    memcpy( CI_IOVEC_BASE(iov), pConn->pRawData, pConn->nRawDataLen );
    pConn->pRawData = NULL;
    pConn->nRawDataLen = 0;
  } else {
    nRetVal += CI_IOVEC_LEN(iov); 
    memcpy( CI_IOVEC_BASE(iov), pConn->pRawData, CI_IOVEC_LEN(iov) ); 
    pConn->pRawData += CI_IOVEC_LEN(iov); 
    pConn->nRawDataLen -= CI_IOVEC_LEN(iov); 
  }

  if ( nRetVal > 0 ) {
    pIov->iovlen = 1;
  }

  return nRetVal;
}

ci_inline int tips_connect(tips_socket *ep, const struct sockaddr* serv_addr, 
                           socklen_t addrlen, ci_fd_t fd, int flags)
{
  return 0;
}

ci_inline int tips_close(tips_socket *ep)
{
  return 0;
}

ci_inline int tips_abort(tips_socket *ep)
{
  return 0;
}

ci_inline int tips_async_tx(tips_stack *ni, tips_tcp_socket *ts,
                            struct ci_mem_desc *md, int items, int entries, int *flags)
{
  return 0;
}

ci_inline int tips_stack_is_locked(tips_stack *ni)
{
  return 0;
}

ci_inline int tips_stack_trylock(tips_stack *ni)
{
  return 0;
}

ci_inline int tips_tcp_state(tips_tcp_socket *ts)
{
  ci_uint32 dwRetVal = 0;

  switch ( ts->state )
  {
    case CLOSED:     dwRetVal = CI_TCP_STATE_NOT_CONNECTED; break;
    case LISTEN:     dwRetVal = CI_TCP_STATE_NOT_CONNECTED; break;
    case SYN_SENT:   dwRetVal = CI_TCP_STATE_NOT_CONNECTED; break;
    case SYN_RCVD:   dwRetVal = CI_TCP_STATE_TCP_CONN;      break;
    case ESTABLISHED:dwRetVal = CI_TCP_STATE_TCP_CONN|CI_TCP_STATE_CAN_FIN; break;
    case FIN_WAIT_1: dwRetVal = CI_TCP_STATE_RECVD_FIN;     break;
    case FIN_WAIT_2: dwRetVal = CI_TCP_STATE_RECVD_FIN;     break;
    case CLOSE_WAIT: dwRetVal = CI_TCP_STATE_NOT_CONNECTED; break;
    case CLOSING:    dwRetVal = CI_TCP_STATE_NOT_CONNECTED; break;
    case LAST_ACK:   dwRetVal = CI_TCP_STATE_NOT_CONNECTED; break;
    case TIME_WAIT:  dwRetVal = CI_TCP_STATE_NOT_CONNECTED; break;
    default:                                                break;
  }

  return dwRetVal;
}

ci_inline int tips_set_tcp_sndlowat(tips_tcp_socket *ts)
{
  return 0;
}

ci_inline int tips_tcp_eff_mss(tips_tcp_socket *ts )
{ 
  return ts->mss;
}

ci_inline int tips_set_tcp_rcvlowat(tips_tcp_socket *ts, ci_int32 lowat)
{
  return 0;
}

ci_inline int tips_get_tcp_rcvlowat(tips_tcp_socket *ts)
{
  return 0;
}

ci_inline int tips_socket_set_rcvtimeo(tips_socket *ep, ci_uint32 period )
{
  return 0;
}

ci_inline int tips_socket_set_sndtimeo(tips_socket *ep, ci_uint32 period )
{
  return 0;
}

ci_inline int tips_destroy(tips_socket *ep)
{
  return 0;
}

ci_inline int tips_stack_get_stats(tips_stack *ni, struct tips_stack_stats *stats )
{
  return 0;
}

#endif /* __CI_INTERNAL_TIPS_INLINE_LWIP_H__ */

