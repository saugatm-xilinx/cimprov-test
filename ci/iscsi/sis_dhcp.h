/**************************************************************************\
*//*! \file dhcp.h   Definitions for DHCP
** <L5_PRIVATE L5_SOURCE>
** \author  mjs
**  \brief  Package - lib/iscsi       iSCSI library
**   \date  2006/09/28
**    \cop  (c) Level 5 Networks Limited.
** </L5_PRIVATE>
*//*
\**************************************************************************/

/*! \cidoxg_include_ci_iscsi */

#ifndef __CI_ISCSI_DHCP_H__
#define __CI_ISCSI_DHCP_H__


#include <ci/net/ethernet.h>

/* ============================================================================
 *  DHCP protocol formats and constants
 * ============================================================================
 */

/* Minimum size of DHCP options (RFC2131 section 2)
 */
#define DHCP_OPTIONS_MIN_SIZE 312

/* Format of a DHCP message (RFC2131 section 2).  Note that all fields are
 * aligned to a boundary appropriate to their size, so we can be hopeful that
 * the compiler will not insert any padding, except possibly 32 bits at the end
 * on a 64-bit system.  This can be checked: the options_cookie field should be
 * at byte offset 232 from the start.)
 */
typedef struct ci_dhcp_msg_s {
  ci_uint8  op;              /* Message opcode                           */
#define DHCP_OP_BOOTREQUEST 1
#define DHCP_OP_BOOTREPLY   2
  ci_uint8  htype;           /* Hardware address type                    */
#define DHCP_HTYPE_ETHERNET 1
  ci_uint8  hlen;            /* Hardware address length                  */
#define DHCP_HLEN_ETHERNET 6
  ci_uint8  hops;            /* Hop count, used by relay agents          */
  ci_uint32 xid;             /* Transaction ID                           */
  ci_uint16 secs;            /* Seconds since beginning acquire or renew */
  ci_uint16 flags;           /* Flags                                    */
#define DHCP_FLAGS_BROADCAST 0x8000
  ci_uint32 ciaddr;          /* Client IP address                        */
  ci_uint32 yiaddr;          /* "Your" IP address                        */
  ci_uint32 siaddr;          /* Server IP address                        */
  ci_uint32 giaddr;          /* Relay agent IP address                   */
  ci_uint8  chaddr[16];      /* Client hardware address                  */
  ci_uint8  sname[64];       /* Server host name                         */
  ci_uint8  file[128];       /* Boot file name                           */
  ci_uint32 options_cookie;  /* Cookie indicating valid DHCP options     */
#define DHCP_OPTIONS_COOKIE 0x63825363  /* Options cookie (host order)   */
  /* options follow... */
  ci_uint8  options[DHCP_OPTIONS_MIN_SIZE - 4];  /* -4 for above cookie  */
} ci_dhcp_msg_t;


/* DHCP option codes */

#define DHCP_OPTION_PAD_OPTION                    0  /* RFC2132 3.1  */
#define DHCP_OPTION_END_OPTION                  255  /* RFC2132 3.2  */
#define DHCP_OPTION_SUBNET_MASK                   1  /* RFC2132 3.3  */
#define DHCP_OPTION_TIME_OFFSET                   2  /* RFC2132 3.4  */
#define DHCP_OPTION_ROUTER                        3  /* RFC2132 3.5  */
#define DHCP_OPTION_TIME_SERVER                   4  /* RFC2132 3.6  */
#define DHCP_OPTION_NAME_SERVER                   5  /* RFC2132 3.7  */
#define DHCP_OPTION_DNS_SERVER                    6  /* RFC2132 3.8  */
#define DHCP_OPTION_LOG_SERVER                    7  /* RFC2132 3.9  */
#define DHCP_OPTION_COOKIE_SERVER                 8  /* RFC2132 3.10 */
#define DHCP_OPTION_LPR_SERVER                    9  /* RFC2132 3.11 */
#define DHCP_OPTION_IMPRESS_SERVER               10  /* RFC2132 3.12 */
#define DHCP_OPTION_RESOURCE_LOCATION_SERVER     11  /* RFC2132 3.13 */
#define DHCP_OPTION_HOST_NAME                    12  /* RFC2132 3.14 */
#define DHCP_OPTION_BOOT_FILE_SIZE               13  /* RFC2132 3.15 */
#define DHCP_OPTION_MERIT_DUMP_FILE              14  /* RFC2132 3.16 */
#define DHCP_OPTION_DOMAIN_NAME                  15  /* RFC2132 3.17 */
#define DHCP_OPTION_SWAP_SERVER                  16  /* RFC2132 3.18 */
#define DHCP_OPTION_ROOT_PATH                    17  /* RFC2132 3.19 */
#define DHCP_OPTION_EXTENSIONS_PATH              18  /* RFC2132 3.20 */
#define DHCP_OPTION_IP_FORWARDING_EN             19  /* RFC2132 4.1  */
#define DHCP_OPTION_NONLOCAL_SOURCE_ROUTING_EN   20  /* RFC2132 4.2  */
#define DHCP_OPTION_POLICY_FILTER                21  /* RFC2132 4.3  */
#define DHCP_OPTION_MAX_DATAGRAM_REASSEMBLY_SIZE 22  /* RFC2132 4.4  */
#define DHCP_OPTION_DEFAULT_IP_TTL               23  /* RFC2132 4.5  */
#define DHCP_OPTION_PATH_MTU_AGING_TIMEOUT       24  /* RFC2132 4.6  */
#define DHCP_OPTION_PATH_MTU_PLATEAU_TABLE       25  /* RFC2132 4.7  */
#define DHCP_OPTION_INTERFACE_MTU                26  /* RFC2132 5.1  */
#define DHCP_OPTION_ALL_SUBNETS_ARE_LOCAL        27  /* RFC2132 5.2  */
#define DHCP_OPTION_BROADCAST_ADDRESS            28  /* RFC2132 5.3  */
#define DHCP_OPTION_PERFORM_MASK_DISCOVERY       29  /* RFC2132 5.4  */
#define DHCP_OPTION_MASK_SUPPLIER                30  /* RFC2132 5.5  */
#define DHCP_OPTION_PERFORM_ROUTER_DISCOVERY     31  /* RFC2132 5.6  */
#define DHCP_OPTION_ROUTER_SOLICITATION_ADDRESS  32  /* RFC2132 5.7  */
#define DHCP_OPTION_STATIC_ROUTE                 33  /* RFC2132 5.8  */
#define DHCP_OPTION_TRAILER_ENCAPSULATION        34  /* RFC2132 6.1  */
#define DHCP_OPTION_ARP_CACHE_TIMEOUT            35  /* RFC2132 6.2  */
#define DHCP_OPTION_ETHERNET_ENCAPSULATION       36  /* RFC2132 6.3  */
#define DHCP_OPTION_TCP_DEFAULT_TTL              37  /* RFC2132 7.1  */
#define DHCP_OPTION_TCP_KEEPALIVE_INTERVAL       38  /* RFC2132 7.2  */
#define DHCP_OPTION_TCP_KEEPALIVE_GARBAGE        39  /* RFC2132 7.3  */
#define DHCP_OPTION_NIS_DOMAIN                   40  /* RFC2132 8.1  */
#define DHCP_OPTION_NIS_SERVERS                  41  /* RFC2132 8.2  */
#define DHCP_OPTION_NTP_SERVERS                  42  /* RFC2132 8.3  */
#define DHCP_OPTION_VENDOR_SPECIFIC_INFO         43  /* RFC2132 8.4  */
#define DHCP_OPTION_NBT_NAME_SERVER              44  /* RFC2132 8.5  */
#define DHCP_OPTION_NBT_DATAGRAM_DISTRIB_SERVER  45  /* RFC2132 8.6  */
#define DHCP_OPTION_NBT_NODE_TYPE                46  /* RFC2132 8.7  */
#define DHCP_OPTION_NBT_SCOPE                    47  /* RFC2132 8.8  */
#define DHCP_OPTION_X_FONT_SERVER                48  /* RFC2132 8.9  */
#define DHCP_OPTION_X_DISPLAY_MANAGER            49  /* RFC2132 8.10 */
#define DHCP_OPTION_NISPLUS_DOMAIN               64  /* RFC2132 8.11 */
#define DHCP_OPTION_NISPLUS_SERVERS              65  /* RFC2132 8.12 */
#define DHCP_OPTION_MOBILE_IP_HOME_AGENT         68  /* RFC2132 8.13 */
#define DHCP_OPTION_SMTP_SERVER                  69  /* RFC2132 8.14 */
#define DHCP_OPTION_POP3_SERVER                  70  /* RFC2132 8.15 */
#define DHCP_OPTION_NNTP_SERVER                  71  /* RFC2132 8.16 */
#define DHCP_OPTION_DEFAULT_WWW_SERVER           72  /* RFC2132 8.17 */
#define DHCP_OPTION_DEFAULT_FINGER_SERVER        73  /* RFC2132 8.18 */
#define DHCP_OPTION_DEFAULT_IRC_SERVER           74  /* RFC2132 8.19 */
#define DHCP_OPTION_STREETTALK_SERVER            75  /* RFC2132 8.20 */
#define DHCP_OPTION_STDA_SERVER                  76  /* RFC2132 8.21 */
#define DHCP_OPTION_REQUESTED_IP_ADDRESS         50  /* RFC2132 9.1  */
#define DHCP_OPTION_IP_ADDRESS_LEASE_TIME        51  /* RFC2132 9.2  */
#define DHCP_OPTION_OPTION_OVERLOAD              52  /* RFC2132 9.3  */
#define DHCP_OPTION_TFTP_SERVER_NAME             66  /* RFC2132 9.4  */
#define DHCP_OPTION_BOOTFILE_NAME                67  /* RFC2132 9.5  */
#define DHCP_OPTION_DHCP_MESSAGE_TYPE            53  /* RFC2132 9.6  */
#define DHCP_OPTION_SERVER_ID                    54  /* RFC2132 9.7  */
#define DHCP_OPTION_PARAM_REQUEST_LIST           55  /* RFC2132 9.8  */
#define DHCP_OPTION_MESSAGE                      56  /* RFC2132 9.9  */
#define DHCP_OPTION_MAX_DHCP_MESSAGE_SIZE        57  /* RFC2132 9.10 */
#define DHCP_OPTION_RENEWAL_TIME_VALUE           58  /* RFC2132 9.11 */
#define DHCP_OPTION_REBINDING_TIME_VALUE         59  /* RFC2132 9.12 */
#define DHCP_OPTION_VENDOR_CLASS_ID              60  /* RFC2132 9.13 */
#define DHCP_OPTION_CLIENT_ID                    61  /* RFC2132 9.14 */


/* Values for the Option Overload option (RFC2132 9.3) */

#define DHCP_OVERLOAD_FILE_FIELD   1  /* 'file' field used for options  */
#define DHCP_OVERLOAD_SNAME_FIELD  2  /* 'sname' field used for options */
#define DHCP_OVERLOAD_BOTH_FIELDS  3  /* both fields used for options   */


/* Values for the DHCP Message Type option (RFC2132 9.6) */

#define DHCP_MSG_DHCPDISCOVER  1
#define DHCP_MSG_DHCPOFFER     2
#define DHCP_MSG_DHCPREQUEST   3
#define DHCP_MSG_DHCPDECLINE   4
#define DHCP_MSG_DHCPACK       5
#define DHCP_MSG_DHCPNAK       6
#define DHCP_MSG_DHCPRELEASE   7
#define DHCP_MSG_DHCPINFORM    8


/* UDP ports (RFC2131 4.1) */

#define DHCP_UDP_PORT_SERVER  67  /* Listening port on server */
#define DHCP_UDP_PORT_CLIENT  68  /* Listening port on client */


/* ============================================================================
 *  Types for our DHCP implementation
 * ============================================================================
 */

/* States for a DHCP client (derived from RFC2131 4.4) */

typedef enum ci_dhcp_fsmstate_e {
  CI_DHCP_FSMSTATE_INIT_REBOOT,
  CI_DHCP_FSMSTATE_REBOOTING,
  CI_DHCP_FSMSTATE_INIT,
  CI_DHCP_FSMSTATE_SELECTING,
  CI_DHCP_FSMSTATE_REQUESTING,
  CI_DHCP_FSMSTATE_BOUND,
  CI_DHCP_FSMSTATE_RENEWING,
  CI_DHCP_FSMSTATE_REBINDING
} ci_dhcp_fsmstate_t;


/* State machine events (derived from RFC2131 4.4) */

typedef enum ci_dhcp_fsmevent_e {
  CI_DHCP_FSMEVENT_NONE,
  CI_DHCP_FSMEVENT_START,
  CI_DHCP_FSMEVENT_UNACCEPTABLE_OFFER,
  CI_DHCP_FSMEVENT_ACCEPTABLE_OFFER,
  CI_DHCP_FSMEVENT_UNACCEPTABLE_ACK,
  CI_DHCP_FSMEVENT_ACCEPTABLE_ACK,
  CI_DHCP_FSMEVENT_NAK,
  CI_DHCP_FSMEVENT_T1_EXPIRED,
  CI_DHCP_FSMEVENT_T2_EXPIRED,
  CI_DHCP_FSMEVENT_LEASE_EXPIRED,
  /* (the following are not from the RFC) */
  CI_DHCP_FSMEVENT_SHUTDOWN,
  CI_DHCP_FSMEVENT_RETRANSMIT
} ci_dhcp_fsmevent_t;


/* Collected interface information */

typedef struct ci_dhcp_info_s {
  unsigned int       hwport;
  ci_uint8           mac[ETH_ALEN];
  ci_uint32          ip_address;
  ci_uint32          subnet_mask;
  ci_uint32          bcast_address;
  ci_uint32          gateway;
  ci_uint32          server;
  ci_uint8           server_mac[ETH_ALEN];
  void *             context;
} ci_dhcp_info_t;


/* Types for interface up/down callbacks */

typedef void (*ci_dhcp_up_cb_t)(ci_dhcp_info_t *info);

typedef void (*ci_dhcp_down_cb_t)(ci_dhcp_info_t *info);


/* Buffer for received packets from net driver callback */

#define CI_DHCP_RXBUF_PKTS  4       /* should be a power of 2 */
#define CI_DHCP_RXBUF_MAX_DATA 576  /* minimum IP MTU */

typedef struct ci_dhcp_rxbuf_s {
  struct {
    ci_ether_hdr hdr;
    ci_uint8 data[CI_DHCP_RXBUF_MAX_DATA];
    unsigned int len;
  } pkts[CI_DHCP_RXBUF_PKTS];
  unsigned int wptr;
  unsigned int rptr;
} ci_dhcp_rxbuf_t;


/* State of DHCP */

typedef struct ci_dhcp_state_s {
  ci_boolean_t       enabled;      /* is this DHCP state enabled? */
  ci_boolean_t       updated;      /* enable state has just been changed */
  ci_dhcp_fsmstate_t fsmstate;     /* current state machine state */
  ci_uint32          now;          /* last tick time (in seconds) */
  ci_uint32          t1_expiry;    /* T1 expiry time (in seconds) */
  ci_uint32          t2_expiry;    /* T2 expiry time (in seconds) */
  ci_uint32          lease_expiry; /* lease expiry time (in seconds) */
  ci_uint32          retx_time;    /* retransmit time (in seconds) */
  ci_dhcp_info_t     info;         /* structure for reported information */
  ci_dhcp_up_cb_t    up_cb;        /* callback when interface comes up */
  ci_dhcp_down_cb_t  down_cb;      /* callback when interface goes down */
  ci_uint32          current_xid;  /* xid for current transaction */
  ci_uint32          tx_count;     /* transmit counter */
  ci_uint8           recv_type;    /* DHCP type of received message */
  unsigned int       option_len;   /* current length of options in bytes */
  ci_uint8           overload;     /* option overload (receive only) */
  tips_stack        *netif;        /* netif */
  ci_uint8           last_rx_mac[ETH_ALEN]; /* Source of most recent rx */
  ci_dhcp_msg_t      msg;          /* current message received or to send */
  ci_dhcp_rxbuf_t    rxbuf;        /* rx buffer for packets from net driver */
} ci_dhcp_state_t;



/* ============================================================================
 *  Exported interface
 * ============================================================================
 */

extern int ci_dhcp_ctor(void);

extern int ci_dhcp_start(unsigned int hwport, ci_uint8 *mac, void *context,
                         ci_dhcp_up_cb_t up_cb, ci_dhcp_down_cb_t down_cb);

extern int ci_dhcp_stop(unsigned int hwport);

extern int ci_dhcp_tick(ci_uint32 now);

extern int ci_dhcp_dtor(void);

#ifndef NDEBUG
extern void ci_dhcp_dump(unsigned int hwport);
#endif


#endif /* __CI_ISCSI_DHCP_H__ */

/*! \cidoxg_end */

