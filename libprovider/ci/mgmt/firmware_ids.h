/**************************************************************************\
*//*! \file
** <L5_PRIVATE L5_SOURCE>
** \author  mjs
**  \brief  Solarflare firmware type and subtype enumerations
**   \date  2009/07/16
**    \cop  (c) Solarflare Communications Inc.
** </L5_PRIVATE>
*//*
\**************************************************************************/

#ifndef CI_MGMT_FIRMWARE_IDS_H
#define CI_MGMT_FIRMWARE_IDS_H

/* Reference: SF-103588-PS
 *
 * This header file is the input for v5s/scripts/genfwdef. So if you touch it,
 * ensure that v5/scripts/genfwdef still works.
 */

enum {
  FIRMWARE_TYPE_PHY = 0,
  FIRMWARE_TYPE_PHY_LOADER = 1,
  FIRMWARE_TYPE_BOOTROM = 2,
  FIRMWARE_TYPE_MCFW = 3,
  FIRMWARE_TYPE_MCFW_BACKUP = 4,
  FIRMWARE_TYPE_DISABLED_CALLISTO = 5,
  FIRMWARE_TYPE_FPGA = 6,
  FIRMWARE_TYPE_FPGA_BACKUP = 7,
  FIRMWARE_TYPE_FCFW = 8,
  FIRMWARE_TYPE_FCFW_BACKUP = 9,
  FIRMWARE_TYPE_CPLD = 10,
};

enum {
  FIRMWARE_PHY_SUBTYPE_SFX7101B = 0x3,
  FIRMWARE_PHY_SUBTYPE_SFT9001A = 0x8,
  FIRMWARE_PHY_SUBTYPE_QT2025C = 0x9,
  FIRMWARE_PHY_SUBTYPE_SFT9001B = 0xa,
  FIRMWARE_PHY_SUBTYPE_SFL9021 = 0x10,      /* used for loader only */
  FIRMWARE_PHY_SUBTYPE_QT2025_KR = 0x11,    /* QT2025 in KR rather than SFP+ mode */
  FIRMWARE_PHY_SUBTYPE_AEL3020 = 0x12,      /* As seen on the R2 HP blade NIC */
};

enum {
  FIRMWARE_BOOTROM_SUBTYPE_FALCON = 0,
  FIRMWARE_BOOTROM_SUBTYPE_BETHPAGE = 1,
  FIRMWARE_BOOTROM_SUBTYPE_SIENA = 2,
  FIRMWARE_BOOTROM_SUBTYPE_HUNTINGTON = 3,
  FIRMWARE_BOOTROM_SUBTYPE_FARMINGDALE = 4,
  FIRMWARE_BOOTROM_SUBTYPE_GREENPORT = 5,
};

enum {
  FIRMWARE_MCFW_SUBTYPE_COSIM = 0,
  FIRMWARE_MCFW_SUBTYPE_HALFSPEED = 6,
  FIRMWARE_MCFW_SUBTYPE_FLORENCE = 7,
  FIRMWARE_MCFW_SUBTYPE_ZEBEDEE = 8,
  FIRMWARE_MCFW_SUBTYPE_ERMINTRUDE = 9,
  FIRMWARE_MCFW_SUBTYPE_DYLAN = 10,
  FIRMWARE_MCFW_SUBTYPE_BRIAN = 11,
  FIRMWARE_MCFW_SUBTYPE_DOUGAL = 12,
  FIRMWARE_MCFW_SUBTYPE_MR_RUSTY = 13,
  FIRMWARE_MCFW_SUBTYPE_BUXTON = 14,
  FIRMWARE_MCFW_SUBTYPE_HOPE = 15,
  FIRMWARE_MCFW_SUBTYPE_MR_MCHENRY = 16,
  FIRMWARE_MCFW_SUBTYPE_UNCLE_HAMISH = 17,
  FIRMWARE_MCFW_SUBTYPE_TUTTLE = 18,
};

enum {
  FIRMWARE_DISABLED_CALLISTO_SUBTYPE_ALL = 0
};

enum {
  FIRMWARE_FPGA_SUBTYPE_PTP = 1,                /* PTP peripheral */
  FIRMWARE_FPGA_SUBTYPE_PTP_MR_MCHENRY = 2,     /* PTP peripheral on R7 boards */
  FIRMWARE_FPGA_SUBTYPE_FLORENCE = 3,           /* Modena FPGA */
  FIRMWARE_FPGA_SUBTYPE_UNCLE_HAMISH = 4,       /* Modena FPGA: Unknown silicon */
  FIRMWARE_FPGA_SUBTYPE_UNCLE_HAMISH_A7 = 5,    /* Modena FPGA: A7 silicon */
  FIRMWARE_FPGA_SUBTYPE_UNCLE_HAMISH_A5 = 6,    /* Modena FPGA: A5 silicon */
};

enum {
  FIRMWARE_FCFW_SUBTYPE_MODENA = 1,
};

enum {
  FIRMWARE_CPLD_SUBTYPE_SFA6902 = 1,            /* CPLD on Modena (2-port) */
};

enum {
  FIRMWARE_LICENSE_SUBTYPE_AOE = 1,            /* AOE */
};

#endif

