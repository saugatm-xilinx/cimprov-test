/**************************************************************************\
*//*! \file
** <L5_PRIVATE L5_HEADER >
** \author  cgg
**  \brief  Control Plane O/S Synchronization definitions
**   \date  2005/07/08
**    \cop  (c) Level 5 Networks Limited.
** </L5_PRIVATE>
*//*
\**************************************************************************/

/*! \cidoxg_include_ci_driver_efab_cplane_sync */

#ifndef __CI_DRIVER_EFAB_CPLANE_SYNC_H__
#define __CI_DRIVER_EFAB_CPLANE_SYNC_H__

/*! This file provides definitions are specific to given Operating System MIB
 *  synchronization scenario.  For example two versions of this header may be
 *  used to deal with Linux synchronization and with Windows synchronization
 *
 *  The prefix cicpos is used for definitions in this header:
 *       ci - our main prefix
 *       cp - control plane
 *       os - operating system synchronization
 */

/*----------------------------------------------------------------------------
 * Configuration
 *---------------------------------------------------------------------------*/

#define CICPOS_LLAP_NAMELEN_MAX 16



/*----------------------------------------------------------------------------
 * Windows-specific IOCTLs
 *---------------------------------------------------------------------------*/


/*! Function to handle IOCTRLs for control plane synchronization 
 *
 *  \param nic         not currently used
 *  \param priv        the ci_private_t being operated on
 *  \op                the operation requested
 *  \copy_out          flag telling caller if the results need copying
 *
 *  \return            an errno
 */
extern int
cicpos_sync_ctrl_op(cicpos_sync_op_t* op, int* copy_out);


#define CICPOS_USER_QLEN_LN2 6 /* ln2 of queue (to user) length */

typedef void *cicp_timer_opaque_t[10]; /* room for ten pointers */

typedef struct
{   void *user_event;   /* NULL iff no user */
    void *access_handle;/* NULL iff no user */
    ci_uint32 q_in;     /* next place circular buffer for user notification */
    ci_uint32 q_out;    /* next notification to give to user */
    cicpos_notification_t q[1<<CICPOS_USER_QLEN_LN2];
    cicp_timer_opaque_t timer; /* tick generator for synchronization */
} cicpos_user_t;


/*----------------------------------------------------------------------------
 * Address Resolution MIB
 *---------------------------------------------------------------------------*/

/* The details of these data structures are not public - please do not
   refer to their content directly - use the functions defined in
   <onload/cplane.h>
*/


typedef struct {
    ci_uint32 nl_msg_reject;     /*< # of rejected netlink neighbor msgs   */
    ci_uint32 poller_last_start; /*< last time poller was started          */
    ci_uint32 poller_last_end;   /*< last time poller ended                */
    ci_uint32 reinforcements;    /*< # of reinforcements                   */
} cicpos_mac_stat_t;


/*! Address Resolution table information used for synchonization that is
 *  O/S specific
 */
typedef struct
{   cicpos_user_t           user;
    int /*bool*/            timeout_unused;
} cicpos_mac_mib_t;


/*
 * Neighbor Cache Entry States
 * These values match those in the host os.
 * WARNING: if the host os values change then these need changing.
 * TODO: Introduce proper conversion functions for CICPOS_STATE_* constants.
 */
#define CICPOS_IPMAC_INCOMPLETE 0x01
#define CICPOS_IPMAC_REACHABLE  0x02
#define CICPOS_IPMAC_STALE      0x04
#define CICPOS_IPMAC_DELAY      0x08
#define CICPOS_IPMAC_PROBE      0x10
#define CICPOS_IPMAC_FAILED     0x20
#define CICPOS_IPMAC_NOARP      0x40
#define CICPOS_IPMAC_PERMANENT  0x80
#define CICPOS_IPMAC_NONE       0x00

#define CICPOS_IPMAC_CONNECTED \
       (CICPOS_IPMAC_NOARP | \
        CICPOS_IPMAC_PERMANENT | \
        CICPOS_IPMAC_REACHABLE)

#define CICPOS_IPMAC_VALID \
       (CICPOS_IPMAC_CONNECTED | \
        CICPOS_IPMAC_STALE | \
        CICPOS_IPMAC_DELAY | \
        CICPOS_IPMAC_PROBE)

struct cicpos_mac_row_sync_s
{   ci_uint8 just_confirmed; /* no windows information available */
}  /* cicpos_mac_row_sync_t */;

/*! Address Resolution entry information used for synchronization that is
    O/S-specific */
typedef struct
{   cicpos_mac_row_sync_t os;     /*< information provided by O/S sync */
    ci_subsec_time_t mapping_set; /*< time mapping last established */
    ci_subsec_time_t mapping_req; /*< time mapping (ARP) last requested */
    ci_verlock_value_t last_retrieval; /*< version when last retrieved */
    ci_verlock_value_t reinforce_request; /*< version to be confirmed */
    /* after each poll, if this flag is not set, the entry is deleted */
    unsigned    source_sync:1,    /*< os field only valid if this is 1 */
	        source_prot:1,    /*< entry has been set by protocol   */
		recent_sync:1,    /*< entry has been seen recently */
	        path_confirmed:1; /* needed? */
} cicpos_mac_row_t;



/*! Indicate that this entry has just been synchronized with the O/S
 */
extern void cicpos_mac_row_synced(cicpos_mac_row_t *row);



/*! Check whether this row has been synced since this function was last
 *  called
 */
extern int /* bool */ cicpos_mac_row_recent(cicpos_mac_row_t *sync);
    

/*! Set policy on unused IP-MAC entry removal
 *
 * \param control_plane    control plane handle (use CICP_HANDLE(netif))
 * \param prune_regularly  whether to remove MAC entries in the control plane
 *
 * With a normal operating system synchronization policy IP-MAC entries will
 * be deleted shortly after the operating system has deleted them in its
 * own table.  In this case the control plane does not need to delete entries
 * itself - in fact, it may be wrong to do so.
 *
 * If there is no operating system synchronization being performed (e.g.
 * because we are using a separate control plane instance or because the
 * Windows user-mode process responsible for performing this syncrhonization
 * has not been activated) we may wish to perform IP-MAC entry timeout
 * ourselves.
 *
 * This function allows the (kernel) user to determine whether to perform
 * regular timeout of unused IP-MAC entries.
 *
 * NB: (currently) this is a windows-specific function
 */
extern void
cicpos_mac_manage_unused(cicp_handle_t *control_plane,
			 int /*bool*/ prune_regularly);




/*----------------------------------------------------------------------------
 * kernel routing MIB
 *---------------------------------------------------------------------------*/

/* The details of these data structures are not public - please do not
   refer to their content directly - use the functions defined in
   <onload/cplane.h>
*/

/*! Synchronization-support module-specific per-entry information */
typedef struct
{   /* after each poll, if this flag is not set, the entry is deleted */
    ci_uint8 confirmed_on_last_poll;
} cicpos_route_row_t;



/*----------------------------------------------------------------------------
 * kernel access point MIB
 *---------------------------------------------------------------------------*/

/* The details of these data structures are not public - please do not
   refer to their content directly - use the functions defined in
   <onload/cplane.h>
*/

typedef struct
{   char name[CICPOS_LLAP_NAMELEN_MAX+1]; /*< interface name e.g. eth0 */
    /*! after each poll, if this flag is not set, the entry is deleted */
    ci_uint8 confirmed_on_last_poll;
} cicpos_llap_row_t;


/*----------------------------------------------------------------------------
 * overall control plane
 *---------------------------------------------------------------------------*/


extern int cicpos_running;


#endif /* __CI_DRIVER_EFAB_CPLANE_SYNC_H__ */
/*! \cidoxg_end */
