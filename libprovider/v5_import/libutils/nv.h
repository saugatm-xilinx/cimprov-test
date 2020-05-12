/*
 * Copyright 2019 Xilinx, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**************************************************************************/
/*!  \file  nv.h
** \author  gel
**  \brief  Functions for access to an nvram partition.
**   \date  2009/12/10
**    \cop  Copyright 2009 Solarflare Communications Inc.
*//************************************************************************/

#ifndef SFUTILS_NV_H
#define SFUTILS_NV_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#include <sys/types.h>

struct nv_context;
struct sfu_device;

enum nv_part_type {
  NV_PART_MCFW,
  NV_PART_DYNAMIC_CFG,
  NV_PART_STATIC_CFG,
  NV_PART_BOOTROM,
  NV_PART_BOOTROM_CFG,
  NV_PART_FPGA,
  NV_PART_FCFW,
  NV_PART_FPGA_BACKUP,
  NV_PART_LICENSE,
  NV_PART_MUMFW,
  NV_PART_UEFI,
  NV_PART_STATUS,
  NV_PART_NTYPES,
  NV_PART_DYNAMIC_CFG_DEFAULTS,
  NV_PART_BOOTROM_CFG_DEFAULTS,
  NV_PART_BUNDLE,
  NV_PART_BUNDLE_METADATA,
  /* !Insert new entries above here! */
  MAX_NV_PART,
};

#if defined(__sun) && defined (__SVR4)
#include "sfxge_ioc.h"

int nv_strioctl(struct nv_context* nv, int cmd, void* buf, int len);
#endif

#if defined(__FreeBSD__)
#include "sfxge_ioc.h"

int
nv_perform_ioctl(const struct nv_context *nv, sfxge_ioc_t* ioc);
#endif

/* Initialise NVRAM access layer */
int nv_init(void);

/* Check that a device has boot ROM access.  If [nvDev] is not NULL and this
** function returns false then errno will be EOPNOTSUPP and *nvDev will either
** be NULL, or will contain a port that can give NV access. If [nvDev] is
** NULL then no attempt will be made to find an alternative port. */
bool
nv_exists(const struct sfu_device*, enum nv_part_type type, unsigned subtype,
          const struct sfu_device** nv_dev);

/* Try to open the given device - makes no attempt to use other devices in
** the group (unlike nv_open_ports()). */
struct nv_context*
nv_open(const struct sfu_device* dev, enum nv_part_type type, unsigned subtype);

/* Try to open [type]/[subtype]. If ![ignore_dev] then start by trying to
** open [dev] and, if that fails (or [ignore_dev] is set), try to open
** using the other devices in the group of which [dev] is a member.
** NOTE: experience has shown that a simple open is not necessarily sufficient
** to determine whether NV access is possible, therefore callers might consider
** making a test read.
** On return, if the function succeeds, [dev_opened] will contain the device
** that the function opened; it may/may not be the same value as [dev]. */
struct nv_context*
nv_open_ports(const struct sfu_device* dev, enum nv_part_type type,
              unsigned subtype, const struct sfu_device** dev_opened,
              bool ignore_dev);

int nv_read_all(struct nv_context* context, void* buf);
int nv_write_all(struct nv_context* context, const void* buf, uint32_t length,
                 bool full_erase,
                 void (*progress)(const char* status, int percent));
ssize_t nv_read(struct nv_context* nv, char* buf, size_t nbytes, off_t offset);
void nv_close(struct nv_context* context);
size_t nv_part_size(const struct nv_context* nv);
bool has_factory_defaults(const struct sfu_device* dev);

#endif /* SFUTILS_NV_H */
