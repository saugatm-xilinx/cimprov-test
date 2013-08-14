#ifndef SOLARFLARE_SF_MCDI_SENSORS_H
#define SOLARFLARE_SF_MCDI_SENSORS_H 1

#include <cimple/Strings.h>
#include <cimple/Array.h>

#include "sf_sensors.h"

namespace solarflare
{
    using cimple::String;
    using cimple::Array;

    int getSensors(Array<Sensor> &sensors,
                   int fd,
                   bool isSocket,
                   String ifName);
}

#endif // SOLARFLARE_SF_MCDI_SENSORS_H
