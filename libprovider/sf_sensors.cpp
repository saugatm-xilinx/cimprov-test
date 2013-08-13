#include "sf_sensors.h"

namespace solarflare
{
    struct value_name_enum {
      const char *name;
      unsigned value;
    };

#define LAST_ENUM {NULL, 0}

#define MAX_NAME_LEN 1024

    static String enum2Str(
                        const struct value_name_enum *values,
                        unsigned value)
    {
        int i;
        char buffer[MAX_NAME_LEN];

        for (i = 0; values[i].name != NULL; i++)
        {
            if (value == values[i].value)
              return String(values[i].name);
        }

        snprintf(buffer, MAX_NAME_LEN, "%d", value);
        return String(buffer);
    }

    static const struct value_name_enum sensor_states[] =
    {
      { "OK",       SENSOR_STATE_OK },
      { "Warn",     SENSOR_STATE_WARNING },
      { "Error",    SENSOR_STATE_FATAL },
      { "Broken",   SENSOR_STATE_BROKEN },
      { "No reading", SENSOR_STATE_NO_READING },
      LAST_ENUM
    };

    static const struct value_name_enum sensor_names[] =
    {
      { "Controller temp.",         SENSOR_CONTROLLER_TEMP },
      { "Phy temp.",                SENSOR_PHY_COMMON_TEMP },
      { "Controller cooling",       SENSOR_CONTROLLER_COOLING },
      { "Phy0 temp.",               SENSOR_PHY0_TEMP },
      { "Phy0 cooling",             SENSOR_PHY0_COOLING },
      { "Phy1 temp.",               SENSOR_PHY1_TEMP },
      { "Phy1 cooling",             SENSOR_PHY1_COOLING },
      { "1.0V supply",              SENSOR_IN_1V0 },
      { "1.2V supply",              SENSOR_IN_1V2 },
      { "1.8V supply",              SENSOR_IN_1V8 },
      { "2.5V supply",              SENSOR_IN_2V5 },
      { "3.3V supply",              SENSOR_IN_3V3 },
      { "12V supply",               SENSOR_IN_12V0 },
      { "1.2VA supply",             SENSOR_IN_1V2A },
      { "Vref supply",              SENSOR_IN_VREF },
      { "AOE FPGA supply",          SENSOR_OUT_VAOE },
      { "AOE FPGA temp.",           SENSOR_AOE_TEMP },
      { "AOE FPGA PSU temp.",       SENSOR_PSU_AOE_TEMP },
      { "AOE PSU temp.",            SENSOR_PSU_TEMP },
      { "Fan 0",                    SENSOR_FAN_0 },
      { "Fan 1",                    SENSOR_FAN_1 },
      { "Fan 2",                    SENSOR_FAN_2 },
      { "Fan 3",                    SENSOR_FAN_3 },
      { "Fan 4",                    SENSOR_FAN_4 },
      { "AOE supply",               SENSOR_IN_VAOE },
      { "AOE Iout",                 SENSOR_OUT_IAOE },
      { "AOE Iin",                  SENSOR_IN_IAOE },
      { "NIC power",                SENSOR_NIC_POWER },
      { "0.9V supply",              SENSOR_IN_0V9 },
      { "0.9V supply current",      SENSOR_IN_I0V9 },
      { "1.2V supply current",      SENSOR_IN_I1V2 },
      { "0.9V supply (at ADC)",     SENSOR_IN_0V9_ADC },
      { "Controller temp 2",        SENSOR_CONTROLLER_2_TEMP },
      { "Regulator internal temp",  SENSOR_VREG_INTERNAL_TEMP },
      { "0.9V regulator temp",      SENSOR_VREG_0V9_TEMP },
      { "1.2V regulator temp",      SENSOR_VREG_1V2_TEMP },
      { "On-chip temp.",            SENSOR_CONTROLLER_INTERNAL_TEMP },
      { "Ambient temp.",            SENSOR_AMBIENT_TEMP },
      { "Airflow",                  SENSOR_AIRFLOW },
      LAST_ENUM
    };

    String sensorType2Str(SensorType type)
    {
        return enum2Str(sensor_names, type);
    }

    String sensorState2Str(SensorState state)
    {
        return enum2Str(sensor_states, state);
    }
}
