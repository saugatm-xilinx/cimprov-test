/**************************************************************************/
/*!  \file  sf_native_device.h
** \author  smitra
**  \brief  Device enumeration for sfutils
**   \date  2018/10/22
**    \cop  Copyright 2018 Solarflare Communications Inc.
*//************************************************************************/

#ifndef SF_NATIVE_DEVICE_H
#define SF_NATIVE_DEVICE_H

#include "sf_pci_ids.h"

namespace solarflare {
#define PCI_ADDR_SIZE 16    /// Size equivalent to SFVMK_PCI_BDF_LEN in driver
#define MAC_ADDR_STR_SIZE 18


/* Note that the serial number size is for the Solarflare V6+ format
** (NOT the OEM serial number) and includes space for a trailing NUL
** for convenience.
*/
#define SFU_SF_SERIALNUMBER_SIZE (24+1)

struct sfu_sll {
  struct sfu_sll *head;
  struct sfu_sll *next;
};

#define SFU_SLL_TO_DEVICE(sll) \
  ((sfu_device *)((unsigned char*)(sll) - offsetof(sfu_device, ports)))
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

extern void sfu_add_to_ports(sfu_device *dev, sfu_device *other);

#define SFU_GET_MASTER(dev) ((dev)->master_port)
#define SFU_GET_DEVICE_LIST(dev) SFU_SLL_GET_HEAD(&SFU_GET_MASTER(dev)->ports)

typedef struct sfu_device_s {
  char netif_name[NETIF_NAME_SIZE];
  char mac_addr[MAC_ADDR_STR_SIZE];
  char serial_number[SFU_SF_SERIALNUMBER_SIZE];

  uint32_t pci_device_id;
  /* Used for OEM specific operations. Currently only filled on Linux. */
  uint32_t pci_subsystem_id;
  char pci_addr[PCI_ADDR_SIZE];
  unsigned port_index;
#ifndef TARGET_CIM_SERVER_esxi_native
  int phy_type;
#endif
  sfu_device *master_port;
  struct sfu_sll ports;
  int ports_count;

} sfu_device;

/* Initialise device enumeration; returns value < 0 on failure */
int sfu_device_init(void);

/* Enumerate our devices.  Return number of devices found or -1 in
 * case of error.  Set *pdevs to point to an array of sfu_device
 * structures. */
int sfu_device_enum(sfu_device **pdevs);

}
#endif /* !SF_NATIVE_DEVICE_H */

