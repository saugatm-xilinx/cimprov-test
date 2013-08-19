#ifndef SOLARFLARE_SF_SENSORS_H
#define SOLARFLARE_SF_SENSORS_H 1

#include <cimple/Strings.h>
#include <cimple/Array.h>

namespace solarflare
{
    using cimple::String;
    using cimple::Array;

    /// Sensor states
    typedef enum {
        SENSOR_STATE_OK,
        SENSOR_STATE_WARNING,
        SENSOR_STATE_FATAL,
        SENSOR_STATE_BROKEN,
        SENSOR_STATE_NO_READING,
        SENSOR_STATE_UNKNOWN,
    } SensorState;

    /// Sensor types
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

    /// Get string representation of sensor type.
    ///
    /// @param type   Sensor type
    ///
    /// @return String representation
    ///
    String sensorType2Str(SensorType type);

    /// Get string representation of sensor type
    /// to be used in IndicationIdentifier.
    ///
    /// @param type   Sensor type
    ///
    /// @return String representation
    ///
    String sensorType2StrId(SensorType type);

    ///
    /// Get string representation of sensor state.
    ///
    /// @param state    Sensor state
    ///
    /// @return String representation
    ///
    String sensorState2Str(SensorState state);

    ///
    /// Get string representation of sensor state
    /// to be used in IndicationIdentifier.
    ///
    /// @param state    Sensor state
    ///
    /// @return String representation
    ///
    String sensorState2StrId(SensorState state);

    /// Sensor readings class
    class Sensor
    {
    public:
        SensorType type;            ///< Sensor type
        SensorState state;          ///< Sensor state

        /// Usually value exceeding the first bound results
        /// in warning, and value exceeding the second
        /// bound means error.
        unsigned int limit1_low;    ///< First lower bound
        unsigned int limit1_high;   ///< First upper bound
        unsigned int limit2_low;    ///< Second lower bound
        unsigned int limit2_high;   ///< Second lower bound

        unsigned int value;         ///< Sensor value

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

    ///
    /// Print sensors reading to CIMPLE log for debug.
    ///
    /// @param sensors    Sensors readings
    /// @param caption    Readings table caption
    ///
    void debugLogSensors(Array<Sensor> &sensors,
                         const String &caption = "");
}

#endif // SOLARFLARE_SF_SENSORS_H
