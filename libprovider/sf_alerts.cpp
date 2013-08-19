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
            if (curLinkState)
            {
                alert.perceivedSeverity =
                  cimple::CIM_AlertIndication::
                             _PerceivedSeverity::enum_Information;
                alert.description = "Link went up";
                alert.localId = "LinkUp";
                alert.messageId = "SOLARFLARE0001";
                alert.message = "Link went up";
            }
            else
            {
                alert.perceivedSeverity =
                  cimple::CIM_AlertIndication::
                               _PerceivedSeverity::enum_Degraded_Warning;
                alert.description = "Link went down";
                alert.localId = "LinkDown";
                alert.messageId = "SOLARFLARE0002";
                alert.message = "Link went down";
            }

            alert.instPath = instPath;
            alert.sysName = sysName;
            alert.sysCreationClassName = sysCreationClassName;

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

            alert.instPath = instPath;
            alert.sysName = sysName;
            alert.sysCreationClassName = sysCreationClassName;

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

                    alert.localId = sensorType2StrId(sensorsCur[i].type);
                    alert.localId.append("_APPEARED_");
                    alert.localId.append(
                                sensorState2StrId(sensorsCur[i].state));

                    alert.messageArguments.clear();
                    if (sensorsCur[i].state == SENSOR_STATE_OK)
                    {
                        alert.perceivedSeverity =
                                cimple::CIM_AlertIndication::
                                    _PerceivedSeverity::enum_Information;
                        alert.messageId = String("SOLARFLARE0003");
                        alert.messageArguments.append(
                                      sensorType2Str(sensorsCur[i].type));
                    }
                    else
                    {
                        alert.perceivedSeverity =
                            cimple::CIM_AlertIndication::
                               _PerceivedSeverity::enum_Degraded_Warning;
                        alert.messageId = String("SOLARFLARE0004");
                        alert.messageArguments.append(
                                      sensorType2Str(sensorsCur[i].type));
                        alert.messageArguments.append(
                                     sensorState2Str(sensorsCur[i].state));
                    }

                    alert.message = alert.description;

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

                    alert.localId = sensorType2StrId(sensorsCur[i].type);
                    alert.localId.append("_");
                    alert.localId.append(
                                sensorState2StrId(sensorsPrev[j].state));
                    alert.localId.append("2");
                    alert.localId.append(
                                sensorState2StrId(sensorsCur[i].state));

                    alert.message = alert.description;
                    alert.messageArguments.clear();
                    alert.messageArguments.append(
                                      sensorType2Str(sensorsCur[i].type));
                    alert.messageArguments.append(
                                    sensorState2Str(sensorsPrev[j].state));
                    switch(sensorsCur[i].state)
                    {
                        case SENSOR_STATE_OK:
                            alert.messageId = String("SOLARFLARE0006");
                            alert.perceivedSeverity =
                              cimple::CIM_AlertIndication::
                                 _PerceivedSeverity::enum_Information;
                            break;

                        case SENSOR_STATE_WARNING:
                            alert.messageId = String("SOLARFLARE0007");
                            alert.perceivedSeverity =
                              cimple::CIM_AlertIndication::
                                 _PerceivedSeverity::enum_Degraded_Warning;
                            break;

                        case SENSOR_STATE_NO_READING:
                            alert.messageId = String("SOLARFLARE0008");
                            alert.perceivedSeverity =
                              cimple::CIM_AlertIndication::
                                 _PerceivedSeverity::enum_Degraded_Warning;
                            break;

                        case SENSOR_STATE_UNKNOWN:
                            alert.messageId = String("SOLARFLARE0009");
                            alert.perceivedSeverity =
                              cimple::CIM_AlertIndication::
                                 _PerceivedSeverity::enum_Degraded_Warning;
                            break;

                        case SENSOR_STATE_FATAL:
                            alert.messageId = String("SOLARFLARE0010");
                            alert.perceivedSeverity =
                              cimple::CIM_AlertIndication::
                                _PerceivedSeverity::
                                          enum_Fatal_NonRecoverable;
                            break;

                        case SENSOR_STATE_BROKEN:
                            alert.messageId = String("SOLARFLARE0011");
                            alert.perceivedSeverity =
                              cimple::CIM_AlertIndication::
                                _PerceivedSeverity::
                                          enum_Fatal_NonRecoverable;
                            break;
                    }

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

                    alert.perceivedSeverity =
                        cimple::CIM_AlertIndication::
                           _PerceivedSeverity::enum_Degraded_Warning;

                    alert.localId = sensorType2StrId(sensorsPrev[j].type);
                    alert.localId.append("_DISAPPEARED");

                    alert.messageId = String("SOLARFLARE0005");
                    alert.message = alert.description;
                    alert.messageArguments.clear();
                    alert.messageArguments.append(
                                      sensorType2Str(sensorsPrev[j].type));

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
