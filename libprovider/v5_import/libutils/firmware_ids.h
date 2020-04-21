/*
 * This is NOT the original source file. Do NOT edit it.
 * To update the board and firmware ids, please edit the copy in
 * the sfregistry repo and then, in that repo,
 * "make id_headers" or "make export" to
 * regenerate and export all types of headers.
 */
/*
 * (c) Copyright 2019 Xilinx, Inc. All rights reserved.
 *
 * This file contains confidential and proprietary information
 * of Xilinx, Inc. and is protected under U.S. and
 * international copyright and other intellectual property
 * laws.
 *
 * DISCLAIMER
 * This disclaimer is not a license and does not grant any
 * rights to the materials distributed herewith. Except as
 * otherwise provided in a valid license issued to you by
 * Xilinx, and to the maximum extent permitted by applicable
 * law: (1) THESE MATERIALS ARE MADE AVAILABLE "AS IS" AND
 * WITH ALL FAULTS, AND XILINX HEREBY DISCLAIMS ALL WARRANTIES
 * AND CONDITIONS, EXPRESS, IMPLIED, OR STATUTORY, INCLUDING
 * BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, NON-
 * INFRINGEMENT, OR FITNESS FOR ANY PARTICULAR PURPOSE; and
 * (2) Xilinx shall not be liable (whether in contract or tort,
 * including negligence, or under any other theory of
 * liability) for any loss or damage of any kind or nature
 * related to, arising under or in connection with these
 * materials, including for any direct, or any indirect,
 * special, incidental, or consequential loss or damage
 * (including loss of data, profits, goodwill, or any type of
 * loss or damage suffered as a result of any action brought
 * by a third party) even if such damage or loss was
 * reasonably foreseeable or Xilinx had been advised of the
 * possibility of the same.
 *
 * CRITICAL APPLICATIONS
 * Xilinx products are not designed or intended to be fail-
 * safe, or for use in any application requiring fail-safe
 * performance, such as life-support or safety devices or
 * systems, Class III medical devices, nuclear facilities,
 * applications related to the deployment of airbags, or any
 * other applications that could lead to death, personal
 * injury, or severe property or environmental damage
 * (individually and collectively, "Critical
 * Applications"). Customer assumes the sole risk and
 * liability of any use of Xilinx products in Critical
 * Applications, subject only to applicable laws and
 * regulations governing limitations on product liability.
 *
 * THIS COPYRIGHT NOTICE AND DISCLAIMER MUST BE RETAINED AS
 * PART OF THIS FILE AT ALL TIMES
 */
/* Do not edit this file. It was generated by mcdigen.py */
#ifndef CI_MGMT_FIRMWARE_IDS_H
#define CI_MGMT_FIRMWARE_IDS_H
enum {

/* FIRMWARE_TYPE enum */
  FIRMWARE_TYPE_PHY = 0x0,
  FIRMWARE_TYPE_PHY_LOADER = 0x1,
  FIRMWARE_TYPE_BOOTROM = 0x2,
  FIRMWARE_TYPE_MCFW = 0x3,
  FIRMWARE_TYPE_MCFW_BACKUP = 0x4,
  FIRMWARE_TYPE_DISABLED_CALLISTO = 0x5,
  FIRMWARE_TYPE_FPGA = 0x6,
  FIRMWARE_TYPE_FPGA_BACKUP = 0x7,
  FIRMWARE_TYPE_FCFW = 0x8,
  FIRMWARE_TYPE_FCFW_BACKUP = 0x9,
  FIRMWARE_TYPE_CPLD = 0xa,
  FIRMWARE_TYPE_MUMFW = 0xb,
  FIRMWARE_TYPE_UEFIROM = 0xc,
  FIRMWARE_TYPE_BUNDLE = 0xd,
  FIRMWARE_TYPE_CMCFW = 0xe,
};
enum {

/* FIRMWARE_PHY_SUBTYPE enum */
  FIRMWARE_PHY_SUBTYPE_SFX7101B = 0x3,
  FIRMWARE_PHY_SUBTYPE_SFT9001A = 0x8,
  FIRMWARE_PHY_SUBTYPE_QT2025C = 0x9,
  FIRMWARE_PHY_SUBTYPE_SFT9001B = 0xa,
  FIRMWARE_PHY_SUBTYPE_SFL9021 = 0x10, /* used for loader only */
  FIRMWARE_PHY_SUBTYPE_QT2025_KR = 0x11, /* QT2025 in KR rather than SFP+ mode */
  FIRMWARE_PHY_SUBTYPE_AEL3020 = 0x12, /* As seen on the R2 HP blade NIC */
};
enum {

/* FIRMWARE_BOOTROM_SUBTYPE enum */
  FIRMWARE_BOOTROM_SUBTYPE_FALCON = 0x0,
  FIRMWARE_BOOTROM_SUBTYPE_BETHPAGE = 0x1,
  FIRMWARE_BOOTROM_SUBTYPE_SIENA = 0x2,
  FIRMWARE_BOOTROM_SUBTYPE_HUNTINGTON = 0x3,
  FIRMWARE_BOOTROM_SUBTYPE_FARMINGDALE = 0x4,
  FIRMWARE_BOOTROM_SUBTYPE_GREENPORT = 0x5,
  FIRMWARE_BOOTROM_SUBTYPE_MEDFORD = 0x6,
  FIRMWARE_BOOTROM_SUBTYPE_MEDFORD2 = 0x7,
  FIRMWARE_BOOTROM_SUBTYPE_RIVERHEAD = 0x8,
};
enum {

/* FIRMWARE_MCFW_SUBTYPE enum */
  FIRMWARE_MCFW_SUBTYPE_COSIM = 0x0,
  FIRMWARE_MCFW_SUBTYPE_HALFSPEED = 0x6,
  FIRMWARE_MCFW_SUBTYPE_FLORENCE = 0x7,
  FIRMWARE_MCFW_SUBTYPE_ZEBEDEE = 0x8,
  FIRMWARE_MCFW_SUBTYPE_ERMINTRUDE = 0x9,
  FIRMWARE_MCFW_SUBTYPE_DYLAN = 0xa,
  FIRMWARE_MCFW_SUBTYPE_BRIAN = 0xb,
  FIRMWARE_MCFW_SUBTYPE_DOUGAL = 0xc,
  FIRMWARE_MCFW_SUBTYPE_MR_RUSTY = 0xd,
  FIRMWARE_MCFW_SUBTYPE_BUXTON = 0xe,
  FIRMWARE_MCFW_SUBTYPE_HOPE = 0xf,
  FIRMWARE_MCFW_SUBTYPE_MR_MCHENRY = 0x10,
  FIRMWARE_MCFW_SUBTYPE_UNCLE_HAMISH = 0x11,
  FIRMWARE_MCFW_SUBTYPE_TUTTLE = 0x12,
  FIRMWARE_MCFW_SUBTYPE_FINLAY = 0x13,
  FIRMWARE_MCFW_SUBTYPE_KAPTEYN = 0x14,
  FIRMWARE_MCFW_SUBTYPE_JOHNSON = 0x15,
  FIRMWARE_MCFW_SUBTYPE_GEHRELS = 0x16,
  FIRMWARE_MCFW_SUBTYPE_WHIPPLE = 0x17,
  FIRMWARE_MCFW_SUBTYPE_FORBES = 0x18,
  FIRMWARE_MCFW_SUBTYPE_LONGMORE = 0x19,
  FIRMWARE_MCFW_SUBTYPE_HERSCHEL = 0x1a,
  FIRMWARE_MCFW_SUBTYPE_SHOEMAKER = 0x1b,
  FIRMWARE_MCFW_SUBTYPE_IKEYA = 0x1c,
  FIRMWARE_MCFW_SUBTYPE_KOWALSKI = 0x1d,
  FIRMWARE_MCFW_SUBTYPE_NIMRUD = 0x1e,
  FIRMWARE_MCFW_SUBTYPE_SPARTA = 0x1f,
  FIRMWARE_MCFW_SUBTYPE_THEBES = 0x20,
  FIRMWARE_MCFW_SUBTYPE_ICARUS = 0x21,
  FIRMWARE_MCFW_SUBTYPE_JERICHO = 0x22,
  FIRMWARE_MCFW_SUBTYPE_BYBLOS = 0x23,
  FIRMWARE_MCFW_SUBTYPE_GROAT = 0x24,
  FIRMWARE_MCFW_SUBTYPE_SHILLING = 0x25,
  FIRMWARE_MCFW_SUBTYPE_FLORIN = 0x26,
  FIRMWARE_MCFW_SUBTYPE_THREEPENCE = 0x27, /* Baildon MC firmware for R3.1 */
  FIRMWARE_MCFW_SUBTYPE_CYCLOPS = 0x28,
  FIRMWARE_MCFW_SUBTYPE_PENNY = 0x29,
  FIRMWARE_MCFW_SUBTYPE_BOB = 0x2a,
  FIRMWARE_MCFW_SUBTYPE_HOG = 0x2b,
  FIRMWARE_MCFW_SUBTYPE_SOVEREIGN = 0x2c,
  FIRMWARE_MCFW_SUBTYPE_SOLIDUS = 0x2d,
  FIRMWARE_MCFW_SUBTYPE_SIXPENCE = 0x2e,
  FIRMWARE_MCFW_SUBTYPE_CROWN = 0x2f,
  FIRMWARE_MCFW_SUBTYPE_SOL = 0x30,
  FIRMWARE_MCFW_SUBTYPE_TANNER = 0x31,
  FIRMWARE_MCFW_SUBTYPE_JOEY = 0x32, /* Baildon MC firmware for R3.2 and newer revisions */
  FIRMWARE_MCFW_SUBTYPE_GUINEA = 0x33,
  FIRMWARE_MCFW_SUBTYPE_BELUGA = 0x40,
  FIRMWARE_MCFW_SUBTYPE_KALUGA = 0x41,
};
enum {

/* FIRMWARE_DISABLED_CALLISTO_SUBTYPE enum */
  FIRMWARE_DISABLED_CALLISTO_SUBTYPE_ALL = 0x0,
};
enum {

/* FIRMWARE_FPGA_SUBTYPE enum */
  FIRMWARE_FPGA_SUBTYPE_PTP = 0x1, /* PTP peripheral */
  FIRMWARE_FPGA_SUBTYPE_PTP_MR_MCHENRY = 0x2, /* PTP peripheral on R7 boards */
  FIRMWARE_FPGA_SUBTYPE_FLORENCE = 0x3, /* Modena FPGA */
  FIRMWARE_FPGA_SUBTYPE_UNCLE_HAMISH = 0x4, /* Modena FPGA Unknown silicon */
  FIRMWARE_FPGA_SUBTYPE_UNCLE_HAMISH_A7 = 0x5, /* Modena FPGA A7 silicon */
  FIRMWARE_FPGA_SUBTYPE_UNCLE_HAMISH_A5 = 0x6, /* Modena FPGA A5 silicon */
  FIRMWARE_FPGA_SUBTYPE_SHOEMAKER = 0x7, /* Sorrento FPGA Unknown silicon */
  FIRMWARE_FPGA_SUBTYPE_SHOEMAKER_A5 = 0x8, /* Sorrento FPGA A5 silicon */
  FIRMWARE_FPGA_SUBTYPE_SHOEMAKER_A7 = 0x9, /* Sorrento FPGA A7 silicon */
};
enum {

/* FIRMWARE_FCFW_SUBTYPE enum */
  FIRMWARE_FCFW_SUBTYPE_MODENA = 0x1,
  FIRMWARE_FCFW_SUBTYPE_SORRENTO = 0x2,
};
enum {

/* FIRMWARE_CPLD_SUBTYPE enum */
  FIRMWARE_CPLD_SUBTYPE_SFA6902 = 0x1, /* CPLD on Modena (2-port) */
};
enum {

/* FIRMWARE_LICENSE_SUBTYPE enum */
  FIRMWARE_LICENSE_SUBTYPE_AOE = 0x1, /* AOE */
};
enum {

/* FIRMWARE_MUMFW_SUBTYPE enum */
  FIRMWARE_MUMFW_SUBTYPE_MADAM_BLUE = 0x1, /* Sorrento MUM firmware */
  FIRMWARE_MUMFW_SUBTYPE_ICARUS = 0x2, /* Malaga MUM firmware */
  FIRMWARE_MUMFW_SUBTYPE_JERICHO = 0x3, /* Emma MUM firmware */
  FIRMWARE_MUMFW_SUBTYPE_BYBLOS = 0x4, /* Pagnell MUM firmware */
  FIRMWARE_MUMFW_SUBTYPE_SHILLING = 0x5, /* Bradford R1.x MUM firmware */
  FIRMWARE_MUMFW_SUBTYPE_FLORIN = 0x6, /* Bingley MUM firmware */
  FIRMWARE_MUMFW_SUBTYPE_THREEPENCE = 0x7, /* Baildon MUM firmware */
  FIRMWARE_MUMFW_SUBTYPE_CYCLOPS = 0x8, /* Talbot MUM firmware */
  FIRMWARE_MUMFW_SUBTYPE_PENNY = 0x9, /* Batley MUM firmware */
  FIRMWARE_MUMFW_SUBTYPE_BOB = 0xa, /* Bradford R2.x MUM firmware */
  FIRMWARE_MUMFW_SUBTYPE_HOG = 0xb, /* Roxburgh MUM firmware */
  FIRMWARE_MUMFW_SUBTYPE_SOVEREIGN = 0xc, /* Stirling MUM firmware */
  FIRMWARE_MUMFW_SUBTYPE_SOLIDUS = 0xd, /* Roxburgh R2 MUM firmware */
  FIRMWARE_MUMFW_SUBTYPE_SIXPENCE = 0xe, /* Melrose MUM firmware for Dell cards */
  FIRMWARE_MUMFW_SUBTYPE_CROWN = 0xf, /* Coldstream MUM firmware */
  FIRMWARE_MUMFW_SUBTYPE_SOL = 0x10, /* Roxburgh R2 MUM firmware for Dell cards with signed-bundle-update */
  FIRMWARE_MUMFW_SUBTYPE_KALUGA = 0x11, /* York MUM firmware */
  FIRMWARE_MUMFW_SUBTYPE_STERLET = 0x12, /* Bourn MUM firmware */
  FIRMWARE_MUMFW_SUBTYPE_TANNER = 0x13, /* Melrose MUM firmware for channel cards */
  FIRMWARE_MUMFW_SUBTYPE_JOEY = 0x14, /* Baildon MUM firmware for R3.2 and newer revisions */
  FIRMWARE_MUMFW_SUBTYPE_GUINEA = 0x15, /* Lincoln MUM firmware */
};
enum {

/* FIRMWARE_UEFIROM_SUBTYPE enum */
  FIRMWARE_UEFIROM_SUBTYPE_EF10 = 0x0, /* Support for SFC91XX controller and SFC92XX controllers */
  FIRMWARE_UEFIROM_SUBTYPE_EF10_DELL = 0x1, /* Support SFX92XX DELL OEM controllers */
};
enum {

/* FIRMWARE_BUNDLE_SUBTYPE enum */
  FIRMWARE_BUNDLE_SUBTYPE_DELL_X2522_25G = 0x1, /* X2522-25G for Dell with bundle update support */
  FIRMWARE_BUNDLE_SUBTYPE_X2552 = 0x2, /* X2552 OCP NIC - firmware bundle */
  FIRMWARE_BUNDLE_SUBTYPE_DELL_X2562 = 0x3, /* X2562 OCP NIC for Dell - firmware bundle */
  FIRMWARE_BUNDLE_SUBTYPE_X2562 = 0x4, /* X2562 OCP NIC - firmware bundle */
  FIRMWARE_BUNDLE_SUBTYPE_U25 = 0x5, /* U25 - firmware bundle */
};
enum {

/* FIRMWARE_CMCFW_SUBTYPE enum */
  FIRMWARE_CMCFW_SUBTYPE_BELUGA = 0x1, /* Riverhead VCU1525 CMC firmware */
  FIRMWARE_CMCFW_SUBTYPE_KALUGA = 0x2, /* York (X3x42) board CMC firmware */
};
enum {

/* CONTROLLER_ID enum */
  CONTROLLER_ID_ANY = 0x0,
  CONTROLLER_ID_FALCON = 0x1, /* SFC4000 "Falcon" controller */
  CONTROLLER_ID_BETHPAGE = 0x2, /* SFC9020 "Bethpage" and SFC9020X "Maranello" controllers */
  CONTROLLER_ID_SIENA = 0x3, /* SFL9021 "Siena" controller */
  CONTROLLER_ID_HUNTINGTON = 0x4, /* SFC9120 "Farmingdale" and SFC9140 "Greenport" controllers */
  CONTROLLER_ID_MEDFORD = 0x5, /* SFC9220 "Medford" controller */
  CONTROLLER_ID_MEDFORD2 = 0x6, /* SFC9250 "Medford2" controller */
  CONTROLLER_ID_RIVERHEAD = 0x7, /* FPGA SmartNIC */
};
#define FIRMWARE_UEFIROM_SUBTYPE_ALL FIRMWARE_UEFIROM_SUBTYPE_EF10
#endif
