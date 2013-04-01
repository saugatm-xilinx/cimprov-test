/**************************************************************************\
*//*! \file iscsi_link_fns.h
** <L5_PRIVATE L5_HEADER >
** \author  aam, cgg
**  \brief  Common header for "direct function call" iscsi->kernel link API.
**   \date  2006/09/04
**    \cop  (c) Solarflare Communications
** </L5_PRIVATE>
*//*
\**************************************************************************/

#ifndef _EFAB_ISCSI_LINK_FNS_H
#define _EFAB_ISCSI_LINK_FNS_H

/* This header is used as a definition of this functional interface used
   between the main stack and, potentially, more than one independently-
   compiled body of code.

   It will be easier to maintain the code-base if no unnecessary definitions
   are included in this header.
*/

/** CHANGE THIS NUMBER WHENEVER YOU CHANGE struct ci_il_fns_t **/
#define EFAB_ISCSI_LINK_VERSION 7

typedef void ci_event_fn_t(void *arg);
typedef int ci_event_handle_t;
#define CI_EVENT_HANDLE_BAD (-1)

/** CHANGE EFAB_ISCSI_LINK_VERSION WHENEVER YOU CHANGE THIS STRUCT **/
typedef struct {
  int version;  /* Expect to be EFAB_ISCSI_LINK_VERSION */
  int /*rc*/ (*interface_open)(void);
  void (*interface_close)(void);
  int /*rc*/ (*register_unload_event)(ci_event_handle_t *out_event,
				      ci_event_fn_t *fn, void *arg);
  int /*rc*/ (*unregister_unload_event)(ci_event_handle_t);
  int (*tcp_ep_ctor)(citp_socket *, ci_netif *, ci_uint8,
                     ci_fixed_descriptor_t);
  int (*tcp_connect)(citp_socket *, const struct sockaddr *, socklen_t,
                     ci_fd_t, int);
  int (*tcp_close)(ci_netif *, ci_tcp_state *);
  int (*tcp_abort)(citp_socket *);
  int (*tcp_recvmsg)(ci_tcp_state *, ci_netif *, struct msghdr *, int,
                     ci_addr_spc_t, ci_boolean_t *);
  int (*tcp_sendmsg)(ci_netif *, ci_tcp_state *, const struct msghdr *,
                     int, ci_addr_spc_t);
  int (*tcp_recvmsg_get)(ci_netif *, ci_tcp_state *, ci_iovec_ptr *,
                         int, ci_addr_spc_t, ci_boolean_t *);
  void (*close_endpoint)(tcp_helper_resource_t *, int);
  int (*netif_ctor)(ci_netif *, ef_driver_handle, unsigned int);
  int (*netif_dtor)(ci_netif *);
  void (*netif_unlock)(ci_netif *);
  void (*netif_send)(ci_netif *, ci_ip_pkt_fmt *);
  ci_ip_pkt_fmt *(*netif_pkt_alloc_slow)(ci_netif *, int);
  int (*netif_pkt_wait)(ci_netif *, int);
  int (*csum_copy_iovec_to_pkt)(ci_netif *, ci_ip_pkt_fmt *, oo_offbuf *,
                                ci_iovec_ptr *, unsigned *, ci_boolean_t);
  unsigned (*ip_csum_partial)(unsigned, const volatile void *, int);
  int (*callback_arm)(tcp_helper_resource_t *, int, void *);
  int (*callback_disarm)(tcp_helper_resource_t *, int);
  int (*callback_set)(tcp_helper_resource_t *, void (*)(void *, int));
  const char* (*ip_addr_str)(ci_uint32);
  NTSTATUS (*copy_to_user)(void *, void *, unsigned int);
  int (*set_ioctl_handler) (void *, int(*)(void*));
  ci_driver_t *driver;
  int /*rc*/ (*hwport_get_mtu)(ci_hwport_id_t hwport, ci_mtu_t *ref_mtu);
  int (*hwport_isup)(ci_hwport_id_t hwport);
  void (*mac_manage_unused)(cicp_handle_t *control_plane,
                            int prune_regularly);
  int (*mac_manage_arpd)(cicp_handle_t *control_plane, int respond_arp_req);
  int (*manage_icmpd)(cicp_handle_t *control_plane, int respond_ping_req);
  int (*llap_declare)(cicp_handle_t *cplane_netif, ci_ifid_t ifindex,
                      ci_hwport_id_t hwport, cicp_encap_t *ref_encap);
  int (*llap_import)(cicp_handle_t *cplane_netif,
                     cicp_llap_rowid_t *out_rowid, ci_ifid_t ifindex,
                     ci_mtu_t mtu, ci_uint8 up, char *name,
                     ci_hwport_id_t hwport, ci_mac_addr_t *ref_mac,
                     cicp_encap_t *ref_encap);
  int (*llap_delete)(cicp_handle_t *cplane_netif, ci_ifid_t ifindex);
  int (*ipif_import)(cicp_handle_t *cplane_netif, 
                     cicp_ipif_rowid_t *out_rowid, ci_ifid_t ifindex,
                     ci_ip_addr_net_t net_ip, ci_ip_addrset_t net_ipset,
                     ci_ip_addr_net_t net_bcast);
  int (*ipif_delete)(cicp_handle_t *cplane_netif, ci_ifid_t ifindex,
                     ci_ip_addr_net_t net_ip, ci_ip_addrset_t net_ipset);
  int (*route_import)(cicp_handle_t *cplane_netif,
                      cicp_route_rowid_t *out_rowid, ci_ip_addr_t dest_ip,
                      ci_ip_addrset_t dest_ipset, ci_ip_addr_t next_hop_ip,
                      ci_ip_tos_t tos, cicp_metric_t metric,
                      ci_ip_addr_t pref_source, ci_ifid_t ifindex);
  int (*route_delete)(cicp_handle_t *cplane_netif, 
                      ci_ip_addr_t dest_ip, ci_ip_addrset_t dest_ipset);
  int (*ping_open)(cicp_handle_t *control_plane, const ci_ip_addr_t *ref_ip);
  void (*ping_close)(cicp_handle_t *control_plane, ci_pinger_t ping_handle);
  int (*ping_request)(cicp_handle_t *control_plane, ci_pinger_t ping_handle,
                      ci_netif *ni, ci_uint16 data_len);
  int (*ping_poll)(cicp_handle_t *control_plane, ci_pinger_t ping_handle,
                   ci_uint8 *out_type, ci_uint8 *out_code,
                   ci_uint16 *out_seqno);
  int (*async_tcp_iscsi_tx)(ci_netif *ni, ci_tcp_state *ts, struct ci_mem_desc *desc_array,
                      int array_length, int total_entries, int *flags);
  void (*dhcp_handler)(int (*handler)(efhw_nic_t *nic, unsigned int port_id,
                                      ci_ether_hdr *hdr, unsigned char *data,
                                      int len));
  int (*mac_setup)(void);

  /** CHANGE EFAB_ISCSI_LINK_VERSION WHENEVER YOU CHANGE THIS STRUCT **/
} ci_il_fns_t;


#endif /* _EFAB_ISCSI_LINK_FNS_H */
