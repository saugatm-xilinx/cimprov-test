/**************************************************************************\
*//*! \file
** <L5_PRIVATE L5_HEADER >
** \author  djr
**  \brief  Char driver operations API.
**   \date  2010/09/01
**    \cop  (c) Solarflare Communications, Inc.
** </L5_PRIVATE>
*//*
\**************************************************************************/

/*! \cidoxg_include_ci_efrm  */

#ifndef __CI_EFCH_OP_TYPES_H__
#define __CI_EFCH_OP_TYPES_H__


struct efch_vi_alloc_in {
  ci_int32            ifindex;
  ci_int32            vi_set_fd;      /* -1 if not allocating from set */
  efch_resource_id_t  vi_set_rs_id;
  ci_int32            vi_set_instance;
  ci_int32            evq_fd;
  efch_resource_id_t  evq_rs_id;
  ci_int32            evq_capacity;
  ci_int32            txq_capacity;
  ci_int32            rxq_capacity;
  ci_uint32           flags;  /* EFAB_VI_* flags */
  ci_uint8            tx_q_tag;
  ci_uint8            rx_q_tag;
};


struct efch_vi_alloc_out {
  ci_int32            evq_capacity;
  ci_int32            txq_capacity;
  ci_int32            rxq_capacity;
  ci_uint8            nic_arch;
  ci_uint8            nic_variant;
  ci_uint8            nic_revision;
  ci_uint32           mem_mmap_bytes;
  ci_uint32           io_mmap_bytes;
  ci_int32            instance;
};


struct efch_vi_set_alloc {
  ci_int32            in_ifindex;
  ci_int32            in_min_n_vis;
  ci_uint32           in_flags;
};


struct efch_iobufset_alloc {
  ci_int32            in_linked_fd;
  efch_resource_id_t  in_linked_rs_id;
  ci_int32            in_n_pages;
  ci_int32            in_vi_fd;
  efch_resource_id_t  in_vi_rs_id;
  ci_boolean_t        in_phys_addr_mode;
  efhw_buffer_addr_t  out_bufaddr;
  ci_uint32           out_mmap_bytes;
};


typedef struct ci_resource_alloc_s {

  ci_uint32 ra_type;

  efch_resource_id_t out_id;

  union {
    struct efch_iobufset_alloc iobufset;
    struct efch_vi_alloc_in    vi_in;
    struct efch_vi_alloc_out   vi_out;
    struct efch_vi_set_alloc   vi_set;
  } u;
} ci_resource_alloc_t;


typedef struct ci_resource_op_s {
  efch_resource_id_t    id;
  ci_uint32             op;
# define                CI_RSOP_VI_GET_MAC              0x49
# define                CI_RSOP_EVENTQ_PUT              0x51
# define                CI_RSOP_EVENTQ_WAIT             0x54
# define                CI_RSOP_VI_GET_MTU              0x55
# define                CI_RSOP_DUMP                    0x58
# define                CI_RSOP_EVQ_REGISTER_POLL       0x59
# define                CI_RSOP_PT_ENDPOINT_FLUSH       0x5a
# define                CI_RSOP_PT_ENDPOINT_PACE        0x62
# define                CI_RSOP_FILTER_ADD_IP4          0x63
# define                CI_RSOP_FILTER_ADD_MAC          0x64
# define                CI_RSOP_FILTER_ADD_ALL_UNICAST  0x65
# define                CI_RSOP_FILTER_ADD_ALL_MULTICAST 0x66
# define                CI_RSOP_FILTER_DEL              0x67

  union {
    struct {
      ci_uint32         current_ptr;
      ci_timeval_t      timeout;
      ci_uint32         nic_index;
    } evq_wait;
    struct {
      efhw_event_t      ev;     /* 32 + 32 bits */
    } evq_put;
    struct {
      ci_uint16         out_mtu;
    } vi_get_mtu;
    struct {
      ci_uint8          out_mac[6];
    } vi_get_mac;
    struct {
      ci_int32          pace;
    } pt;
    struct {
      struct {
        ci_uint8        protocol;
        ci_int16        port_be16;
        ci_int16        rport_be16;
        ci_uint32       host_be32;
        ci_uint32       rhost_be32;
      } ip4;
      struct {
        ci_int16        vlan_id;
        ci_uint8        mac[6];
      } mac;
      int               replace;
      ci_int32          out_filter_id;
    } filter_add;
    struct {
      ci_int32          filter_id;
    } filter_del;
  } u CI_ALIGN(8);
} ci_resource_op_t;


#endif  /* __CI_EFCH_OP_TYPES_H__ */
