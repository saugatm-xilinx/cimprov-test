/***************************************************************************//*! \file liprovider/sf_siocefx_nvram.cpp
** <L5_PRIVATE L5_SOURCE>
** \author  OktetLabs
**  \brief  CIM Provider
**   \date  2013/10/02
**    \cop  (c) Solarflare Communications Inc.
** </L5_PRIVATE>
*//*
\**************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <cimple.h>

#include <sys/ioctl.h>
#include "efx_ioctl.h"
#include "ci/tools/byteorder.h"
#include "ci/tools/bitfield.h"
#include "ci/mgmt/mc_flash_layout.h"
#include "efx_regs_mcdi.h"

#include "sf_siocefx_common.h"

#include "sf_utils.h"
#include "sf_logging.h"

extern "C" {
#include "tlv_partition.h"
#include "tlv_layout.h"
#include "endian_base.h"
}

// Size of block to be read from NIC NVRAM at once
#define CHUNK_LEN 0x80

namespace solarflare 
{
    ///
    /// Prepare argument for private SIOCEFX ioctl() call.
    ///
    /// @param isSock             Whether ioctl() will be called on socket
    ///                           or device file fd
    /// @param mcdi_req     [out] Pointer to efx_mcdi_request structure
    ///                           within ioctl() argument
    /// @param iocData      [out] Where to store data required to call
    ///                           SIOCEFX ioctl() on device file fd
    /// @param sockIocData  [out] Where to store data required to call
    ///                           SIOCEFX ioctl() on socket fd
    /// @param iocArg       [out] Where to save pointer to ioctl() argument
    /// @param ifr          [out] Where to save ifreq structure
    /// @param ifname             Network interface name
    ///
    static inline void initIoctlArg(
                    bool isSock,
                    struct efx_mcdi_request *&mcdi_req,
                    struct efx_ioctl &iocData,
                    struct efx_sock_ioctl &sockIocData,
                    void *&iocArg,
                    struct ifreq &ifr,
                    const char *ifname)
    {
        if (isSock)
        {
            memset(&sockIocData, 0, sizeof(sockIocData));
            memset(&ifr, 0, sizeof(ifr));
            strncpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name));
            sockIocData.cmd = EFX_MCDI_REQUEST;
            mcdi_req = &sockIocData.u.mcdi_request;
            ifr.ifr_data = reinterpret_cast<caddr_t>(&sockIocData);
            iocArg = reinterpret_cast<void *>(&ifr);
        }
        else
        {
            memset(&iocData, 0, sizeof(iocData));
            strncpy(iocData.if_name, ifname, sizeof(iocData.if_name));
            iocData.cmd = EFX_MCDI_REQUEST;
            mcdi_req = &iocData.u.mcdi_request;
            iocArg = reinterpret_cast<void *>(&iocData);
        }
    }

    /// Described in sf_siocefx_nvram.h
    int siocEFXReadNVRAM(int fd, bool isSock,
                         uint8_t *data, unsigned int offset,
                         unsigned int len,
                         const char *ifname,
                         uint32_t type)
    {
        unsigned int    end = offset + len;
        unsigned int    chunk;
        uint8_t        *ptr = data;

        struct efx_mcdi_request *mcdi_req;
        struct efx_ioctl         iocData;
        struct efx_sock_ioctl    sockIocData;
        struct ifreq             ifr;
        void                    *iocArg = NULL;
        payload_t                payload;

        for ( ; offset < end; )
        {
            chunk = end - offset;
            if (chunk > CHUNK_LEN)
                chunk = CHUNK_LEN;

            initIoctlArg(isSock, mcdi_req, iocData,
                         sockIocData, iocArg, ifr, ifname);

            mcdi_req->cmd = MC_CMD_NVRAM_READ;
            mcdi_req->len = 12;
            
            memset(&payload, 0, sizeof(payload));
            SET_PAYLOAD_DWORD(payload, 0, type);
            SET_PAYLOAD_DWORD(payload, 1, offset);
            SET_PAYLOAD_DWORD(payload, 2, chunk);
            memcpy(mcdi_req->payload, payload.u8,
                   mcdi_req->len);

            if (ioctl(fd, SIOCEFX, iocArg) < 0)
            {
                PROVIDER_LOG_ERR("ioctl(SIOCEFX/EFX_MCDI_REQUEST) failed, "
                                 "errno %d ('%s')",
                                 errno, strerror(errno));
                return -1;
            }

            memcpy(ptr, mcdi_req->payload, mcdi_req->len);
            ptr += mcdi_req->len;
            offset += mcdi_req->len;
            if (mcdi_req->len == 0)
            {
                PROVIDER_LOG_ERR("MCDI request returned data of "
                                 "zero length");
                return -1;
            }
        }

        return 0;
    }

    /// Described in sf_siocefx_nvram.h
    int siocEFXGetNVRAMPartitionSize(int fd, bool isSock,
                                     const char *ifname,
                                     uint32_t type,
                                     uint32_t &partitionSize)
    {
        struct efx_mcdi_request *mcdi_req;
        struct efx_ioctl         iocData;
        struct efx_sock_ioctl    sockIocData;
        struct ifreq             ifr;
        void                    *iocArg = NULL;
        payload_t                payload;

        initIoctlArg(isSock, mcdi_req, iocData,
                     sockIocData, iocArg, ifr, ifname);

        mcdi_req->cmd = MC_CMD_NVRAM_INFO;
        mcdi_req->len = 12;

        memset(&payload, 0, sizeof(payload));
        SET_PAYLOAD_DWORD(payload, 0, type);
        memcpy(mcdi_req->payload, &payload,
               sizeof(*(mcdi_req->payload)));

        if (ioctl(fd, SIOCEFX, iocArg) < 0)
        {
            PROVIDER_LOG_ERR("ioctl(SIOCEFX/MC_CMD_NVRAM_INFO) "
                             "failed with errno %d ('%s')",
                             errno, strerror(errno));
            return -1;
        }

        memcpy(payload.u8, mcdi_req->payload,
               mcdi_req->len);
        partitionSize = PAYLOAD_DWORD(payload, 1);
        return 0;
    }

    /// Described in sf_siocefx_nvram.h
    int siocEFXReadNVRAMPartition(int fd, bool isSock,
                                  const char *ifname,
                                  uint32_t type,
                                  uint8_t *&data,
                                  uint32_t &dataLen)
    {
        if (siocEFXGetNVRAMPartitionSize(fd, isSock,
                                         ifname, type,
                                         dataLen) < 0)
            return -1;

        data = new uint8_t[dataLen];
        if (siocEFXReadNVRAM(fd, isSock, data, 0,
                             dataLen,
                             ifname, type) < 0)
        {
            delete[] data;
            return -1;
        }

        return 0;
    }

    /// Described in sf_siocefx_nvram.h
    int siocEFXGetBootROMVersionSiena(const char *ifname,
                                      int port_index,
                                      int fd, bool isSock,
                                      VersionInfo &ver)
    {
        uint32_t     nvram_type;
        char        *buf = NULL;

        siena_mc_dynamic_config_hdr_t    partial_hdr;
        siena_mc_dynamic_config_hdr_t   *header;
        const siena_mc_fw_version_t     *item;

        nvram_type = port_index == 0 ?
                        MC_CMD_NVRAM_TYPE_DYNAMIC_CFG_PORT0 :
                                MC_CMD_NVRAM_TYPE_DYNAMIC_CFG_PORT1;

        if (siocEFXReadNVRAM(fd, isSock, (uint8_t *)&partial_hdr, 0,
                             sizeof(partial_hdr),
                             ifname, nvram_type) < 0)
        {
            PROVIDER_LOG_ERR("%s(): failed to get partial "
                             "header from NVRAM",
                             __FUNCTION__);
            return -1;
        }

        buf = new char[partial_hdr.length];

        if (siocEFXReadNVRAM(fd, isSock, (uint8_t *)buf, 0,
                             partial_hdr.length,
                             ifname, nvram_type) < 0)
        {
            PROVIDER_LOG_ERR("%s(): failed to get header from NVRAM",
                             __FUNCTION__);
            delete[] buf;
            return -1;
        }

        header = reinterpret_cast<siena_mc_dynamic_config_hdr_t *>(buf);
        item = &header->fw_version[MC_CMD_NVRAM_TYPE_EXP_ROM];
        ver = VersionInfo(item->version_w, item->version_x,
                          item->version_y, item->version_z);
        delete[] buf;
        return 0;
    }

#if __BYTE_ORDER == __ORDER_LITTLE_ENDIAN
void byteorder_le_to_native_16(uint16_t* data, size_t count) {}
#else
void byteorder_le_to_native_16(uint16_t* data, size_t count)
{
  uint8_t* bytes = (uint8_t*)data;
  size_t addr;

  for( addr = 0; addr < count; addr++ )
    data[addr] = (bytes[2 * addr] |
                  (bytes[2 * addr + 1] << 8));
}
#endif

    struct firmware_version
    {
        uint16_t a;
        uint16_t b;
        uint16_t c;
        uint16_t d;
    };

    /// Described in sf_siocefx_nvram.h
    int siocEFXGetBootROMVersionEF10(const char *ifname,
                                     int port_index,
                                     int fd, bool isSock,
                                     VersionInfo &ver)
    {
        nvram_partition_t  partition;
        uint8_t           *nvram_data = NULL;
        uint32_t           nvram_data_len;
        uint32_t           nvram_type;
        int                rc;

        struct firmware_version result;

        result.a = 0;
        result.b = 0;
        result.c = 0;
        result.d = 0;

        nvram_type = NVRAM_PARTITION_TYPE_DYNAMIC_CONFIG;

        if (siocEFXReadNVRAMPartition(fd, isSock, ifname,
                                      nvram_type, nvram_data,
                                      nvram_data_len) < 0)
        {
            PROVIDER_LOG_ERR("Failed to read VPD from NVRAM");
            return -1;
        }

        if ((rc = tlv_init_partition_from_buffer(
                                &partition, nvram_data,
                                nvram_data_len)) != TLV_OK)
        {
            PROVIDER_LOG_ERR("tlv_init_partition_from_buffer() returned %d",
                             rc);
            delete[] nvram_data;
            return -1;
        }

        if ((rc =
              tlv_find(
                  &partition.tlv_cursor,
                  TLV_TAG_PARTITION_VERSION(
                  NVRAM_PARTITION_TYPE_EXPANSION_ROM))) != TLV_OK)
        {
            PROVIDER_LOG_ERR("tlv_find() returned %d", rc);
            delete[] nvram_data;
            return -1;
        }

        memcpy(&result, &(partition.tlv_cursor.current[2]), sizeof(result));
        /* The nvram is little-endian. */
        byteorder_le_to_native_16((uint16_t*)&result,
                                  sizeof(result) / sizeof(uint16_t));

        ver = VersionInfo(result.a, result.b,
                          result.c, result.d);
        delete[] nvram_data;
        return 0;
    }
}
