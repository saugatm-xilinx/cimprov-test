/**************************************************************************\
*//*! \file
** <L5_PRIVATE L5_HEADER >
** \author  djr
**  \brief  Definition of ci_netif etc.
**   \date  2006/06/05
**    \cop  (c) Level 5 Networks Limited.
** </L5_PRIVATE>
*//*
\**************************************************************************/

/*! \cidoxg_include_ci_internal  */

#ifndef __CI_INTERNAL_IP_TYPES_H__
#define __CI_INTERNAL_IP_TYPES_H__

/*
** READ ME FIRST please.
**
** This header contains type definitions for the Etherfabric TCP/IP stack
** that do not form part of the state of the stack.  ie. These types are
** part of the support and infrastructure.
**
** The only stuff that may appear here is types and data structures,
** constants associated with fields in those data structures and
** documentation.
**
** NO CODE IN THIS FILE PLEASE.
*/



/*!
** ci_netif_nic_t
**
** The portion of a netif that corresponds to H/W resources and must be
** replicated per NIC.
*/
typedef struct ci_netif_nic_s {
  ef_vi                      vi;
#ifdef __KERNEL__
  struct oo_iobufset** pkt_rs;
#endif
} ci_netif_nic_t;


struct tcp_helper_endpoint_s;


/*!
** ci_netif
**
** This is the top-level representation of an Etherfabric stack.  It is the
** key-stone that provides access to the state of the stack.
**
** This data-structure is not shared: There is one copy per userlevel
** address space, and one in the kernel.  Therefore it does not contain any
** of the "state" of the stack, merely description of whether that state
** is.
*/
struct ci_netif_s {
  ci_magic_t           magic;
  efrm_nic_set_t       nic_set; 
  int                  nic_n;
  /* resources */
  ci_netif_nic_t       nic_hw[CI_CFG_MAX_INTERFACES];

  ci_netif_state*      state;

#ifndef __KERNEL__
  /* Use ci_netif_get_driver_handle() rather than this directly. */
  ef_driver_handle     driver_handle;
  unsigned             mmap_bytes;
  char*                io_ptr;
  char*                buf_ptr;
#endif

#ifdef __ci_driver__
  ci_int8              hwport_to_intf_i[CI_CFG_MAX_REGISTER_INTERFACES];
  ci_int8              intf_i_to_hwport[CI_CFG_MAX_INTERFACES];
  ci_int8              blacklist_intf_i[CI_CFG_MAX_BLACKLIST_INTERFACES];
  unsigned             blacklist_length;
  uid_t                uid;
  uid_t                euid;
# ifdef CI_HAVE_OS_NOPAGE
  ci_shmbuf_t          pages_buf;
# else
  ci_shmbuf_t**        k_shmbufs;
  unsigned             k_shmbufs_n;
# endif
#else
# ifndef CI_HAVE_OS_NOPAGE
  void **              u_shmbufs;
# endif
#endif

#if defined(_WIN32)
#if ! defined(__KERNEL__)
  HANDLE               ctx_thread; /* Thread used to process APCs */
  HANDLE               end_ctx_thread; /* Event signalled to
                                          terminate ctx_thread */
#endif
#endif

  cicp_ni_t            cplane;
    
#ifdef __KERNEL__
  /** eplock resource. Note that this has the SAME lifetime as [lock]. 
   *  The reference on this object is taken when the lock is created -
   *  and no other reference is taken. */
  eplock_helper_t      eplock_helper;
#endif

  ci_netif_filter_table* filter_table;

#ifdef __KERNEL__
  /** pkt resources, 1:1 mapping with [pkt_bufs]. Note that these have
   *  the SAME lifetime as [pkt_bufs]. Entry in this array MUST NOT be
   *  taken to imply that a ref. has been taken; it hasn't! */
  struct oo_iobufset** pkt_rs;
# if CI_CFG_PKTS_AS_HUGE_PAGES
  int                   huge_pages_flag;
# endif
#elif CI_CFG_MMAP_EACH_PKTSET
# if CI_CFG_PKTS_AS_HUGE_PAGES
  ci_int32             *pkt_shm_id;
# endif
  char**                pkt_sets; /* array of mmaped pkt sets */
#else
  char*                 pkt_sets; /* one area for all packets */
#endif
  /* At user level, this is the address space id of the process in
     which this netif resides (and so only set once).  In the kernel
     (on windows) this is the current address space that the kernel
     netif is using (and so can change), but on linux is a constant
     (CI_ADDR_SPC_ID_KERNEL), I think. */
  ci_addr_spc_id_t     addr_spc_id;

#ifdef __ci_driver__
  ci_contig_shmbuf_t   state_buf;
  unsigned             pkt_sets_n;
  unsigned             pkt_sets_max;
  ci_uint32            ep_ofs;           /**< Copy from ci_netif_state_s */

  /*! Trusted per-socket state. */
  struct tcp_helper_endpoint_s**  ep_tbl;
  unsigned                        ep_tbl_n;
  unsigned                        ep_tbl_max;
#endif

#ifndef __ci_driver__
  /* for table of active UL netifs (unix/netif_init.c) */
  ci_dllink            link;
  
  /* Number of active endpoints this process has in this UL netif.  Used as a
  ** reference count to govern the lifetime of the UL netif.
  */
  oo_atomic_t          ref_count;
#endif /* __ci_driver__ */

  /* General flags */  
  unsigned             flags;
  /* netif was once (and maybe still is) shared between multiple processes */
# define CI_NETIF_FLAGS_SHARED           0x1
  /* netif is protected from destruction with an extra ref_count */
# define CI_NETIF_FLAGS_DTOR_PROTECTED   0x2
  /* netif is a kernel-only stack and thus is trusted */
# define CI_NETIF_FLAGS_IS_TRUSTED       0x4
  /* netif state is broken */
# define CI_NETIF_FLAGS_IS_BROKEN        0x8
  /* Use physical addressing mode */
# define CI_NETIF_FLAGS_PHYS_ADDR_MODE   0x10
  /* Use reserved iSCSI event/DMA queues */
# define CI_NETIF_FLAGS_ISCSI            0x20
  /* Use interrupting event queue */
# define CI_NETIF_FLAGS_INTERRUPT        0x40
  /* Stack [k_ref_count] to be decremented when sockets close. */
# define CI_NETIF_FLAGS_DROP_SOCK_REFS   0x80
  /* Don't use this stack for new sockets unless name says otherwise */
# define CI_NETIF_FLAGS_DONT_USE_ANON    0x100

#if CI_CFG_CHIMNEY
  struct efx_ndis_adapter*  ndis_adapter;

  /* Pool of net buffer lists (each with one net buffer preallocated) */
  NDIS_HANDLE nblPool;
  /* Cache of free net buffer lists (each with one net buffer) */
  NET_BUFFER_LIST* nblCache;
  /* Number of net buffer lists currently allocated */
  ULONG nblAllocated;
  /* TODO keep track of NBLs passed to NDIS? */
  
  /* Atomic list of tcp_states that have yet to be orphaned and freed,
  ** although their offload has been terminated.
  */
  uintptr_t chimney_deferred_orphan_head;

  /* Atomic list of NBLs that have been returned from the terminate path */
  uintptr_t chimney_deferred_nbl_return;
#endif

#ifndef __KERNEL__
  double    ci_ip_time_tick2ms;     /* time for 1 tick in ms */
#endif

#ifdef __KERNEL__
  ci_netif_config_opts opts;
#endif

  /* Used from ci_netif_poll_evq() only.  Moved here to avoid stack
   * overflow. */
  ef_event      events[16];
  ef_request_id tx_events[EF_VI_TRANSMIT_BATCH];
  /* Another stack overflow avoidance, used from allocate_vi(). */
  char vi_data[VI_MAPPINGS_SIZE];
};

#if defined(_WIN32) && !defined(__KERNEL__)  

/* The following set of structures are used exclusively in the WSP.
 * They are included here to allow access to the upcall & downcall 
 * tables from the IP (core) library via citp_socket in place of 
 * the Unix system calls (ci_sys_bind() for example).
 */
#define wf_ext_proc_table_size 7

typedef struct _wf_ext_proc_table_t {
    LPFN_ACCEPTEX             lpWSPAcceptEx;
    LPFN_TRANSMITFILE         lpWSPTransmitFile;
    LPFN_GETACCEPTEXSOCKADDRS lpWSPGetAcceptExSockAddrs;
    LPFN_TRANSMITPACKETS      lpWSPTransmitPackets;
    LPFN_CONNECTEX            lpWSPConnectEx;
    LPFN_DISCONNECTEX         lpWSPDisconnectEx;
    LPFN_WSARECVMSG           lpWSPRecvMsg;
} wf_ext_proc_table_t;

typedef enum  {
  wf_pvdf_NULL=0,
  wf_pvdf_extension_functions_set=0x0001

} wf_provider_flags_t;

typedef struct _wf_provider_t {
  /* reference count of this providers startup */
  int startup_cnt; 

  /* Next provider in chain */
  WSAPROTOCOL_INFOW downstream_pvdr;

  /* This layered provider */
  WSAPROTOCOL_INFOW this_pvdr;

  /* Proc table of this provider */
  WSPPROC_TABLE* this_proc_tbl;

  wf_ext_proc_table_t* this_ext_proc_table;
  DWORD               dummy_lsp_id;

  /* Proc table of next provider.  This table provides access to 
   * the majority of the downcalls required. */
  WSPPROC_TABLE  downstream_proc_tbl;

  /* Proc table of next provider's extension, This table provides
   * the remainder of the downcalls (see [downstream_proc_tbl]. */ 
  wf_ext_proc_table_t downstream_ext_proc_tbl; 

  WCHAR downstream_pvdr_path[MAX_PATH];
  WCHAR library_path[MAX_PATH];
  int downstream_provider_path_len;

  WSPDATA downstream_startup_data;

  LPWSPSTARTUP      downstream_startup_fn;
  HMODULE           downstream_module;

  wf_provider_flags_t flags;

/*     CRITICAL_SECTION    critSec; */
} wf_provider_t;
#endif /* _WIN32 && !__KERNEL__ */

/*!
** citp_socket
**
** This is the keystone that provides access to a socket.  It provides
** access to the stack the socket lies in, and identifies the socket within
** that stack.
*/
struct citp_socket_s {
  ci_netif*            netif;
  ci_sock_cmn*         s;

  /* When SO_LINGER is used, this value checks that the endpoint was not
   * re-used for another socket. */
  ci_uint64            so_linger_hash;

#ifdef _WIN32
  ci_fd_t              win_sock;  /*!< Store for backing socket */
# if !defined(__KERNEL__)
  /* Downcall provider for this socket - this entry guarantees to have a
   * complete set of downcalls. */
  wf_provider_t*       provider;
  /* Protocol provider (database entry) for this socket. Cannot be used
   * for downcalls. */
  /*! \todo fixme, this duplication is daft! */
  wf_provider_t*       protocol_provider;
# endif

  /* Thanks to the behaviour of "*Ex" API & SO_UPDATE_*_CONTEXT we have to 
   * remember the remote addressing information of the last connection 
   * established even when a new connection is established on the socket.  
   * Once the SO_UPDATE_*_CONTEXT sockopt has been set we can sync these 
   * with the current addressing information.
   *
   * To fully emulate the cross-process socket support we also have to 
   * hold a separate copy in each process & update this copy when we
   * handle SO_UPDATE_ACCEPT_CONTEXT. Unpleasant enough?
   *
   * NOTE that there is a "cache active" flag in shared state - this is 
   * needed to ensure that cross-process inherited sockets all start to
   * use their local caches at the same time.
   */
  ci_uint32 prev_laddr_be32;
  ci_uint32 prev_raddr_be32;
  ci_uint16 prev_lport_be16;
  ci_uint16 prev_rport_be16;

  /* Sock opts that have to survive across connectex/update
   * (see winsock2/extensions/update_context) */
  ci_int32  so_debug;
#endif
};


/* Arguments to ci_tcp_recvmsg(). */
typedef struct ci_tcp_recvmsg_args {
  ci_netif*      ni;
  ci_tcp_state*  ts;
  struct msghdr* msg;
  int            flags;
#ifdef __KERNEL__
  ci_addr_spc_t  addr_spc;
#endif
} ci_tcp_recvmsg_args;

/* Arguments to ci_udp_sendmsg and ci_udp_recvmsg */
typedef struct ci_udp_iomsg_args {
  ci_udp_state  *us;
  ci_netif      *ni;
#ifndef __KERNEL__
  citp_socket   *ep;
  ci_fd_t        fd;
#else
  /* This one is required to call poll on filp from
   * recv */
  struct file   *filp;
  /* stored to speed up os socket recv */
#endif
} ci_udp_iomsg_args;

struct ci_netif_poll_state {
  oo_pkt_p  tx_pkt_free_list;
  oo_pkt_p* tx_pkt_free_list_insert;
  int       tx_pkt_free_list_n;
};



#endif  /* __CI_INTERNAL_IP_TYPES_H__ */
/*! \cidoxg_end */
