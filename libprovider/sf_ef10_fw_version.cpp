/***************************************************************************//*! \file liprovider/sf_ef10_fw_version.cpp
** <L5_PRIVATE L5_SOURCE>
** \author  OktetLabs
**  \brief  CIM Provider
**   \date  2013/11/21
**    \cop  (c) Solarflare Communications Inc.
** </L5_PRIVATE>
*//*
\**************************************************************************/

#include "sf_logging.h"
#include "sf_utils.h"
#include "sf_siocefx_nvram.h"
#include <stdint.h>

extern "C" {
#include "tlv_partition.h"
#include "tlv_layout.h"
#include "endian_base.h"
}

#include "efx_regs_mcdi.h"

namespace solarflare
{

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

    /// Described in sf_ef10_fw_version.h
    int getBootROMVersionEF10Gen(uint8_t *buffer,
                                 uint32_t length,
                                 VersionInfo &ver)
    {
        nvram_partition_t  partition;
        int                rc;

        struct firmware_version result;

        result.a = 0;
        result.b = 0;
        result.c = 0;
        result.d = 0;

        if ((rc = tlv_init_partition_from_buffer(
                                &partition, buffer, length)) != TLV_OK)
        {
            PROVIDER_LOG_ERR("tlv_init_partition_from_buffer() returned %d",
                             rc);
            return -1;
        }

        if ((rc =
              tlv_find(
                  &partition.tlv_cursor,
                  TLV_TAG_PARTITION_VERSION(
                  NVRAM_PARTITION_TYPE_EXPANSION_ROM))) != TLV_OK)
        {
            PROVIDER_LOG_ERR("tlv_find() returned %d", rc);
            return -1;
        }

        memcpy(&result, &(partition.tlv_cursor.current[2]), sizeof(result));
        /* The nvram is little-endian. */
        byteorder_le_to_native_16((uint16_t*)&result,
                                  sizeof(result) / sizeof(uint16_t));

        ver = VersionInfo(result.a, result.b,
                          result.c, result.d);
        return 0;
    }
}
