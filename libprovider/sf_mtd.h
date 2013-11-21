/***************************************************************************//*! \file liprovider/sf_mtd.h
** <L5_PRIVATE L5_SOURCE>
** \author  OktetLabs
**  \brief  CIM Provider
**   \date  2013/10/02
**    \cop  (c) Solarflare Communications Inc.
** </L5_PRIVATE>
*//*
\**************************************************************************/

#ifndef SOLARFLARE_SF_MTD_H
#define SOLARFLARE_SF_MTD_H 1

#include "sf_utils.h"

namespace solarflare
{
    ///
    /// Get BootROM version via MTD for a Siena NIC.
    ///
    /// @param netif_name         Network interface name
    /// @param ver          [out] Where to save obtained version
    ///
    /// @return 0 on success, -1 on failure
    ///
    int mtdGetBootROMVersionSiena(const char *netif_name, VersionInfo &ver);

    ///
    /// Get BootROM version via MTD for an EF10 NIC.
    ///
    /// @param netif_name         Network interface name
    /// @param ver          [out] Where to save obtained version
    ///
    /// @return 0 on success, -1 on failure
    ///
    int mtdGetBootROMVersionEF10(const char *netif_name, VersionInfo &ver);
}

#endif // SOLARFLARE_SF_MTD_H
