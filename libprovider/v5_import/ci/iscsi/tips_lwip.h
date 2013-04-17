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


#ifndef __CI_INTERNAL_TIPSLWIP_H__
#define __CI_INTERNAL_TIPSLWIP_H__

#ifdef __ci_storport
#include <lwip/ip.h>
#include <lwip/tcp.h>
#else
#include <lwip/ip.h>
#include <lwip/tcp.h>
#endif
    // def __ci_storport

#include <ci/iscsi/sis_internal.h>

typedef void *ci_os_scatterlist_t;

#define CI_TCP_HELPER_CALLBACK_RX_DATA 1
#define CI_TCP_HELPER_CALLBACK_CLOSED  2




/*
 * Atomic stuff.   CND beware.
 */
typedef struct {
  volatile ci_uint32 n;
} oo_atomic_t;

ci_inline ci_int32 oo_atomic_read(const oo_atomic_t* a) { return a->n; }
ci_inline void oo_atomic_set(oo_atomic_t* a, ci_int32 v) { a->n = v; }

ci_inline void oo_atomic_inc(oo_atomic_t* a)
{ ci_atomic32_inc(&a->n); }

ci_inline int oo_atomic_dec_and_test(oo_atomic_t* a)
{ return ci_atomic32_dec_and_test(&a->n); }

ci_inline void oo_atomic_add(oo_atomic_t* a, int n)
{ ci_atomic32_add(&a->n, n); }

/*
 * End of the atomic stuff.
 */


/*
 * TIPS-required stuff.
 * TODO: The following need properly defining.
 */
typedef void *ef_driver_handle;
typedef void *ci_addr_spc_t;
typedef void *ci_fd_t;
typedef void *ci_timeval_t;

#define NI_ID(_n)                0

#define CI_TCP_STATE_SYNCHRONISED	0x001
#define CI_TCP_STATE_SLOW_PATH		0x002
#define CI_TCP_STATE_NOT_CONNECTED	0x004
#define CI_TCP_STATE_CAN_FIN		0x008
#define CI_TCP_STATE_ACCEPT_DATA	0x010
#define CI_TCP_STATE_TXQ_ACTIVE		0x020
#define CI_TCP_STATE_NO_TIMERS		0x040
#define CI_TCP_STATE_TIMEOUT_ORPHAN	0x080
#define CI_TCP_STATE_SOCKET		0x100
#define CI_TCP_STATE_TCP		0x200
#define CI_TCP_STATE_TCP_CONN		0x400
#define CI_TCP_STATE_RECVD_FIN		0x800

#define CI_IP_PRINTF_FORMAT       "%d.%d.%d.%d"
#define CI_IP_PRINTF_ARGS(p_be32) ((int) ((ci_uint8*)(p_be32))[0]), \
                                  ((int) ((ci_uint8*)(p_be32))[1]), \
                                  ((int) ((ci_uint8*)(p_be32))[2]), \
                                  ((int) ((ci_uint8*)(p_be32))[3])


/*!
** lwip_stack
**
** This is the top-level representation of an LWIP stack.  It is the
** key-stone that provides access to the state of the stack.
**
** This data-structure is not shared: There is one copy per userlevel
** address space, and one in the kernel.  Therefore it does not contain any
** of the "state" of the stack, merely description of what that state
** might be.
*/
struct _STOR_ADAPTER_CONTEXT; 
typedef struct _STOR_ADAPTER_CONTEXT  STOR_ADAPTER_CONTEXT, *PSTOR_ADAPTER_CONTEXT;


struct lwip_stack_s;
typedef struct lwip_stack_s lwip_stack;


struct lwip_stack_s {
  /* General flags */  
  /*
   * TODO:: STANBELL doesn't understand what effect these flags have, or
   *        what components they affect. The values and names were copied
   *        from "ci/internal/ip_types.h".
   */
  unsigned             flags;
  /* netif was once (and maybe still is) shared between multiple processes */
# define CI_NETIF_FLAGS_SHARED           1       
  /* netif is protected from destruction with an extra ref_count */
# define CI_NETIF_FLAGS_DTOR_PROTECTED   2
  /* netif is a kernel-only stack and thus is trusted */
# define CI_NETIF_FLAGS_IS_TRUSTED       4
  /* netif state is broken */
# define CI_NETIF_FLAGS_IS_BROKEN        8
  /* Use physical addressing mode */
# define CI_NETIF_FLAGS_PHYS_ADDR_MODE   16
  /* Use reserved iSCSI event/DMA queues */
# define CI_NETIF_FLAGS_ISCSI            32
  /* Use interrupting event queue */
# define CI_NETIF_FLAGS_INTERRUPT        64

  /* The Incoming event callback. Data or lost connection */
  void  (*RxEvent)(void *arg, int why);

  /* The STORPORT Adapter structure */
  PSTOR_ADAPTER_CONTEXT pAdapter;
};
typedef lwip_stack tips_stack;


/*
 * This is in stor_iscsi, there should probably be a better (more
 * abstract) way to find it, but this will do for now. I can't see
 * we'll ever have more than 1.
 */
extern tips_stack         TipsStack;


/*
 * End of TIPS-required stuff.
 */

/*
 * Descriptor stuff.
 */
typedef int ci_descriptor_t;
#define  CI_INVALID_DESCRIPTOR ((ci_descriptor_t)-1)
#define  from_descriptor( desc ) desc
#define  to_descriptor( desc ) ((ci_descriptor_t)(ULONG_PTR)(desc))
#define  IS_VALID_DESCRIPTOR(fd) ((fd)!=CI_INVALID_DESCRIPTOR)
#ifdef _WIN64
# define  DESCRIPTOR_FMT "%I64u"
#else
# define  DESCRIPTOR_FMT "%u"
#endif
/*
 * End of descriptor stuff.
 */

/*
 * TCP Stuff.
 */
// extern int ci_tcp_close(tips_stack *, ci_tcp_state* ts) CI_HF;


#define CI_MSG_NETIF_LOCKED         0x80000000

/*! Return value where a handover is required by a higher layer. */
#define CI_SOCKET_HANDOVER -2

/*
 * End of TCP Stuff.
 */

/*
 * Sockets.  Note that for sfcstor these are not BSD sockets.
 */
#define AF_INET     2
extern const char* /*??ci_*/ip_addr_str(unsigned addr_be32) CI_HF;
#define CI_INVALID_SOCKET -1

/* members are in network byte order */
struct sockaddr_in {
  u8_t sin_len;
  u8_t sin_family;
  u16_t sin_port;
  struct in_addr sin_addr;
  char sin_zero[8];
};

struct sockaddr {
  u8_t sa_len;
  u8_t sa_family;
  char sa_data[14];
};

/* Netif ID, TCP state ID */
#define NT_FMT "%d:%d "
// #define NT_PRI_ARGS(n,t) NI_ID(n), S_FMT(t)
#define NT_PRI_ARGS(n,t) NI_ID(n), (t)

#define OO_SP_TO_INT(p)         (p)
#define S_ID(ss)                OO_SP_TO_INT(S_SP(ss))
#define S_FMT(s)                ((int) S_ID(s))
#define SC_SP(s)                ((citp_waitable_obj*) (s))

/*
 * End of sockets.
 */

/* Define this to see where code is looking inside these types.  This
 * won't produce a usable bit of code, but will show up where there
 * are problems with type abstraction */
#define TIPS_OPAQUE 0

#if TIPS_OPAQUE

typedef struct tips_stack {
  tips_netif _ni;
} tips_stack;

typedef struct tips_tcp_socket {
  tips_tcp_state _ts;
} tips_tcp_socket;

typedef struct tips_socket {
  tips_stack *netif;
  tips_tcp_socket *s;
} tips_socket;

#define TIPS_STACK_TO_NETIF(x) (&((x)->_ni))
#define TIPS_TCP_SOCKET_TO_TCP(x) (&((x)->_ts))
#define TIPS_SOCKET_TO_XXXX_SOCKET(x) (&((x)->_ep))

#define TIPS_SOCKET_STACK(x) (x)->netif
#define TIPS_SOCKET_TCP_SOCKET(x) (x)->s

#else

/* An implementation of tips_socket must provide at least the
 * following structure, or ensure everything uses TIPS_SOCKET_STACK
 * and TIPS_SOCKET_TCP_SOCKET macros to access them in another place:
 *
 *   typedef struct tips_socket_s {
 *     tips_stack *netif;
 *     tips_tcp_socket *s;
 *   } tips_socket;
 */

/*
 * tcp_pcb is the LWIP TCP endpoint representation.
 */
typedef struct tcp_pcb tips_tcp_socket;

typedef struct{
  tips_stack      *netif;
  tips_tcp_socket *s;
} lwip_socket;

typedef lwip_socket tips_socket;

#define TIPS_STACK_TO_NETIF(x) ((netif *)(x))
#define TIPS_TCP_SOCKET_TO_TCP(x) ((ci_tcp_state *)(x))

// typedef ci_sock_cmn tips_tcp_socket;


#define netif2tcp_helper_resource(ni)                   \
  CI_CONTAINER(tcp_helper_resource_t, netif, (ni))

#define TIPS_SOCKET_TO_CITP_SOCKET(x) ((citp_socket *)(x))

#define TIPS_SOCKET_STACK(x) (x)->netif
#define SOCK_TO_TCP(x)        (tips_tcp_socket *)(x)
#define TIPS_SOCKET_TCP_SOCKET(x) SOCK_TO_TCP((x)->s);

#endif


/* Arguments to tips_recvmsg(). */
typedef struct tips_recvmsg_args {
  tips_stack *      ni;
  tips_tcp_socket*  ts;
  struct msghdr*    msg;
  int               flags;
} tips_recvmsg_args;

extern VOID
StorEventPoll( 
  IN  PSTOR_ADAPTER_CONTEXT pAdapter
  );

struct tips_stack_stats {
  unsigned n_fails;
#ifndef NDEBUG
  unsigned n_allocs;
  unsigned n_frees;
#endif
};

#endif /* __CI_INTERNAL_TIPSLWIP_H__ */

