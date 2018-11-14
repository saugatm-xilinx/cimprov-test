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
#include "sf_ef10_fw_version.h"

#include "sf_utils.h"
#include "sf_logging.h"

extern "C" {
#include "tlv_partition.h"
#include "tlv_layout.h"
#include "endian_base.h"
}

namespace solarflare
{
#ifndef TARGET_CIM_SERVER_esxi_native
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

#define min(a, b) (((a) < (b)) ? (a) : (b))

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

        for ( ; offset < end; )
        {
            chunk = min(end - offset, sizeof(mcdi_req->payload));

            initIoctlArg(isSock, mcdi_req, iocData,
                         sockIocData, iocArg, ifr, ifname);

            mcdi_req->cmd = MC_CMD_NVRAM_READ;
            mcdi_req->len = MC_CMD_NVRAM_READ_IN_LEN;
            mcdi_req->payload[MC_CMD_NVRAM_READ_IN_TYPE_OFST / 4] =
                host_to_le32(type);
            mcdi_req->payload[MC_CMD_NVRAM_READ_IN_OFFSET_OFST / 4] =
                host_to_le32(offset);
            mcdi_req->payload[MC_CMD_NVRAM_READ_IN_LENGTH_OFST / 4] =
                host_to_le32(chunk);

            if (ioctl(fd, SIOCEFX, iocArg) < 0 ||
                mcdi_req->rc != 0)
            {
                if (mcdi_req->rc != 0)
                    errno = mcdi_req->rc;

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
    int siocEFXWriteNVRAM(int fd, bool isSock,
                          uint8_t *data,
                          uint32_t offset,
                          uint32_t len,
                          const char *ifname,
                          uint32_t type)
    {
        unsigned int    end = offset + len;
        uint8_t        *ptr = data;
        unsigned int    write_len;

        struct efx_mcdi_request *mcdi_req;
        struct efx_ioctl         iocData;
        struct efx_sock_ioctl    sockIocData;
        struct ifreq             ifr;
        void                    *iocArg = NULL;
        uint32_t                *payload;

        for ( ; offset < end; )
        {
            initIoctlArg(isSock, mcdi_req, iocData,
                         sockIocData, iocArg, ifr, ifname);

            write_len = min(len,
                            sizeof(mcdi_req->payload) -
                                MC_CMD_NVRAM_WRITE_IN_LEN(0));

            mcdi_req->cmd = MC_CMD_NVRAM_WRITE;
            mcdi_req->len = MC_CMD_NVRAM_WRITE_IN_LEN(write_len);

            payload = mcdi_req->payload;
            memset(payload, 0, sizeof(mcdi_req->payload));
            payload[MC_CMD_NVRAM_WRITE_IN_TYPE_OFST / 4] =
                                              host_to_le32(type);
            payload[MC_CMD_NVRAM_WRITE_IN_OFFSET_OFST / 4] =
                                              host_to_le32(offset);
            payload[MC_CMD_NVRAM_WRITE_IN_LENGTH_OFST / 4] =
                                              host_to_le32(write_len);
            memcpy(&payload[MC_CMD_NVRAM_WRITE_IN_WRITE_BUFFER_OFST/ 4],
                   ptr, write_len);

            if (ioctl(fd, SIOCEFX, iocArg) < 0 ||
                mcdi_req->rc != 0)
            {
                if (mcdi_req->rc != 0)
                    errno = mcdi_req->rc;

                PROVIDER_LOG_ERR("ioctl(SIOCEFX/EFX_MCDI_REQUEST) failed, "
                                 "errno %d ('%s')",
                                 errno, strerror(errno));
                return -1;
            }

            ptr += write_len;
            offset += write_len;
            len -= write_len;
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

        initIoctlArg(isSock, mcdi_req, iocData,
                     sockIocData, iocArg, ifr, ifname);

        mcdi_req->cmd = MC_CMD_NVRAM_INFO;
        mcdi_req->len = MC_CMD_NVRAM_INFO_IN_LEN;
        mcdi_req->payload[MC_CMD_NVRAM_INFO_IN_TYPE_OFST / 4] =
                                                host_to_le32(type);

        if (ioctl(fd, SIOCEFX, iocArg) < 0)
        {
            PROVIDER_LOG_ERR("ioctl(SIOCEFX/MC_CMD_NVRAM_INFO) "
                             "failed with errno %d ('%s')",
                             errno, strerror(errno));
            return -1;
        }

        partitionSize = le32_to_host(
                  mcdi_req->payload[MC_CMD_NVRAM_INFO_OUT_SIZE_OFST / 4]);
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

    /// Described in sf_siocefx_nvram.h
    int siocEFXGetBootROMVersionEF10(const char *ifname,
                                     int port_index,
                                     int fd, bool isSock,
                                     VersionInfo &ver)
    {
        uint8_t           *nvram_data = NULL;
        uint32_t           nvram_data_len;
        uint32_t           nvram_type;
        int                rc;

        nvram_type = NVRAM_PARTITION_TYPE_DYNAMIC_CONFIG;

        if (siocEFXReadNVRAMPartition(fd, isSock, ifname,
                                      nvram_type, nvram_data,
                                      nvram_data_len) < 0)
        {
            PROVIDER_LOG_ERR("Failed to read VPD from NVRAM");
            return -1;
        }

        rc = getBootROMVersionEF10Gen(nvram_data, nvram_data_len,
                                      ver);
        delete[] nvram_data;
        return rc;
    }
#endif
}
