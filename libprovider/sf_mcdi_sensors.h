#ifndef SOLARFLARE_SF_MCDI_SENSORS_H
#define SOLARFLARE_SF_MCDI_SENSORS_H 1

#include <cimple/Strings.h>
#include <cimple/Array.h>

#include "sf_sensors.h"

namespace solarflare
{
    using cimple::String;
    using cimple::Array;

    ///
    /// Get sensors readings via MCDI interface.
    ///
    /// @param sensors    [out] Where to save readings
    /// @param fd               File descriptor to be used 
    ///                         in ioctl(SIOCEFX) call
    /// @param isSocket         Is specified file descriptor
    ///                         a socket file descriptor?
    /// @param ifName           Interface name
    ///
    /// @return 0 on Success, -1 on Failure
    ///
    int mcdiGetSensors(Array<Sensor> &sensors,
                       int fd,
                       bool isSocket,
                       String ifName);
}

#endif // SOLARFLARE_SF_MCDI_SENSORS_H
