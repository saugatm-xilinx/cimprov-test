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
  FIRMWARE_TYPE_MUMFW = 11,
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
  FIRMWARE_BOOTROM_SUBTYPE_MEDFORD = 6,
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
  FIRMWARE_MCFW_SUBTYPE_FINLAY = 19,
  FIRMWARE_MCFW_SUBTYPE_KAPTEYN = 20,
  FIRMWARE_MCFW_SUBTYPE_JOHNSON = 21,
  FIRMWARE_MCFW_SUBTYPE_GEHRELS = 22,
  FIRMWARE_MCFW_SUBTYPE_WHIPPLE = 23,
  FIRMWARE_MCFW_SUBTYPE_FORBES = 24,
  FIRMWARE_MCFW_SUBTYPE_LONGMORE = 25,
  FIRMWARE_MCFW_SUBTYPE_HERSCHEL = 26,
  FIRMWARE_MCFW_SUBTYPE_SHOEMAKER = 27,
  FIRMWARE_MCFW_SUBTYPE_IKEYA = 28,
  FIRMWARE_MCFW_SUBTYPE_KOWALSKI = 29,
  FIRMWARE_MCFW_SUBTYPE_NIMRUD = 30,
  FIRMWARE_MCFW_SUBTYPE_SPARTA = 31,
  FIRMWARE_MCFW_SUBTYPE_THEBES = 32,
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
  FIRMWARE_FPGA_SUBTYPE_SHOEMAKER = 7,          /* Sorrento FPGA: Unknown silicon */
  FIRMWARE_FPGA_SUBTYPE_SHOEMAKER_A5 = 8,       /* Sorrento FPGA: A5 silicon */
  FIRMWARE_FPGA_SUBTYPE_SHOEMAKER_A7 = 9,       /* Sorrento FPGA: A7 silicon */
};

enum {
  FIRMWARE_FCFW_SUBTYPE_MODENA = 1,
  FIRMWARE_FCFW_SUBTYPE_SORRENTO = 2,
};

enum {
  FIRMWARE_CPLD_SUBTYPE_SFA6902 = 1,            /* CPLD on Modena (2-port) */
};

enum {
  FIRMWARE_LICENSE_SUBTYPE_AOE = 1,            /* AOE */
};

enum {
  FIRMWARE_MUMFW_SUBTYPE_MADAM_BLUE = 1,       /* Sorrento MUM firmware */
};

#endif

