#ifndef SOLARFLARE_SF_SIOCEFX_NVRAM_H
#define SOLARFLARE_SF_SIOCEFX_NVRAM_H 1

namespace solarflare 
{
    ///
    /// Read NVRAM data from NIC with help of SIOCEFX private ioctl
    ///
    /// @param fd        File descriptor to be used for ioctl()
    /// @param isSock    Whether file descriptor is socket or not
    /// @param data      Where to save obtained data
    /// @param offset    Offset of data to be read
    /// @param len       Length of data to be read
    /// @param ifname    Interface name
    /// @param type      Determines what part of VPD should be read
    ///
    /// @return 0 on success or error code
    ///
    int siocEFXReadNVRAM(int fd, bool isSock,
                         uint8_t *data, unsigned int offset,
                         unsigned int len,
                         const char *ifname,
                         uint32_t type);

    ///
    /// Get BootROM version from NVRAM via SIOCEFX.
    ///
    /// @param ifname         Interface name
    /// @param port_index     Port index
    /// @param fd             File descriptor to be used when
    ///                       calling ioctl()
    /// @param isSock         Whether file descriptor is a socket or not
    /// @param ver      [out] Where to save BootROM version
    ///
    /// @return 0 on success, -1 on failure
    ///
    int siocEFXGetBootROMVersion(const char *ifname,
                                 int port_index,
                                 int fd, bool isSock,
                                 VersionInfo &ver);
}

#endif // SOLARFLARE_SF_SIOCEFX_NVRAM_H
