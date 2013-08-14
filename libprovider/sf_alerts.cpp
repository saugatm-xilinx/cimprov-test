#include "sf_alerts.h"

namespace solarflare
{
    bool LinkStateAlertInfo::check(Array<AlertProps> &alerts)
    {
        bool         result = false;

        if (updateLinkState() != 0)
            return false;

        if (curLinkState != prevLinkState && !linkStateFirstTime)
        {
            AlertProps alert;

            alert.alertType = cimple::CIM_AlertIndication::
                                     _AlertType::enum_Device_Alert;
            alert.perceivedSeverity = cimple::CIM_AlertIndication::
                                     _PerceivedSeverity::enum_Major;
            if (curLinkState)
                alert.description = "Link went up";
            else
                alert.description = "Link went down";

            alerts.append(alert);
            result = true;
        }
        linkStateFirstTime = false;
        prevLinkState = curLinkState;
        return result;
    }

    bool SensorsAlertInfo::check(Array<AlertProps> &alerts)
    {
        unsigned int i;
        unsigned int j;

        bool result = false;

        if (updateSensors() != 0)
            return false;

        for (i = 0; i < sensorsCur.size(); i++)
        {
            // Do not report sensors of other port's Phy
            if (((sensorsCur[i].type == SENSOR_PHY0_COOLING ||
                  sensorsCur[i].type == SENSOR_PHY0_TEMP) &&
                 portFn == 1) ||
                ((sensorsCur[i].type == SENSOR_PHY1_COOLING ||
                  sensorsCur[i].type == SENSOR_PHY1_TEMP) &&
                 portFn == 0))
            {
                sensorsCur.remove(i);
                i--;
            }
        }
#if 0
        // For debug only
        Buffer bufCaption;

        bufCaption.format("Port %d sensors reading",
                          portFn);
        debugLogSensors(sensorsCur,
                        String(bufCaption.data()));
#endif

        if (!sensorsStateFirstTime)
        {
            AlertProps alert;

            alert.alertType = cimple::CIM_AlertIndication::
                                 _AlertType::enum_Device_Alert;
            alert.perceivedSeverity =
                              cimple::CIM_AlertIndication::
                                 _PerceivedSeverity::enum_Major;

            for (i = 0; i < sensorsCur.size(); i++)
            {
                Buffer     buffer;

                for (j = 0; j < sensorsPrev.size(); j++)
                    if (sensorsPrev[j].type == sensorsCur[i].type)
                        break;
                if (j == sensorsPrev.size())
                {
                    buffer.format(
                        "Sensor '%s' not seen previously "
                        "appeared in the state '%s'",
                        sensorType2Str(sensorsCur[i].type).c_str(),
                        sensorState2Str(sensorsCur[i].state).c_str());
                    alert.description = String(buffer.data());

                    alerts.append(alert);
                    result = true;
                }
                else if (sensorsCur[i].state != sensorsPrev[j].state)
                {
                    buffer.format(
                        "Sensor '%s' changed state from '%s' to '%s'",
                        sensorType2Str(sensorsCur[i].type).c_str(),
                        sensorState2Str(sensorsPrev[j].state).c_str(),
                        sensorState2Str(sensorsCur[i].state).c_str());

                    alert.description = String(buffer.data());
                    alerts.append(alert);
                    result = true;
                }
            }

            for (j = 0; j < sensorsPrev.size(); j++)
            {
                Buffer     buffer;
            
                for (i = 0; i < sensorsCur.size(); i++)
                    if (sensorsPrev[j].type == sensorsCur[i].type)
                        break;
                if (i == sensorsCur.size())
                {
                    buffer.format(
                        "Sensor '%s' disappeared",
                        sensorType2Str(sensorsPrev[j].type).c_str());
                    alert.description = String(buffer.data());

                    alerts.append(alert);
                    result = true;
                }
            }
        }

        sensorsStateFirstTime = false;
        sensorsPrev = sensorsCur;
        return result;
    }
}
