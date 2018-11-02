/**************************************************************************/
/*!  \file  sf_native_nv.h
** \author  smitra
**  \brief  Functions for access to an nvram partition.
**   \date  2018/04/10
**    \cop  Copyright 2018 Solarflare Communications Inc.
*//************************************************************************/

#ifndef SF_NATIVE_NV_H
#define SF_NATIVE_NV_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#include <sys/types.h>
#include "userMgmtSrc/sfvmk_mgmt_interface.h"

namespace solarflare {
  #define NETIF_NAME_SIZE 64

  typedef struct nv_context_ {
    size_t            part_size;
    sfvmk_nvramType_t nv_image_type;
    char              devName[NETIF_NAME_SIZE];
  } nv_context;

  typedef struct sfu_device_s sfu_device;

  enum nv_part_type {
    NV_PART_MCFW,
    NV_PART_DYNAMIC_CFG,
    NV_PART_STATIC_CFG,
    NV_PART_BOOTROM,
    NV_PART_BOOTROM_CFG,
    NV_PART_PHY,
    NV_PART_FPGA,
    NV_PART_FCFW,
    NV_PART_CPLD,
    NV_PART_FPGA_BACKUP,
    NV_PART_LICENSE,
    NV_PART_MUMFW,
    NV_PART_UEFI,
    NV_PART_STATUS,
    NV_PART_NTYPES
  };

  /* Initialise NVRAM access layer */
  int nv_init(void);

  /* Check that a device has boot ROM access.  If [nvDev] is not NULL and this
  ** function returns false then errno will be EOPNOTSUPP and *nvDev will either
  ** be NULL, or will contain a port that can give NV access. If [nvDev] is
  ** NULL then no attempt will be made to find an alternative port. */
  bool
  nv_exists(const sfu_device *, enum nv_part_type type, unsigned subtype,
            const sfu_device **nv_dev);

  /* Try to open the given device - makes no attempt to use other devices in
  ** the group (unlike nv_open_ports()). */
  nv_context* nv_open(const sfu_device *dev, enum nv_part_type type,
                      unsigned subtype);

  /* Try to open [type]/[subtype]. If ![ignore_dev] then start by trying to
  ** open [dev] and, if that fails (or [ignore_dev] is set), try to open
  ** using the other devices in the group of which [dev] is a member.
  ** NOTE: experience has shown that a simple open is not necessarily sufficient
  ** to determine whether NV access is possible, therefore callers might consider
  ** making a test read.
  ** On return, if the function succeeds, [dev_opened] will contain the device
  ** that the function opened; it may/may not be the same value as [dev]. */
  nv_context*
  nv_open_ports(const sfu_device *dev, enum nv_part_type type,
                unsigned subtype, const sfu_device **dev_opened,
                bool ignore_dev);
  int nv_read_all(nv_context *context, void *buf);
  int nv_write_all(nv_context *context, const void *buf,
                   uint32_t length, bool full_erase,
                   void (*progress)(const char *status, int percent));
  int nv_read(nv_context *nv, char *buf, size_t nbytes, off_t offset);
  void nv_close(nv_context *context);
  size_t nv_part_size(const nv_context *nv);
}

#endif /* SF_NATIVE_NV_H */

