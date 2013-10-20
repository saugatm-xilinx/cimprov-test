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
    /// Get BootROM version via MTD.
    ///
    /// @param netif_name         Network interface name
    /// @param ver          [out] Where to save obtained version
    ///
    /// @return 0 on success, -1 on failure
    ///
    int mtdGetBootROMVersion(const char *netif_name, VersionInfo &ver);
}

#endif // SOLARFLARE_SF_MTD_H
