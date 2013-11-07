/***************************************************************************//*! \file liprovider/sf_siocefx_nvram.h
** <L5_PRIVATE L5_SOURCE>
** \author  OktetLabs
**  \brief  CIM Provider
**   \date  2013/10/02
**    \cop  (c) Solarflare Communications Inc.
** </L5_PRIVATE>
*//*
\**************************************************************************/

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
    /// @param type      NVRAM partition type
    ///
    /// @return 0 on success or error code
    ///
    int siocEFXReadNVRAM(int fd, bool isSock,
                         uint8_t *data, unsigned int offset,
                         unsigned int len,
                         const char *ifname,
                         uint32_t type);

    ///
    /// Get size of NVRAM partition from NIC with help of
    /// SIOCEFX private ioctl.
    ///
    /// @param fd                   File descriptor to be used for ioctl()
    /// @param isSock               Whether file descriptor is socket or not
    /// @param ifname               Interface name
    /// @param type                 Partition type
    /// @param partitionSize  [out] Partition size
    ///
    /// @return 0 on success or error code
    ///
    int siocEFXGetNVRAMPartitionSize(int fd, bool isSock,
                                     const char *ifname,
                                     uint32_t type,
                                     uint32_t &partitionSize);

    ///
    /// Read NVRAM partition from NIC with help of
    /// SIOCEFX private ioctl.
    ///
    /// @param fd                   File descriptor to be used for ioctl()
    /// @param isSock               Whether file descriptor is socket or not
    /// @param ifname               Interface name
    /// @param type                 Partition type
    /// @param data           [out] Where to save data read from partition
    /// @param dataLen        [out] Where to save number of bytes read
    ///
    /// @return 0 on success or error code
    ///
    int siocEFXReadNVRAMPartition(int fd, bool isSock,
                                  const char *ifname,
                                  uint32_t type,
                                  uint8_t *&data,
                                  uint32_t &dataLen);

    ///
    /// Get BootROM version from NVRAM via SIOCEFX for Siena NIC.
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
    int siocEFXGetBootROMVersionSiena(const char *ifname,
                                      int port_index,
                                      int fd, bool isSock,
                                      VersionInfo &ver);

    ///
    /// Get BootROM version from NVRAM via SIOCEFX for EF10 NIC.
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
    int siocEFXGetBootROMVersionEF10(const char *ifname,
                                     int port_index,
                                     int fd, bool isSock,
                                     VersionInfo &ver);
}

#endif // SOLARFLARE_SF_SIOCEFX_NVRAM_H
