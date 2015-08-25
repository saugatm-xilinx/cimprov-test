/**************************************************************************/
/*!  \file  sfu_device.h
** \author  bwh
**  \brief  Device enumeration for sfutils
**   \date  2010/05/21
**    \cop  Copyright 2010 Solarflare Communications Inc.
*//************************************************************************/

#ifndef SFUTILS_SFU_DEVICE_H
#define SFUTILS_SFU_DEVICE_H

#include <stdint.h>
#include <assert.h>

#if defined(__linux__) && !defined(SFUTILS_USE_UIO)
#include <net/if.h>
#include <sys/socket.h>
#include "efx_linux_types.h"
#include <linux/ethtool.h>

#define NETIF_NAME_SIZE	IFNAMSIZ
#define PCI_ADDR_SIZE	ETHTOOL_BUSINFO_LEN
#endif

#if defined(__sun) && defined (__SVR4)
#include <net/if.h>
#include <sys/socket.h>

#define NETIF_NAME_SIZE	IFNAMSIZ
#define PCI_ADDR_SIZE	1 /* small as it's not used in solaris sfudpate */
#endif

#if defined(__FreeBSD__)
#include <net/if.h>
#include <sys/socket.h>

#define NETIF_NAME_SIZE	IFNAMSIZ
#define PCI_ADDR_SIZE	(4 + 1 + 3 + 1 + 3 + 1 + 3 + 1) /* format: dom:bus:dev:fn */
#endif

#ifdef SFUTILS_USE_UIO
#define NETIF_NAME_SIZE	(6 + 1)	/* just an index */
#define PCI_ADDR_SIZE	(7 + 1)	/* format: bbbb:dd.f */
#endif

#if defined(__APPLE__) && defined (__MACH__)
#define NETIF_NAME_SIZE	(4 + 1)  /* format "enNN", N=0..99 */
#define PCI_ADDR_SIZE	(14 + 1) /* format: "bb:dd.f(s:s)" */
#endif

#include "utils.h"
#include "nv.h"

struct dynamic_cfg_context;

/* Note that the serial number size is for the Solarflare V6+ format
** (NOT the OEM serial number) and includes space for a trailing NUL
** for convenience.
*/
#define SFU_SF_SERIALNUMBER_SIZE (24+1)

struct sfu_sll {
  struct sfu_sll* head;
  struct sfu_sll* next;
};

#define SFU_SLL_TO_DEVICE(sll) \
  ((struct sfu_device *)((unsigned char*)(sll) - offsetof(struct sfu_device, ports)))
#define SFU_DEVICE_TO_SLL(dev) (&(dev)->ports)
#define SFU_SLL_IS_END(item) ((item) == NULL)
#define SFU_SLL_GET_HEAD(list) ((list)->head)

#define SFU_PORTS_NOT_INIT(dev) ((dev)->ports.head == NULL)
#define SFU_INIT_PORTS(dev) {        \
  assert(SFU_PORTS_NOT_INIT(dev));   \
  (dev)->ports.head = &(dev)->ports; \
  (dev)->ports.next = NULL;          \
  (dev)->ports_count = 1;            \
}

extern void sfu_add_to_ports(struct sfu_device* dev, struct sfu_device* other);

#define SFU_IS_GROUP_MASTER(dev) ((dev) == (dev)->master_port)
#define SFU_GET_MASTER(dev) ((dev)->master_port)
#define SFU_GET_DEVICE_LIST(dev) SFU_SLL_GET_HEAD(&SFU_GET_MASTER(dev)->ports)
#define SFU_DEV_MAX_N_PORTS 4

struct sfu_device {
  char netif_name[NETIF_NAME_SIZE];
  char mac_addr[MAC_ADDR_STR_SIZE];
  char serial_number[SFU_SF_SERIALNUMBER_SIZE];
 
  uint32_t pci_device_id;
  /* Used for OEM specific operations. Currently only filled on Linux. */
  uint32_t pci_subsystem_id;
  char pci_addr[PCI_ADDR_SIZE];
  unsigned port_index;
  int phy_type;
  /* Once initialised all ports have master_port != NULL.  _The_ master port
  ** will have master_port set to itself (and this is how it is identified).
  ** The master port maintains the port list. */
  struct sfu_device* master_port;
  /* Set of all ports in the group, including the master port.
  ** ONLY the master port maintains this list. Can be accessed from any group
  ** member like this: dev->master_port->ports */
  struct sfu_sll ports;
  int ports_count;
  /* The dynamic context that this device has access to */
  struct dynamic_cfg_context *dyn_context;
  void* priv;				/* platform private pointer */
};

/* Get the best device for NV access. Will use the provided device if
** possible but will otherwise return the first device that can open
** and read the NV requested (note that devices are opened, but test
** data reads are not performed). */
extern const struct sfu_device*
sfu_get_nv_device(const struct sfu_device* dev, enum nv_part_type type,
                  unsigned subtype);

/* Initialise device enumeration; returns value < 0 on failure */
extern int sfu_device_init(void);

/* Enumerate our devices.  Return number of devices found or -1 in
 * case of error.  Set *pdevs to point to an array of sfu_device
 * structures. */
extern int sfu_device_enum(struct sfu_device** pdevs);

#define SFU_DEVICE_ID_FALCON_A   0x19240703
#define SFU_DEVICE_ID_FALCON_B   0x19240710
#define SFU_DEVICE_ID_BETHPAGE   0x19240803
#define SFU_DEVICE_ID_SIENA      0x19240813
#define SFU_DEVICE_ID_HUNTINGTON 0x19240903

enum sfu_device_types {
  SFU_DEVICE_TYPE_FALCON     = 0x0700,
  SFU_DEVICE_TYPE_SIENA      = 0x0800,
  SFU_DEVICE_TYPE_HUNTINGTON = 0x0900
};

#define SFU_DEVICE_TYPE(pci_id) ((pci_id) & 0x0F00)

static inline bool is_ef10_device(const struct sfu_device *dev)
{
  return SFU_DEVICE_TYPE(dev->pci_device_id) == SFU_DEVICE_TYPE_HUNTINGTON;
}

/* Printing helpers. */
#define SFU_DEV_NAME_FMT      "%s (%s)"
#define SFU_DEV_NAME_ARG(dev) (dev)->netif_name, (dev)->mac_addr

/* Get the PCI addr as an int. Returns -1 on error or if not
** fully supported. */
extern uint32_t sfu_parse_pci_addr(const struct sfu_device* dev);
#define SFU_NO_PCI_ADDR (0xffffffff)
#define SFU_PCI_ADDR_MASK_NO_PF (0xffffff00)

static inline uint32_t parse_pci_addr_no_pf(const struct sfu_device* dev) {
  uint32_t rc = sfu_parse_pci_addr(dev);
  if(rc != SFU_NO_PCI_ADDR)
    rc &= SFU_PCI_ADDR_MASK_NO_PF;
  return rc;
}

/* Verify that the given NUL-terminated string is a valid Solarflare
** serial number in both length and content. */
extern bool sfu_is_solarflare_serial(const char* serial_number);

/* Group the devices by their serial numbers */
extern void sfu_group_devices(struct sfu_device ** pdevs, int ndevs);

/* Opens NV using the first port in the group that has successful NV access.
** Will return NULL if no port can open NV storage - this may be because
** the current user does not have appropriate permission. */
extern struct nv_context* sfu_nv_open(struct sfu_device* dev, int part, int sub);

/* Is the device possibly a VF? Will always return false until the device
** struct has been fully initialised/grouped. The possibility is determined
** by the serial number (which, on Linux at least, is not available for VFs).*/
extern bool sfu_maybe_vf(const struct sfu_device* dev);
#endif /* !SFUTILS_SFU_DEVICE_H */
