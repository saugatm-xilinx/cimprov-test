/***************************************************************************//*! \file liprovider/sf_ef10_fw_version.h
** <L5_PRIVATE L5_SOURCE>
** \author  OktetLabs
**  \brief  CIM Provider
**   \date  2013/11/21
**    \cop  (c) Solarflare Communications Inc.
** </L5_PRIVATE>
*//*
\**************************************************************************/

#ifndef SOLARFLARE_SF_EF10_FW_VERSION_H
#define SOLARFLARE_SF_EF10_FW_VERSION_H 1

#include "sf_utils.h"
#include <stdint.h>

namespace solarflare 
{
    ///
    /// Get BootROM verion after having read dynamic configuration
    /// partition from EF10 NIC.
    ///
    /// @param buffer       Buffer where dynamic configuration data is
    ///                     stored
    /// @param length       Length of data in the buffer
    /// @param ver    [out] Where to store obtained BootROM version
    ///
    /// @return 0 on success, -1 on failure
    ///
    int getBootROMVersionEF10Gen(uint8_t *buffer,
                                 uint32_t length,
                                 VersionInfo &ver);
}

#endif // SOLARFLARE_SF_EF10_FW_VERSION_H
