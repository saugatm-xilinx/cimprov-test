/**************************************************************************\
*//*! \file driverlink_win_api.h
** <L5_PRIVATE L5_HEADER >
** \author  mch
**  \brief  Common header for new "direct function call" driverlink API.
**     $Id$
**   \date  2005/11
**    \cop  (c) Level 5 Networks Limited.
** </L5_PRIVATE>
*//*
\**************************************************************************/

#ifndef EFAB_DRIVERLINK_WIN_API_H
#define EFAB_DRIVERLINK_WIN_API_H

#include <ci/compat.h>
#include <ci/tools/log.h>
#include <ci/tools/debug.h>
#include <ci/tools/dllist.h>


//For comments on datastructures in this file, see header:
//<src/driver/linux_net/driverlink_api.h>

//This version of the API has had as much type information removed from it as possible.
//In particular "device" pointers are opaque, which allows for various stub layers with the
//same function prototypes.

/* Forward declarations */

#define EFAB_DRIVERLINK_API_VERSION 0101

typedef unsigned int DRIVERLINK_NIC_ID;

/** An Etherfabric device type */
enum efab_type_index {
	/* Must start with a non-zero value; zero is used as a list
	 * terminator in an array of struct efab_device_ids.
	 */
	/** A Falcon NIC */
	EFAB_TYPE_FALCON = 1,
};

enum efab_veto {
	/** Packet may be transmitted/received */
	EFAB_ALLOW_PACKET = 0,
	/** Packet may not be transmitted/received */
	EFAB_VETO_PACKET = 1,
};

//IDC handle is a generic handle which can be translated to either a wdm or an ndis NIC
//in the appropriate driver (see wdm_idc.h and ndis_idc.h)
typedef void * idc_handle;

struct efab_dl_callbacks {
	/** Packet about to be transmitted */
	enum efab_veto  (__fastcall * tx_packet ) ( idc_handle efab_dev, ULONG port_index, void *buf_kva, unsigned buf_len, unsigned buf_total_len );
	/** Packet received */
	enum efab_veto (__fastcall * rx_packet ) ( idc_handle efab_dev, ULONG port_index, void *buf_kva, unsigned buf_len, unsigned buf_total_len );
	/** Link status change */
	void ( * link_change ) (idc_handle efab_dev, ULONG port_index, int link_up );
	/** Request MTU change */
	int ( * request_mtu ) ( idc_handle efab_dev, ULONG port_index, int new_mtu );
	/** MTU has been changed */
	void ( * mtu_changed ) ( idc_handle efab_dev, ULONG port_index);
};

//Function for windows driverlink to query an interface about which NIC it
//is related to.

//TODO - May also need to get more referencing information about
//device/port/pci dev structure, but at the moment, this has been omitted.
extern int efab_dl_get_id_and_nic(int module_instance, 
                                  DRIVERLINK_NIC_ID *id,
                                  struct efhw_nic_s **nic);


/** Register callbacks for an Etherfabric NIC */
extern int efab_dl_register_callbacks ( idc_handle efab_dev,
					struct efab_dl_callbacks *callbacks );

/** Unregister callbacks for an Etherfabric NIC */
extern void efab_dl_unregister_callbacks ( idc_handle efab_dev );

/** Reset Etherfabric NIC */
extern int efab_dl_reset ( idc_handle efab_dev );

/** Schedule reset of Etherfabric NIC */
extern void efab_dl_schedule_reset ( idc_handle efab_dev );

/** Suspend net driver operations */
extern int efab_dl_suspend_net_driver ( idc_handle efab_dev );

/** Resume net driver operations */
extern void efab_dl_resume_net_driver ( idc_handle efab_dev );

/* TODO Reset suspend and reset resume callbacks not required *yet*, because *at the moment*
   only one client (WDM) will be attached to each server (NDIS).
   Suggest these can be added if/when we have multiple clients for each server.

   */

/** Device type naming function
 *
 * @v type		Etherfabric device type
 * @ret name		Device type name
 *
 * This function provides a textual representation of the specified
 * type for use in log messages.
 */
static __inline const char * efab_type_name ( enum efab_type_index type ) {
	switch ( type ) {
	case EFAB_TYPE_FALCON:
		return "Falcon";
	default:
		return "UNKNOWN";
	}
}

/* Some methods of accessing API functions may need to get hold of them
   via a table (esp on windows) */

//TODO - Need to consider reset suspend and reset resume callbacks
//TODO - This we'll end up doing stuff with ci_ndis_port_t :-(

typedef int (* efab_dl_get_id_and_nic_t)(int module_instance,  DRIVERLINK_NIC_ID *id, struct efhw_nic_s **nic);

typedef int (*efab_dl_register_callbacks_t) ( idc_handle efab_dev,
					struct efab_dl_callbacks *callbacks );
typedef void (*efab_dl_unregister_callbacks_t) ( idc_handle efab_dev );
typedef int (*efab_dl_reset_t) ( idc_handle efab_dev );
typedef void (*efab_dl_schedule_reset_t) ( idc_handle efab_dev );
typedef int (*efab_dl_suspend_net_driver_t) ( idc_handle efab_dev );
typedef void (*efab_dl_resume_net_driver_t) ( idc_handle efab_dev );

typedef struct efab_dl_function_table_s {
  int table_version;                  //Expect to be EFAB_DRIVERLINK_API_VERSION
  efab_dl_get_id_and_nic_t            get_id_and_nic_fn;
  efab_dl_register_callbacks_t        register_callbacks_fn;
  efab_dl_unregister_callbacks_t      unregister_callbacks_fn;
  efab_dl_reset_t                     reset_fn;
  efab_dl_schedule_reset_t            schedule_reset_fn;
  efab_dl_suspend_net_driver_t        suspend_net_driver_fn;
  efab_dl_resume_net_driver_t         resume_net_driver_fn;
} efab_dl_function_table_t, *pefab_dl_function_table_t;

#endif /* EFAB_DRIVERLINK_API_H */

/*
 * Local variables:
 *  c-basic-offset: 8
 *  c-indent-level: 8
 *  tab-width: 8
 * End:
 */
