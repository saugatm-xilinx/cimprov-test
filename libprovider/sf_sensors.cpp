/***************************************************************************//*! \file liprovider/sf_sensors.cpp
** <L5_PRIVATE L5_SOURCE>
** \author  OktetLabs
**  \brief  CIM Provider
**   \date  2013/10/02
**    \cop  (c) Solarflare Communications Inc.
** </L5_PRIVATE>
*//*
\**************************************************************************/

#include "sf_sensors.h"
#include "sf_logging.h"

#include <cimple/log.h>

namespace solarflare
{
    /// Structure for storing correspondence between
    /// an enumeration value and its string representation
    struct value_name_enum {
      const char *name;
      unsigned value;
    };

#define LAST_ENUM {NULL, 0}

#define MAX_NAME_LEN 1024

    ///
    /// Get string representation of some enumeration value.
    ///
    /// @param values   Pointer to array storing correspondence
    ///                 between enum values and their string
    ///                 representations
    ///
    /// @param value    Enumeration value
    ///
    /// @return String representation
    ///
    static String enum2Str(
                        const struct value_name_enum *values,
                        unsigned int value)
    {
        int i;
        char buffer[MAX_NAME_LEN];

        for (i = 0; values[i].name != NULL; i++)
        {
            if (value == values[i].value)
              return String(values[i].name);
        }

        snprintf(buffer, MAX_NAME_LEN, "%u", value);
        return String(buffer);
    }

    ///
    /// Get some enumeration value from its string representation.
    ///
    /// @param values    Pointer to array storing correspondence
    ///                  between enum values and their string
    ///                  representations
    ///
    /// @param str       String representation
    /// @param val [out] Enumeration value
    ///
    /// @return 0 on success, -1 on failure
    ///
    static int str2Enum(const struct value_name_enum *values,
                        const char *str,
                        unsigned int &val)
    {
        int i;

        for (i = 0; values[i].name != NULL; i++)
        {
            if (strcmp(values[i].name, str) == 0)
            {
                val = values[i].value;
                return 0;
            }
        }

        return -1;
    }

    /// String representations of sensor states
    static const struct value_name_enum sensor_states[] =
    {
      { "OK",       SENSOR_STATE_OK },
      { "Warn",     SENSOR_STATE_WARNING },
      { "Error",    SENSOR_STATE_FATAL },
      { "Broken",   SENSOR_STATE_BROKEN },
      { "No reading", SENSOR_STATE_NO_READING },
      LAST_ENUM
    };

    /// String representations of sensor states
    /// to be used in IndicationIdentifier
    static const struct value_name_enum sensor_states_ids[] =
    {
      { "OK",         SENSOR_STATE_OK },
      { "WARNING",    SENSOR_STATE_WARNING },
      { "ERROR",      SENSOR_STATE_FATAL },
      { "BROKEN",     SENSOR_STATE_BROKEN },
      { "NO_READING", SENSOR_STATE_NO_READING },
      LAST_ENUM
    };

    /// String representations of sensor types
    static const struct value_name_enum sensor_types[] =
    {
        { "Controller temperature: degC",          SENSOR_CONTROLLER_TEMP },
        { "Phy common temperature: degC",          SENSOR_PHY_COMMON_TEMP },
        { "Controller cooling: bool",
          SENSOR_CONTROLLER_COOLING },
        { "Phy 0 temperature: degC",               SENSOR_PHY0_TEMP },
        { "Phy 0 cooling: bool",                   SENSOR_PHY0_COOLING },
        { "Phy 1 temperature: degC",               SENSOR_PHY1_TEMP },
        { "Phy 1 cooling: bool",                   SENSOR_PHY1_COOLING },
        { "1.0v power: mV",                        SENSOR_IN_1V0 },
        { "1.2v power: mV",                        SENSOR_IN_1V2 },
        { "1.8v power: mV",                        SENSOR_IN_1V8 },
        { "2.5v power: mV",                        SENSOR_IN_2V5 },
        { "3.3v power: mV",                        SENSOR_IN_3V3 },
        { "12v power: mV",                         SENSOR_IN_12V0 },
        { "1.2v analogue power: mV",               SENSOR_IN_1V2A },
        { "reference voltage: mV",                 SENSOR_IN_VREF },
        { "AOE FPGA power: mV",                    SENSOR_OUT_VAOE },
        { "AOE FPGA temperature: degC",            SENSOR_AOE_TEMP },
        { "AOE FPGA PSU temperature: degC",        SENSOR_PSU_AOE_TEMP },
        { "AOE PSU temperature: degC",             SENSOR_PSU_TEMP },
        { "Fan 0 speed: RPM",                      SENSOR_FAN_0 },
        { "Fan 1 speed: RPM",                      SENSOR_FAN_1 },
        { "Fan 2 speed: RPM",                      SENSOR_FAN_2 },
        { "Fan 3 speed: RPM",                      SENSOR_FAN_3 },
        { "Fan 4 speed: RPM",                      SENSOR_FAN_4 },
        { "AOE FPGA input power: mV",              SENSOR_IN_VAOE },
        { "AOE FPGA current: mA",                  SENSOR_OUT_IAOE },
        { "AOE FPGA input current: mA",            SENSOR_IN_IAOE },
        { "NIC power consumption: W",              SENSOR_NIC_POWER },
        { "0.9v power voltage: mV",                SENSOR_IN_0V9 },
        { "0.9v power current: mA",                SENSOR_IN_I0V9 },
        { "1.2v power current: mA",                SENSOR_IN_I1V2 },
        { "0.9v power voltage (at ADC): mV",       SENSOR_IN_0V9_ADC },
        { "Controller temperature 2: degC",
          SENSOR_CONTROLLER_2_TEMP },
        { "Voltage regulator internal temperature: degC",
          SENSOR_VREG_INTERNAL_TEMP },
        { "0.9V voltage regulator temperature: degC",
          SENSOR_VREG_0V9_TEMP },
        { "1.2V voltage regulator temperature: degC",
          SENSOR_VREG_1V2_TEMP },
        { "controller internal temperature sensor voltage "
          "(internal ADC): mV",
          SENSOR_CONTROLLER_VPTAT },
        { "controller internal temperature (internal ADC): degC",
          SENSOR_CONTROLLER_INTERNAL_TEMP },
        { "controller internal temperature sensor voltage "
          "(external ADC): mV",
          SENSOR_CONTROLLER_VPTAT_EXTADC },
        { "controller internal temperature (external ADC): degC",
          SENSOR_CONTROLLER_INTERNAL_TEMP_EXTADC },
        { "ambient temperature: degC",             SENSOR_AMBIENT_TEMP },
        { "air flow: bool",                        SENSOR_AIRFLOW },
        { "voltage between VSS08D and VSS08D at CSR: mV",
          SENSOR_VDD08D_VSS08D_CSR },
        { "voltage between VSS08D and VSS08D at CSR (external ADC): mV",
          SENSOR_VDD08D_VSS08D_CSR_EXTADC },
        { "Hotpoint temperature: degC",            SENSOR_HOTPOINT_TEMP },
        { "Port 0 PHY power switch over-current: bool",
          SENSOR_PHY_POWER_PORT0 },
        { "Port 1 PHY power switch over-current: bool",
          SENSOR_PHY_POWER_PORT1 },
        { "Mop-up microcontroller reference voltage (millivolts)",
          SENSOR_MUM_VCC },
        { "0.9v power phase A voltage: mV",        SENSOR_IN_0V9_A },
        { "0.9v power phase A current: mA",        SENSOR_IN_I0V9_A },
        { "0.9V voltage regulator phase A temperature: degC",
          SENSOR_VREG_0V9_A_TEMP },
        { "0.9v power phase B voltage: mV",        SENSOR_IN_0V9_B },
        { "0.9v power phase B current: mA",        SENSOR_IN_I0V9_B },
        { "0.9V voltage regulator phase B temperature: degC",
          SENSOR_VREG_0V9_B_TEMP },
        { "CCOM AVREG 1v2 supply (interval ADC): mV",
          SENSOR_CCOM_AVREG_1V2_SUPPLY },
        { "CCOM AVREG 1v2 supply (external ADC): mV",
          SENSOR_CCOM_AVREG_1V2_SUPPLY_EXTADC },
        { "CCOM AVREG 1v8 supply (interval ADC): mV",
          SENSOR_CCOM_AVREG_1V8_SUPPLY },
        { "CCOM AVREG 1v8 supply (external ADC): mV",
          SENSOR_CCOM_AVREG_1V8_SUPPLY_EXTADC },
        { "CCOM RTS temperature: degC",            SENSOR_CONTROLLER_RTS },
        { "controller internal temperature sensor voltage on master core "
          "(internal ADC): mV",
          SENSOR_CONTROLLER_MASTER_VPTAT },
        { "controller internal temperature on master core "
          "(internal ADC): degC",
          SENSOR_CONTROLLER_MASTER_INTERNAL_TEMP },
        { "controller internal temperature sensor voltage on master core "
          "(external ADC): mV",
          SENSOR_CONTROLLER_MASTER_VPTAT_EXTADC },
        { "controller internal temperature on master core "
          "(external ADC): degC",
          SENSOR_CONTROLLER_MASTER_INTERNAL_TEMP_EXTADC },
        { "controller internal temperature on slave core sensor "
          "voltage (internal ADC): mV",
          SENSOR_CONTROLLER_SLAVE_VPTAT },
        { "controller internal temperature on slave core "
          "(internal ADC): degC",
          SENSOR_CONTROLLER_SLAVE_INTERNAL_TEMP },
        { "controller internal temperature on slave core sensor voltage "
          "(external ADC): mV",
          SENSOR_CONTROLLER_SLAVE_VPTAT_EXTADC },
        { "controller internal temperature on slave core "
          "(external ADC): degC",
          SENSOR_CONTROLLER_SLAVE_INTERNAL_TEMP_EXTADC },
        { "Voltage supplied to the SODIMMs from their power supply: mV",
          SENSOR_SODIMM_VOUT },
        { "Temperature of SODIMM 0 (if installed): degC",
          SENSOR_SODIMM_0_TEMP },
        { "Temperature of SODIMM 1 (if installed): degC",
          SENSOR_SODIMM_1_TEMP },
        { "Voltage supplied to the QSFP #0 from their power supply: mV",
          SENSOR_PHY0_VCC },
        { "Voltage supplied to the QSFP #1 from their power supply: mV",
          SENSOR_PHY1_VCC },
        LAST_ENUM
    };

#define SENSOR_TYPE_ID(type_) \
    { #type_, type_ }

    /// String representations of sensor types
    /// to be used in IndicationIdentifier
    static const struct value_name_enum sensor_types_ids[] =
    {
        SENSOR_TYPE_ID(SENSOR_CONTROLLER_TEMP),
        SENSOR_TYPE_ID(SENSOR_PHY_COMMON_TEMP),
        SENSOR_TYPE_ID(SENSOR_CONTROLLER_COOLING),
        SENSOR_TYPE_ID(SENSOR_PHY0_TEMP),
        SENSOR_TYPE_ID(SENSOR_PHY0_COOLING),
        SENSOR_TYPE_ID(SENSOR_PHY1_TEMP),
        SENSOR_TYPE_ID(SENSOR_PHY1_COOLING),
        SENSOR_TYPE_ID(SENSOR_IN_1V0),
        SENSOR_TYPE_ID(SENSOR_IN_1V2),
        SENSOR_TYPE_ID(SENSOR_IN_1V8),
        SENSOR_TYPE_ID(SENSOR_IN_2V5),
        SENSOR_TYPE_ID(SENSOR_IN_3V3),
        SENSOR_TYPE_ID(SENSOR_IN_12V0),
        SENSOR_TYPE_ID(SENSOR_IN_1V2A),
        SENSOR_TYPE_ID(SENSOR_IN_VREF),
        SENSOR_TYPE_ID(SENSOR_OUT_VAOE),
        SENSOR_TYPE_ID(SENSOR_AOE_TEMP),
        SENSOR_TYPE_ID(SENSOR_PSU_AOE_TEMP),
        SENSOR_TYPE_ID(SENSOR_PSU_TEMP),
        SENSOR_TYPE_ID(SENSOR_FAN_0),
        SENSOR_TYPE_ID(SENSOR_FAN_1),
        SENSOR_TYPE_ID(SENSOR_FAN_2),
        SENSOR_TYPE_ID(SENSOR_FAN_3),
        SENSOR_TYPE_ID(SENSOR_FAN_4),
        SENSOR_TYPE_ID(SENSOR_IN_VAOE),
        SENSOR_TYPE_ID(SENSOR_OUT_IAOE),
        SENSOR_TYPE_ID(SENSOR_IN_IAOE),
        SENSOR_TYPE_ID(SENSOR_NIC_POWER),
        SENSOR_TYPE_ID(SENSOR_IN_0V9),
        SENSOR_TYPE_ID(SENSOR_IN_I0V9),
        SENSOR_TYPE_ID(SENSOR_IN_I1V2),
        SENSOR_TYPE_ID(SENSOR_IN_0V9_ADC),
        SENSOR_TYPE_ID(SENSOR_CONTROLLER_2_TEMP),
        SENSOR_TYPE_ID(SENSOR_VREG_INTERNAL_TEMP),
        SENSOR_TYPE_ID(SENSOR_VREG_0V9_TEMP),
        SENSOR_TYPE_ID(SENSOR_VREG_1V2_TEMP),
        SENSOR_TYPE_ID(SENSOR_CONTROLLER_VPTAT),
        SENSOR_TYPE_ID(SENSOR_CONTROLLER_INTERNAL_TEMP),
        SENSOR_TYPE_ID(SENSOR_CONTROLLER_VPTAT_EXTADC),
        SENSOR_TYPE_ID(SENSOR_CONTROLLER_INTERNAL_TEMP_EXTADC),
        SENSOR_TYPE_ID(SENSOR_AMBIENT_TEMP),
        SENSOR_TYPE_ID(SENSOR_AIRFLOW),
        SENSOR_TYPE_ID(SENSOR_VDD08D_VSS08D_CSR),
        SENSOR_TYPE_ID(SENSOR_VDD08D_VSS08D_CSR_EXTADC),
        SENSOR_TYPE_ID(SENSOR_HOTPOINT_TEMP),
        SENSOR_TYPE_ID(SENSOR_PHY_POWER_PORT0),
        SENSOR_TYPE_ID(SENSOR_PHY_POWER_PORT1),
        SENSOR_TYPE_ID(SENSOR_MUM_VCC),
        SENSOR_TYPE_ID(SENSOR_IN_0V9_A),
        SENSOR_TYPE_ID(SENSOR_IN_I0V9_A),
        SENSOR_TYPE_ID(SENSOR_VREG_0V9_A_TEMP),
        SENSOR_TYPE_ID(SENSOR_IN_0V9_B),
        SENSOR_TYPE_ID(SENSOR_IN_I0V9_B),
        SENSOR_TYPE_ID(SENSOR_VREG_0V9_B_TEMP),
        SENSOR_TYPE_ID(SENSOR_CCOM_AVREG_1V2_SUPPLY),
        SENSOR_TYPE_ID(SENSOR_CCOM_AVREG_1V2_SUPPLY_EXTADC),
        SENSOR_TYPE_ID(SENSOR_CCOM_AVREG_1V8_SUPPLY),
        SENSOR_TYPE_ID(SENSOR_CCOM_AVREG_1V8_SUPPLY_EXTADC),
        SENSOR_TYPE_ID(SENSOR_CONTROLLER_RTS),
        SENSOR_TYPE_ID(SENSOR_CONTROLLER_MASTER_VPTAT),
        SENSOR_TYPE_ID(SENSOR_CONTROLLER_MASTER_INTERNAL_TEMP),
        SENSOR_TYPE_ID(SENSOR_CONTROLLER_MASTER_VPTAT_EXTADC),
        SENSOR_TYPE_ID(SENSOR_CONTROLLER_MASTER_INTERNAL_TEMP_EXTADC),
        SENSOR_TYPE_ID(SENSOR_CONTROLLER_SLAVE_VPTAT),
        SENSOR_TYPE_ID(SENSOR_CONTROLLER_SLAVE_INTERNAL_TEMP),
        SENSOR_TYPE_ID(SENSOR_CONTROLLER_SLAVE_VPTAT_EXTADC),
        SENSOR_TYPE_ID(SENSOR_CONTROLLER_SLAVE_INTERNAL_TEMP_EXTADC),
        SENSOR_TYPE_ID(SENSOR_SODIMM_VOUT),
        SENSOR_TYPE_ID(SENSOR_SODIMM_0_TEMP),
        SENSOR_TYPE_ID(SENSOR_SODIMM_1_TEMP),
        SENSOR_TYPE_ID(SENSOR_PHY0_VCC),
        SENSOR_TYPE_ID(SENSOR_PHY1_VCC),
        SENSOR_TYPE_ID(SENSOR_UNKNOWN),
        LAST_ENUM
    };

    /// Desribed in sf_sensors.h
    String sensorType2Str(SensorType type)
    {
        return enum2Str(sensor_types, type);
    }

    /// Desribed in sf_sensors.h
    String sensorState2Str(SensorState state)
    {
        return enum2Str(sensor_states, state);
    }

    /// Desribed in sf_sensors.h
    String sensorState2StrId(SensorState state)
    {
        return enum2Str(sensor_states_ids, state);
    }

    /// Desribed in sf_sensors.h
    int sensorStrId2State(const char *strState, SensorState &state)
    {
        unsigned int val = 0;
        int          rc;

        rc = str2Enum(sensor_states_ids, strState, val);
        if (rc >= 0)
            state = static_cast<SensorState>(val);
        return rc;
    }

    /// Desribed in sf_sensors.h
    String sensorType2StrId(SensorType type)
    {
        return enum2Str(sensor_types_ids, type);
    }

    /// Desribed in sf_sensors.h
    int sensorStrId2Type(const char *strType, SensorType &type)
    {
        unsigned int val = 0;
        int          rc;

        rc = str2Enum(sensor_types_ids, strType, val);
        if (rc >= 0)
            type = static_cast<SensorType>(val);
        return rc;
    }

    /// Desribed in sf_sensors.h
    void debugLogSensors(Array<Sensor> &sensors,
                         const String &caption)
    {
        unsigned int i;

        if (strlen(caption.c_str()) > 0)
            PROVIDER_LOG_DBG(caption.c_str());
        PROVIDER_LOG_DBG("    Sensor name             min1   max1   "
                         "min2   max2   value state");

        for (i = 0; i < sensors.size(); i++)
            PROVIDER_LOG_DBG("%27s  %5d  %5d  %5d  %5d  %5d  %s",
                             sensorType2Str(sensors[i].type).c_str(),
                             sensors[i].limit1_low,
                             sensors[i].limit1_high,
                             sensors[i].limit2_low,
                             sensors[i].limit2_high,
                             sensors[i].value,
                             sensorState2Str(sensors[i].state).c_str());
    }
}
