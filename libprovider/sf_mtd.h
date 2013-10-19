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
