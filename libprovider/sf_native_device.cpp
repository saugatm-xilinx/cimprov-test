#include "sf_native_nv.h"
#include "sf_native_device.h"
#include "sf_native_vmware.h"
#include "sf_logging.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <sys/utsname.h>
#include <stdlib.h>

namespace solarflare {

  extern  int getNICNameList(sfvmk_ifaceList_t *ifaceList);

  static struct sfu_sll *walk_to_end(struct sfu_sll *head) {
    struct sfu_sll *p = SFU_SLL_GET_HEAD(head);

    while(p->next)
      p = p->next;

    return p;
  }

  void sfu_add_to_ports(sfu_device *dev, sfu_device *other) {
    struct sfu_sll *last=walk_to_end(&(dev)->ports);
    assert(SFU_PORTS_NOT_INIT(other));
    last->next = &(other)->ports;
    (other)->ports.next = NULL;
    ++(dev)->ports_count;
  }

  static void add_device(sfu_device *master, sfu_device *other) {
    assert(master);
    assert(other);
    assert(master != other);

    other->master_port = master;
    sfu_add_to_ports(master, other);
  }

  /* Match up ports with the same serial number */
  void sfu_group_devices(sfu_device **pdevs, int ndevs) {
    int i, j;

    assert(pdevs);
    assert(ndevs >= 0);

    for(i = 0; i < ndevs; ++i) {
      sfu_device *dev = *pdevs + i;

      /* It's part of a group and, as we're setting up masters here,
      ** it is not a master port, so we can skip it */
      if(dev->master_port != NULL)
        continue;

      /* So we make it a master. */
      SFU_INIT_PORTS(dev);
      dev->master_port = dev;

      for(j = i+1; j < ndevs; ++j) {
        sfu_device  *dev2 = *pdevs + j;

        /* If the device is not yet in a group, see if it can be part
        ** of this one. */
        if(dev2->master_port == NULL &&
           strcmp(dev->serial_number, dev2->serial_number) == 0) {
          add_device(dev, dev2);
        }
      }
    }
  }

  static int sfu_dev_name_cmp(const void *dev1, const void *dev2)
  {
    if((strlen((char *)((const sfu_device*)dev1)->netif_name)) ==
      (strlen((char*)((const sfu_device*)dev2)->netif_name)))
      return strcmp(((const sfu_device *)dev1)->netif_name,
                    ((const sfu_device *)dev2)->netif_name);
    else
      return strlen(((const sfu_device*)dev1)->netif_name) -
      strlen(((const sfu_device*)dev2)->netif_name);
  }

  static int get_pci_addr_nums(const char *addr,
                               unsigned int *dom,
                               unsigned int *bus,
                               unsigned int *dev,
                               unsigned int *fn)
  {
    assert(addr != NULL && dom != NULL && bus != NULL &&
           dev != NULL && fn != NULL);

    if( sscanf(addr, "%x:%x:%x.%x", dom, bus, dev, fn) != 4 )
      return -1;

    return 0;
  }

  static int parse_pci_addr(const char *addr, uint32_t *parsed_addr)
  {
    unsigned dom, bus, dev, fn;

    assert(addr != NULL && parsed_addr != NULL);

    if( get_pci_addr_nums(addr, &dom, &bus, &dev, &fn) != 0 )
      return -1;
    *parsed_addr = (dom << 16) | (bus << 8) | (dev << 3) | fn;
    return 0;
  }

  /*
   * Fills up the sfu_device structure for a interface
   *
   * @param dev          pointer to sfu_device type
   * @param netif_name   char pointer
   *
   * @return > 0         for success
   * @return < 0         failure in parsing
   * @return 0           Ignore the device
   */
  static int
  device_fill(sfu_device *dev, const char *netif_name)
  {
    uint32_t pci_addr;
    sfvmk_macAddress_t   macAddress;
    sfvmk_pciInfo_t pciInfo;
    sfvmk_vpdInfo_t vpdInfo;
    int i = 0;
    char SN_keyWord[3]= {"SN"};
    strncpy(dev->netif_name, netif_name, sizeof(dev->netif_name) - 1);

    if (DrvMgmtCall(netif_name, SFVMK_CB_MAC_ADDRESS_GET, &macAddress) != VMK_OK) {
        PROVIDER_LOG_ERR("%s(): DrvMgmtCall failed for MAC Adderss", __FUNCTION__);
        goto ignore;
    }
    snprintf(dev->mac_addr, MAC_ADDR_STR_SIZE,
           "%02X-%02X-%02X-%02X-%02X-%02X",
           macAddress.macAddress[0],
           macAddress.macAddress[1],
           macAddress.macAddress[2],
           macAddress.macAddress[3],
           macAddress.macAddress[4],
           macAddress.macAddress[5]);

    if (DrvMgmtCall(netif_name, SFVMK_CB_PCI_INFO_GET, &pciInfo) != VMK_OK) {
      PROVIDER_LOG_ERR("%s(): DrvMgmtCall failed for PCI Info", __FUNCTION__);
      goto ignore;
    }
    dev->pci_device_id =  pciInfo.vendorId << 16 | pciInfo.deviceId;
    dev->pci_subsystem_id = pciInfo.subVendorId << 16 | pciInfo.subDeviceId;
    strncpy(dev->pci_addr, vmk_NameToString(&pciInfo.pciBDF), PCI_ADDR_SIZE);
    if( parse_pci_addr(dev->pci_addr, &pci_addr) != 0 )
      return -1;
    dev->port_index = pci_addr & 0xff;

    vpdInfo.vpdOp = SFVMK_MGMT_DEV_OPS_GET;
    vpdInfo.vpdTag = VPD_TAG_R;
    vpdInfo.vpdKeyword = SN_keyWord[0] | SN_keyWord[1] << 8;
    if ((DrvMgmtCall(netif_name, SFVMK_CB_VPD_REQUEST, &vpdInfo)) != VMK_OK) {
      PROVIDER_LOG_ERR("%s(): DrvMgmtCall failed for VPD Request", __FUNCTION__);
      goto ignore;
    }
    strncpy(dev->serial_number, (const char *)vpdInfo.vpdPayload, vpdInfo.vpdLen);

    /* at start of day the group state of a port is undecided */
    dev->ports.next = NULL;
    dev->master_port = NULL;

    return 1;

    ignore:
      PROVIDER_LOG_ERR("%s(): Ignoring invalid interface %s", __FUNCTION__, netif_name);
      return 0;
  }

  int sfu_device_init(void)
  {
    struct utsname buf;
    int val = 0;
    if (uname(&buf) == 0)
      if (strcmp(buf.sysname, "VMkernel") == 0)
          return 0;
      else
        PROVIDER_LOG_ERR("%s(): Host not an esxi machine", __FUNCTION__);
    else
      PROVIDER_LOG_ERR("%s(): Failed to get system details", __FUNCTION__);
    return -1;
  }

  int sfu_device_enum(sfu_device **pdevs)
  {
    sfvmk_ifaceList_t    ifaceList;
    sfu_device *dev;
    int count, i;
    int matched;
    int rc;

    if (getNICNameList(&ifaceList) < 0)
    {
       PROVIDER_LOG_ERR("%s(): Getting  NIC Name List Failed", __FUNCTION__);
       return -1;
    }
    count = ifaceList.ifaceCount;
    *pdevs = (sfu_device*)calloc(count, sizeof(**pdevs));
    if( !*pdevs ) {
      rc = -1;
      goto out;
    }

    dev = *pdevs;
    rc = 0;
    for( i = 0; i < count; i++ ) {
      matched = device_fill(dev, ifaceList.ifaceArray[i].string);
      if( matched < 0 ) {
        rc = -1;
        goto out;
      }
      if( matched > 0 ) {
        ++dev;
        ++rc;
      }
    }

    /* Sort the devices by netif name.
     * This fixes the enumeration order, which is expected by update_firmware().
     */
    assert(rc >= 0);
    qsort(*pdevs, rc, sizeof(sfu_device), sfu_dev_name_cmp);
    sfu_group_devices(pdevs, rc);

   out:
    if( rc < 0 )
      free(*pdevs);
    return rc;
  }

}
