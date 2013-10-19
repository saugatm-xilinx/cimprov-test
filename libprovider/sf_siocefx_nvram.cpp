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

// Size of block to be read from NIC NVRAM at once
#define CHUNK_LEN 0x80

namespace solarflare 
{
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
                CIMPLE_ERR(("ioctl(SIOCEFX/EFX_MCDI_REQUEST) failed, "
                            "errno %d ('%s')",
                            errno, strerror(errno)));
                return -1;
            }

            memcpy(ptr, mcdi_req->payload, mcdi_req->len);
            ptr += mcdi_req->len;
            offset += mcdi_req->len;
            if (mcdi_req->len == 0)
            {
                CIMPLE_ERR(("MCDI request returned data of zero length"));
                return -1;
            }
        }

        return 0;
    }

    /// Described in sf_siocefx_nvram.h
    int siocEFXGetBootROMVersion(const char *ifname,
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
            CIMPLE_ERR(("%s(): failed to get partial header from NVRAM",
                        __FUNCTION__));
            return -1;
        }

        buf = new char[partial_hdr.length];

        if (siocEFXReadNVRAM(fd, isSock, (uint8_t *)buf, 0,
                             partial_hdr.length,
                             ifname, nvram_type) < 0)
        {
            CIMPLE_ERR(("%s(): failed to get header from NVRAM",
                        __FUNCTION__));
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
}
