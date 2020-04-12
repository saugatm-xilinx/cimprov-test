#include "sf_native_nv.h"
#include "sf_native_device.h"
#include "sf_native_vmware.h"
#include "sf_utils.h"
#include "sf_logging.h"
#include <assert.h>
#include <stddef.h>
#include <errno.h>
#include <string.h>

namespace solarflare {

  static sfvmk_nvramType_t mgmt_part_type(enum nv_part_type type)
  {
    switch( type ) {
      case NV_PART_MCFW:
        return SFVMK_NVRAM_MC;
      case NV_PART_FCFW:
        return SFVMK_NVRAM_FCFW;
      case NV_PART_DYNAMIC_CFG:
        return SFVMK_NVRAM_DYNAMIC_CFG;
      case NV_PART_STATIC_CFG:
        return SFVMK_NVRAM_INVALID;
      case NV_PART_BOOTROM:
        return SFVMK_NVRAM_BOOTROM;
      case NV_PART_BOOTROM_CFG:
        return SFVMK_NVRAM_BOOTROM_CFG;
      case NV_PART_PHY:
        return SFVMK_NVRAM_PHY;
      case NV_PART_FPGA:
        return SFVMK_NVRAM_FPGA;
      case NV_PART_CPLD:
        return SFVMK_NVRAM_CPLD;
      case NV_PART_FPGA_BACKUP:
        return SFVMK_NVRAM_FPGA_BACKUP;
      case NV_PART_LICENSE:
        return SFVMK_NVRAM_INVALID;
      case NV_PART_MUMFW:
        return SFVMK_NVRAM_INVALID;
      case NV_PART_DYNAMIC_CFG_DEFAULTS:
        return SFVMK_NVRAM_DYNAMIC_CFG;
      case NV_PART_BOOTROM_CFG_DEFAULTS:
        return SFVMK_NVRAM_BOOTROM_CFG;
      case NV_PART_BUNDLE:
        return SFVMK_NVRAM_BUNDLE;
      case NV_PART_BUNDLE_METADATA:
        return SFVMK_NVRAM_BUNDLE_METADATA;
      default:
        assert(!"invalid NV type");
        return SFVMK_NVRAM_INVALID;
    }
  }

  nv_context* nv_open(const sfu_device *dev, enum nv_part_type type,
                      unsigned subtype)
  {
    sfvmk_nvramCmd_t nvram_partition;
    nv_context *nv = (nv_context*) malloc(sizeof(nv_context));

    if (!nv)
      return NULL;

    nvram_partition.type = mgmt_part_type(type);
    if (nvram_partition.type  == SFVMK_NVRAM_INVALID)
      return NULL;

    nvram_partition.op = SFVMK_NVRAM_OP_GET_VER;
    if (DrvMgmtCall((char*)dev->netif_name, SFVMK_CB_NVRAM_REQUEST, &nvram_partition) != VMK_OK) {
        PROVIDER_LOG_ERR("%s(): NVRAM get version failed", __FUNCTION__);
        return NULL;
    }
    if (subtype != nvram_partition.subtype) {
        return NULL;
    }


    memset(&nvram_partition, 0, sizeof(sfvmk_nvramCmd_t));
    nvram_partition.type = mgmt_part_type(type);
    nvram_partition.op = SFVMK_NVRAM_OP_SIZE;
    if (DrvMgmtCall((char*)dev->netif_name, SFVMK_CB_NVRAM_REQUEST, &nvram_partition) != VMK_OK)
    {
      free(nv);
      return NULL;
    }
    nv->nv_image_type = nvram_partition.type;
    nv->part_size = nvram_partition.size;
    strcpy(nv->devName, dev->netif_name);
    return nv;
  }

  static bool test_exists(const sfu_device *dev,
                 enum nv_part_type type, unsigned subtype)
  {
    nv_context* nv;
    if (nv = nv_open(dev, type, subtype) ) {
      free (nv);
      return true;
    } else {
      return false;
    }
  }

  int nv_init(void)
  {
    return 0;
  }

  nv_context*
  nv_open_ports(const sfu_device *dev, enum nv_part_type type, unsigned subtype,
                const sfu_device **dev_opened, bool ignore_dev) {
    struct sfu_sll *list;
    nv_context *rc = NULL;
    const sfu_device *dev2;

    assert(dev);
    assert(dev_opened);

    if (!ignore_dev) {
      dev2 = dev;
      rc = nv_open(dev, type, subtype);
    }

    if (!rc) {
      list = SFU_GET_DEVICE_LIST(dev);
      assert(list);

      do {
        dev2 = SFU_SLL_TO_DEVICE(list);
        if(dev != dev2)
            rc = nv_open(dev2, type, subtype);
        list = list->next;
      } while (!rc && !SFU_SLL_IS_END(list));
    }

    if (rc) {
      *dev_opened = dev2;
    }

    return rc;
  }
  /* If nv_dev is NULL then this will look for the type and subtype only on dev.
   * If nv_dev is not NULL then it will look on all ports of dev to find the type
   * and subtype and will return True if it is found on any of them, with the port
   * in *nv_dev, otherwise it will return False and *nv_dev will be NULL. */
  bool nv_exists(const sfu_device *dev,
                          enum nv_part_type type, unsigned subtype,
                          const sfu_device **nv_dev)
  {
    const sfu_device *dev_opened;
    nv_context *ctxt = NULL;
    bool rc = false;

    if(!nv_dev)
      return test_exists(dev, type, subtype);

    rc = nv_open_ports(dev, type, subtype, &dev_opened, false);
    if (rc) {
      nv_close(ctxt);
      *nv_dev = dev_opened;
    } else
      *nv_dev = NULL;
    return rc;
  }

  void nv_close(nv_context *nv)
  {
    free(nv);
  }

  int
  nv_read(nv_context *nv, char *buf,
                   size_t length, off_t offset)
  {
    sfvmk_nvramCmd_t nvram_partition;

    nvram_partition.type = nv->nv_image_type;
    nvram_partition.op = SFVMK_NVRAM_OP_READ;
    nvram_partition.offset = offset;
    nvram_partition.size = length;
    /* This is a workaround for  Esxi 6.0 on which if the MSB of pointer
     * has "FF" in the address and if we typecast it into 64 bit unsigned integer
     * value then it fill rest of the upper 32 bits with all "1".
     */
#if VMKAPI_REVISION <= VMK_REVISION_FROM_NUMBERS(2, 5, 0, 0)
    nvram_partition.data = ((vmk_uint64)(vmk_uint32)buf);
#else
    nvram_partition.data = ((vmk_uint64)buf);
#endif

    if (DrvMgmtCall(nv->devName, SFVMK_CB_NVRAM_REQUEST, &nvram_partition) != VMK_OK)
      return -1;
    // Assuming the nv_read command fills up the length with the number of bytes read.
    return nvram_partition.size;
  }

  int nv_read_all(nv_context *nv, void *buf)
  {
    int rc = nv_read(nv, (char*)buf, nv->part_size, 0);
    if( rc == (ssize_t)nv->part_size )
        return 0;
    if( rc >= 0 )
        errno = -EIO;
    return -1;
  }

  int nv_write_all(nv_context *nv, const void *buf,
                   uint32_t length, bool full_erase,
                   void (*progress)(const char *status,
                   int percent))
  {
    UNUSED(progress);
    sfvmk_nvramCmd_t nvram_partition;

    nvram_partition.type = nv->nv_image_type;
    nvram_partition.op = SFVMK_NVRAM_OP_WRITEALL;
    nvram_partition.size = length;
    /* This is a workaround for  Esxi 6.0 on which if the MSB of pointer
     * has "FF" in the address and if we typecast it into 64 bit unsigned integer
     * value then it fill rest of the upper 32 bits with all "1".
     */
#if VMKAPI_REVISION <= VMK_REVISION_FROM_NUMBERS(2, 5, 0, 0)
    nvram_partition.data =  ((vmk_uint64)(vmk_uint32)buf);
#else
    nvram_partition.data =  ((vmk_uint64)buf);
#endif
    nvram_partition.erasePart = full_erase;

    if (DrvMgmtCall(nv->devName, SFVMK_CB_NVRAM_REQUEST, &nvram_partition) != VMK_OK)
      return 0;

    return nvram_partition.size;
  }

  size_t
  nv_part_size(const nv_context *nv)
  {
    return nv->part_size;
  }

}
