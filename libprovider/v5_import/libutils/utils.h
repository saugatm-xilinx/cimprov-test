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
/*!  \file  utils.h
** \author  bwh
**  \brief  Utility functions for sfutils
**   \date  2008/10/28
**    \cop  Copyright 2008 Solarflare Communications Inc.
*//************************************************************************/

#ifndef SFUTILS_UTILS_H
#define SFUTILS_UTILS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

extern void byteorder_native_to_le_16(uint16_t* data, size_t count);
extern void byteorder_le_to_native_16(uint16_t* data, size_t count);
extern void byteorder_native_to_le_32(uint32_t* data, size_t count);
extern void byteorder_le_to_native_32(uint32_t* data, size_t count);

uint32_t host_to_le32(uint32_t n);
uint16_t host_to_le16(uint16_t n);
uint32_t le32_to_host(uint32_t n);
uint16_t le16_to_host(uint16_t n);


extern uint32_t crc32_partial(const uint8_t* buf, size_t len, uint32_t crc);

static inline uint8_t sum_bytes(const uint8_t* buf, size_t len)
{
  uint8_t sum = 0;
  while( len-- )
    sum += *buf++;
  return sum;
}

#define NETIF_ATTR_PATH_MAX 100
extern char*
netif_attr_path(char* buf, const char* if_name, const char* attr_name);

struct ethtool_drvinfo;
extern int
netif_get_drvinfo(struct ethtool_drvinfo* einfo, const char* if_name);

#define PCI_ATTR_PATH_MAX 100
extern char*
pci_attr_path(char* buf, const char* bus_addr, const char* attr_name);

#define MAC_ADDR_STR_SIZE (6 * 3)
extern char* netif_mac_addr(const char* if_name, char* buf);
extern char* normalise_mac_addr(const char* mac_addr, char* buf);

extern bool netif_is_ours(const char* if_name);
extern int
strioctl(int fd, int cmd, void *buf, int len);
#define NETIF_DIR "/sys/class/net"
#define DEV_DIR "/dev/"

extern int run_on_esxi;

int open_netdev_fd(const char *if_name);
#endif /* SFUTILS_UTILS_H */
