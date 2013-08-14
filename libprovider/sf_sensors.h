#ifndef SOLARFLARE_SF_SENSORS_H
#define SOLARFLARE_SF_SENSORS_H 1

#include <cimple/Strings.h>
#include <cimple/Array.h>

namespace solarflare
{
    using cimple::String;
    using cimple::Array;

    typedef enum {
        SENSOR_STATE_OK,
        SENSOR_STATE_WARNING,
        SENSOR_STATE_FATAL,
        SENSOR_STATE_BROKEN,
        SENSOR_STATE_NO_READING,
        SENSOR_STATE_UNKNOWN,
    } SensorState;

    typedef enum {
        SENSOR_CONTROLLER_TEMP,
        SENSOR_PHY_COMMON_TEMP,
        SENSOR_CONTROLLER_COOLING,
        SENSOR_PHY_COOLING,
        SENSOR_PHY0_TEMP,
        SENSOR_PHY0_COOLING,
        SENSOR_PHY1_TEMP,
        SENSOR_PHY1_COOLING,
        SENSOR_IN_1V0,
        SENSOR_IN_1V2,
        SENSOR_IN_1V8,
        SENSOR_IN_2V5,
        SENSOR_IN_3V3,
        SENSOR_IN_12V0,
        SENSOR_IN_1V2A,
        SENSOR_IN_VREF,
        SENSOR_OUT_VAOE,
        SENSOR_AOE_TEMP,
        SENSOR_PSU_AOE_TEMP,
        SENSOR_PSU_TEMP,
        SENSOR_FAN_0,
        SENSOR_FAN_1,
        SENSOR_FAN_2,
        SENSOR_FAN_3,
        SENSOR_FAN_4,
        SENSOR_IN_VAOE,
        SENSOR_OUT_IAOE,
        SENSOR_IN_IAOE,
        SENSOR_NIC_POWER,
        SENSOR_IN_0V9,
        SENSOR_IN_I0V9,
        SENSOR_IN_I1V2,
        SENSOR_IN_0V9_ADC,
        SENSOR_CONTROLLER_2_TEMP,
        SENSOR_VREG_INTERNAL_TEMP,
        SENSOR_VREG_0V9_TEMP,
        SENSOR_VREG_1V2_TEMP,
        SENSOR_CONTROLLER_INTERNAL_TEMP,
        SENSOR_AMBIENT_TEMP,
        SENSOR_AIRFLOW,
        SENSOR_UNKNOWN,
    } SensorType;

    String sensorType2Str(SensorType type);

    String sensorState2Str(SensorState state);

    class Sensor
    {
    public:
        SensorType type;
        SensorState state;

        unsigned int limit1_low;
        unsigned int limit1_high;
        unsigned int limit2_low;
        unsigned int limit2_high;

        unsigned int value;

        Sensor() : type(SENSOR_UNKNOWN), state(SENSOR_STATE_UNKNOWN),
                   limit1_low(0), limit1_high(0), limit2_low(0),
                   limit2_high(0), value(0) {};

        bool operator== (const Sensor &rhs)
        {
            if (type == rhs.type &&
                state == state &&
                limit1_low == rhs.limit1_low &&
                limit1_high == rhs.limit1_high &&
                limit2_low == rhs.limit2_low &&
                limit2_high == rhs.limit2_high &&
                value == rhs.value)
                return true;

            return false;
        }
    };

    void debugLogSensors(Array<Sensor> &sensors);
}

#endif // SOLARFLARE_SF_SENSORS_H
