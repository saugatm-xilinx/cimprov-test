/***************************************************************************//*! \file liprovider/sf_mcdi_sensors.cpp
** <L5_PRIVATE L5_SOURCE>
** \author  OktetLabs
**  \brief  CIM Provider
**   \date  2013/10/02
**    \cop  (c) Solarflare Communications Inc.
** </L5_PRIVATE>
*//*
\**************************************************************************/

#include <cstdio>
#include <cstdlib>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

extern "C" {
#include "efx_ioctl.h"
#include "endian_base.h"
#include "ci/mgmt/mc_driver_pcol.h"
}

#include "sf_siocefx_common.h"

#include <cimple/Strings.h>
#include <cimple/Array.h>
#include <cimple/log.h>

#include "sf_mcdi_sensors.h"
#include "sf_logging.h"

namespace solarflare 
{
    using cimple::String;
    using cimple::Array;

#define MCDI_SENSOR_TYPE_CHECK(_name) \
    case MC_CMD_ ## _name: return _name

    ///
    /// Get sensor type from MCDI id.
    ///
    /// @param type   MCDI sensor id
    ///
    /// @return Sensor type
    ///
    SensorType sensorTypeMCDI2Common(unsigned int type)
    {
        switch(type)
        {
            MCDI_SENSOR_TYPE_CHECK(SENSOR_CONTROLLER_TEMP);
            MCDI_SENSOR_TYPE_CHECK(SENSOR_PHY_COMMON_TEMP);
            MCDI_SENSOR_TYPE_CHECK(SENSOR_CONTROLLER_COOLING);
            MCDI_SENSOR_TYPE_CHECK(SENSOR_PHY0_TEMP);
            MCDI_SENSOR_TYPE_CHECK(SENSOR_PHY0_COOLING);
            MCDI_SENSOR_TYPE_CHECK(SENSOR_PHY1_TEMP);
            MCDI_SENSOR_TYPE_CHECK(SENSOR_PHY1_COOLING);
            MCDI_SENSOR_TYPE_CHECK(SENSOR_IN_1V0);
            MCDI_SENSOR_TYPE_CHECK(SENSOR_IN_1V2);
            MCDI_SENSOR_TYPE_CHECK(SENSOR_IN_1V8);
            MCDI_SENSOR_TYPE_CHECK(SENSOR_IN_2V5);
            MCDI_SENSOR_TYPE_CHECK(SENSOR_IN_3V3);
            MCDI_SENSOR_TYPE_CHECK(SENSOR_IN_12V0);
            MCDI_SENSOR_TYPE_CHECK(SENSOR_IN_1V2A);
            MCDI_SENSOR_TYPE_CHECK(SENSOR_IN_VREF);
            MCDI_SENSOR_TYPE_CHECK(SENSOR_OUT_VAOE);
            MCDI_SENSOR_TYPE_CHECK(SENSOR_AOE_TEMP);
            MCDI_SENSOR_TYPE_CHECK(SENSOR_PSU_AOE_TEMP);
            MCDI_SENSOR_TYPE_CHECK(SENSOR_PSU_TEMP);
            MCDI_SENSOR_TYPE_CHECK(SENSOR_FAN_0);
            MCDI_SENSOR_TYPE_CHECK(SENSOR_FAN_1);
            MCDI_SENSOR_TYPE_CHECK(SENSOR_FAN_2);
            MCDI_SENSOR_TYPE_CHECK(SENSOR_FAN_3);
            MCDI_SENSOR_TYPE_CHECK(SENSOR_FAN_4);
            MCDI_SENSOR_TYPE_CHECK(SENSOR_IN_VAOE);
            MCDI_SENSOR_TYPE_CHECK(SENSOR_OUT_IAOE);
            MCDI_SENSOR_TYPE_CHECK(SENSOR_IN_IAOE);
            MCDI_SENSOR_TYPE_CHECK(SENSOR_NIC_POWER);
            MCDI_SENSOR_TYPE_CHECK(SENSOR_IN_0V9);
            MCDI_SENSOR_TYPE_CHECK(SENSOR_IN_I0V9);
            MCDI_SENSOR_TYPE_CHECK(SENSOR_IN_I1V2);
            MCDI_SENSOR_TYPE_CHECK(SENSOR_IN_0V9_ADC);
            MCDI_SENSOR_TYPE_CHECK(SENSOR_CONTROLLER_2_TEMP);
            MCDI_SENSOR_TYPE_CHECK(SENSOR_VREG_INTERNAL_TEMP);
            MCDI_SENSOR_TYPE_CHECK(SENSOR_VREG_0V9_TEMP);
            MCDI_SENSOR_TYPE_CHECK(SENSOR_VREG_1V2_TEMP);
            MCDI_SENSOR_TYPE_CHECK(SENSOR_CONTROLLER_VPTAT);
            MCDI_SENSOR_TYPE_CHECK(SENSOR_CONTROLLER_INTERNAL_TEMP);
            MCDI_SENSOR_TYPE_CHECK(SENSOR_CONTROLLER_VPTAT_EXTADC);
            MCDI_SENSOR_TYPE_CHECK(SENSOR_CONTROLLER_INTERNAL_TEMP_EXTADC);
            MCDI_SENSOR_TYPE_CHECK(SENSOR_AMBIENT_TEMP);
            MCDI_SENSOR_TYPE_CHECK(SENSOR_AIRFLOW);
            MCDI_SENSOR_TYPE_CHECK(SENSOR_VDD08D_VSS08D_CSR);
            MCDI_SENSOR_TYPE_CHECK(SENSOR_VDD08D_VSS08D_CSR_EXTADC);
            MCDI_SENSOR_TYPE_CHECK(SENSOR_HOTPOINT_TEMP);
            MCDI_SENSOR_TYPE_CHECK(SENSOR_PHY_POWER_PORT0);
            MCDI_SENSOR_TYPE_CHECK(SENSOR_PHY_POWER_PORT1);
            MCDI_SENSOR_TYPE_CHECK(SENSOR_MUM_VCC);
            MCDI_SENSOR_TYPE_CHECK(SENSOR_IN_0V9_A);
            MCDI_SENSOR_TYPE_CHECK(SENSOR_IN_I0V9_A);
            MCDI_SENSOR_TYPE_CHECK(SENSOR_VREG_0V9_A_TEMP);
            MCDI_SENSOR_TYPE_CHECK(SENSOR_IN_0V9_B);
            MCDI_SENSOR_TYPE_CHECK(SENSOR_IN_I0V9_B);
            MCDI_SENSOR_TYPE_CHECK(SENSOR_VREG_0V9_B_TEMP);
            MCDI_SENSOR_TYPE_CHECK(SENSOR_CCOM_AVREG_1V2_SUPPLY);
            MCDI_SENSOR_TYPE_CHECK(SENSOR_CCOM_AVREG_1V2_SUPPLY_EXTADC);
            MCDI_SENSOR_TYPE_CHECK(SENSOR_CCOM_AVREG_1V8_SUPPLY);
            MCDI_SENSOR_TYPE_CHECK(SENSOR_CCOM_AVREG_1V8_SUPPLY_EXTADC);
            MCDI_SENSOR_TYPE_CHECK(SENSOR_CONTROLLER_RTS);
            MCDI_SENSOR_TYPE_CHECK(SENSOR_CONTROLLER_MASTER_VPTAT);
            MCDI_SENSOR_TYPE_CHECK(SENSOR_CONTROLLER_MASTER_INTERNAL_TEMP);
            MCDI_SENSOR_TYPE_CHECK(SENSOR_CONTROLLER_MASTER_VPTAT_EXTADC);
            MCDI_SENSOR_TYPE_CHECK(SENSOR_CONTROLLER_MASTER_INTERNAL_TEMP_EXTADC);
            MCDI_SENSOR_TYPE_CHECK(SENSOR_CONTROLLER_SLAVE_VPTAT);
            MCDI_SENSOR_TYPE_CHECK(SENSOR_CONTROLLER_SLAVE_INTERNAL_TEMP);
            MCDI_SENSOR_TYPE_CHECK(SENSOR_CONTROLLER_SLAVE_VPTAT_EXTADC);
            MCDI_SENSOR_TYPE_CHECK(SENSOR_CONTROLLER_SLAVE_INTERNAL_TEMP_EXTADC);
            MCDI_SENSOR_TYPE_CHECK(SENSOR_SODIMM_VOUT);
            MCDI_SENSOR_TYPE_CHECK(SENSOR_SODIMM_0_TEMP);
            MCDI_SENSOR_TYPE_CHECK(SENSOR_SODIMM_1_TEMP);
            MCDI_SENSOR_TYPE_CHECK(SENSOR_PHY0_VCC);
            MCDI_SENSOR_TYPE_CHECK(SENSOR_PHY1_VCC);

            default:
                return SENSOR_UNKNOWN;
        }
    }

#define MCDI_SENSOR_STATE_CHECK(_name) \
    case MC_CMD_ ## _name: return _name

    ///
    /// Get sensor state from MCDI id
    ///
    /// @param state  MCDI state id
    ///
    /// @return Sensor state
    ///
    SensorState sensorStateMCDI2Common(unsigned int state)
    {
        switch(state)
        {
            MCDI_SENSOR_STATE_CHECK(SENSOR_STATE_OK);
            MCDI_SENSOR_STATE_CHECK(SENSOR_STATE_WARNING);
            MCDI_SENSOR_STATE_CHECK(SENSOR_STATE_FATAL);
            MCDI_SENSOR_STATE_CHECK(SENSOR_STATE_BROKEN);
            MCDI_SENSOR_STATE_CHECK(SENSOR_STATE_NO_READING);
            default:
                return SENSOR_STATE_UNKNOWN;
        }
    }

    ///
    /// Allocate memory for ioctl(SIOCEFX) call.
    ///
    /// @param ioc      [out] Pointer to allocated memory
    /// @param mcdi_req [out] Pointer to efx_mcdi_request structure
    ///                       to be filled
    /// @param isSock         Whether we will call ioctl() on socket
    ///                       or char device fd
    /// @param ifName         Interface name
    ///
    void ioctlPrepare(void **ioc,
                      struct efx_mcdi_request **mcdi_req,
                      bool isSock,
                      String ifName)
    {
        efx_ioctl_data *data;

        assert(ioc != NULL && mcdi_req != NULL);

        if (isSock)
        {
            struct efx_sock_ioctl  efxSockIoc;
            struct efx_sock_ioctl *pEfxSockIoc;

            unsigned int data_offset =
                reinterpret_cast<uint8_t *>(&efxSockIoc.u) -
                reinterpret_cast<uint8_t *>(&efxSockIoc);

            *ioc = new uint8_t[data_offset +
                               sizeof(struct efx_mcdi_request)];
            pEfxSockIoc = reinterpret_cast<struct efx_sock_ioctl *>(*ioc);

            data = &pEfxSockIoc->u;
            pEfxSockIoc->cmd = EFX_MCDI_REQUEST;
        }
        else
        {
            struct efx_ioctl  efxIoc;
            struct efx_ioctl *pEfxIoc;

            unsigned int data_offset =
                reinterpret_cast<uint8_t *>(&efxIoc.u) -
                reinterpret_cast<uint8_t *>(&efxIoc);

            *ioc = new uint8_t[data_offset +
                               sizeof(struct efx_mcdi_request)];
            pEfxIoc = reinterpret_cast<struct efx_ioctl *>(*ioc);

            data = &pEfxIoc->u;    
            pEfxIoc->cmd = EFX_MCDI_REQUEST;
            strncpy(pEfxIoc->if_name, ifName.c_str(),
                    sizeof(pEfxIoc->if_name));
        }

        *mcdi_req = reinterpret_cast<struct efx_mcdi_request *>(data);
    }

    /// Described in sf_mcdi_sensors.h
    int mcdiGetSensors(Array<Sensor> &sensors,
                       int fd,
                       bool isSocket,
                       String ifName)
    {
        void   *ioc;

        struct efx_mcdi_request   *mcdi_req; 
        payload_t                  payload_readings;
        int                        rc;
        uint16_t                  *readings;
        unsigned int               readings_len;
        uint32_t                   page;
        uint32_t                   mask;
        uint16_t                  *lims;
        int                        i;
        unsigned int               info_count = 0;
        struct ifreq               ifreq;

        sensors.clear();

        ioctlPrepare(&ioc, &mcdi_req, isSocket, ifName);

        if (isSocket)
        {
            memcpy(ifreq.ifr_name, ifName.c_str(), sizeof(ifreq.ifr_name));
            ifreq.ifr_data = reinterpret_cast<char *>(ioc);
        }

        mcdi_req->cmd = MC_CMD_READ_SENSORS;
        mcdi_req->len = MC_CMD_READ_SENSORS_EXT_IN_LEN;
        mcdi_req->payload[MC_CMD_READ_SENSORS_EXT_IN_DMA_ADDR_LO_OFST / 4] =
          host_to_le32(0xffffffff);
        mcdi_req->payload[MC_CMD_READ_SENSORS_EXT_IN_DMA_ADDR_HI_OFST / 4] =
          host_to_le32(0xffffffff);
        mcdi_req->payload[MC_CMD_READ_SENSORS_EXT_IN_LENGTH_OFST / 4] =
          host_to_le32(sizeof(mcdi_req->payload));

        if (!isSocket)
            rc = ioctl(fd, SIOCEFX, ioc);
        else
            rc = ioctl(fd, SIOCEFX, &ifreq);
        if (rc != 0)
        {
            PROVIDER_LOG_ERR("ioctl(SIOCEFX/MC_CMD_READ_SENSORS) "
                             "returned %d with errno %d (%s)",
                             rc, errno, strerror(errno));
            delete[] reinterpret_cast<uint8_t *>(ioc);
            return -1;
        }

        memcpy(payload_readings.u8,
               mcdi_req->payload,
               mcdi_req->len);
        readings = payload_readings.u16; 
        readings_len = mcdi_req->len;

        for (page = 0; ; page++)
        {
            memset(mcdi_req, 0, sizeof(*mcdi_req));

            mcdi_req->cmd = MC_CMD_SENSOR_INFO;
            mcdi_req->len = MC_CMD_SENSOR_INFO_EXT_IN_LEN;
            mcdi_req->payload[MC_CMD_SENSOR_INFO_EXT_IN_PAGE_OFST / 4] =
              host_to_le32(page);

            if (!isSocket)
                rc = ioctl(fd, SIOCEFX, ioc);
            else
                rc = ioctl(fd, SIOCEFX, &ifreq);
            if (rc != 0)
            {
                PROVIDER_LOG_ERR("ioctl(SIOCEFX/MC_CMD_SENSOR_INFO) "
                                 "returned %d with errno %d (%s)",
                                 rc, errno, strerror(errno));
                delete[] reinterpret_cast<uint8_t *>(ioc);
                sensors.clear();
                return -1;
            }

            mask = le32_to_host(
              mcdi_req->payload[MC_CMD_SENSOR_INFO_EXT_OUT_MASK_OFST / 4]);
            lims = (uint16_t *)&mcdi_req->payload[1];

            for (i = 0; i < 31; i++)
            {
                uint8_t state  = *(reinterpret_cast<uint8_t *>
                                        (&readings[1]));
                int sensorId = (page << 5) + i;

                if (mask & (1 << i))
                {
                    Sensor sensor;

                    sensor.type = sensorTypeMCDI2Common(sensorId);
                    sensor.state = sensorStateMCDI2Common(
                                        static_cast<unsigned int>(state));
                    sensor.limit1_low = lims[0];
                    sensor.limit1_high = lims[1];
                    sensor.limit2_low = lims[2];
                    sensor.limit2_high = lims[3];
                    sensor.value = readings[0];

#if 0
                    // Temporary; for debug only

                    static unsigned int t = 0;
                    t++;
                    if ((t / 5) % 2 == 1)
                        sensor.state = SENSOR_STATE_BROKEN;
#endif

                    if (sensor.type == SENSOR_UNKNOWN)
                        PROVIDER_LOG_ERR("Unknown sensor id %d encountered",
                                         sensorId);
                    if (sensor.state == SENSOR_STATE_UNKNOWN)
                        PROVIDER_LOG_ERR("Unknown sensor state %d "
                                         "encountered for sensor %d",
                                         static_cast<int>(state),
                                         sensorId);

                    if (sensor.type != SENSOR_UNKNOWN)
                        sensors.append(sensor);

                    lims += 4;
                    readings += 2;
                    info_count++;
                }
            }

            /* Check for another page of sensor info */
            if ((mask & (1 << 31)) == 0)
              break;
        }

        unsigned int info_len = info_count * sizeof(uint16_t) * 2;
        if (info_len != readings_len)
            PROVIDER_LOG_ERR("MC_CMD_READ_SENSORS obtained "
                             "%u bytes of data, whereas "
                             "MC_CMD_SENSOR_INFO got %u bytes",
                             readings_len, info_len);
        delete[] reinterpret_cast<uint8_t *>(ioc);
        return 0;
    }
}
