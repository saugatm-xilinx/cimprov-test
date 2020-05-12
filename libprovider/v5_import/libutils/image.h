/*
 * Copyright 2019 Xilinx, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * Copyright 2008 Solarflare Communications Inc. All rights reserved.
 * Use is subject to license terms.
 */

#ifndef IMAGE_H
#define IMAGE_H

#include <stdint.h>
#include "reflash_image_layout.h"

/*
 * Refer to document SF-102785-PS for details of the header structure.
 */

/* Image types */
typedef enum image_type_e {			/* GENERATED BY scripts/genfwdef */

/* FIRMWARE_TYPE enum */
	IMAGE_TYPE_PHY = 0x0,
	IMAGE_TYPE_PHY_LOADER = 0x1,
	IMAGE_TYPE_BOOTROM = 0x2,
	IMAGE_TYPE_MCFW = 0x3,
	IMAGE_TYPE_MCFW_BACKUP = 0x4,
	IMAGE_TYPE_DISABLED_CALLISTO = 0x5,
	IMAGE_TYPE_FPGA = 0x6,
	IMAGE_TYPE_FPGA_BACKUP = 0x7,
	IMAGE_TYPE_FCFW = 0x8,
	IMAGE_TYPE_FCFW_BACKUP = 0x9,
	IMAGE_TYPE_CPLD = 0xa,
	IMAGE_TYPE_MUMFW = 0xb,
	IMAGE_TYPE_UEFIROM = 0xc,
	IMAGE_TYPE_BUNDLE = 0xd,
	IMAGE_TYPE_CMCFW = 0xe,
} image_type_t;

/* PHY flash subtypes */
typedef enum image_subtype_phy_e {		/* GENERATED BY scripts/genfwdef */

/* FIRMWARE_PHY_SUBTYPE enum */
	IMAGE_SUBTYPE_PHY_SFX7101B = 0x3,
	IMAGE_SUBTYPE_PHY_SFT9001A = 0x8,
	IMAGE_SUBTYPE_PHY_QT2025C = 0x9,
	IMAGE_SUBTYPE_PHY_SFT9001B = 0xa,
	IMAGE_SUBTYPE_PHY_SFL9021 = 0x10, /* used for loader only */
	IMAGE_SUBTYPE_PHY_QT2025_KR = 0x11, /* QT2025 in KR rather than SFP+ mode */
	IMAGE_SUBTYPE_PHY_AEL3020 = 0x12, /* As seen on the R2 HP blade NIC */
} image_subtype_phy_t;

typedef enum image_subtype_bootrom_e {		/* GENERATED BY scripts/genfwdef */

/* FIRMWARE_BOOTROM_SUBTYPE enum */
	IMAGE_SUBTYPE_BOOTROM_FALCON = 0x0,
	IMAGE_SUBTYPE_BOOTROM_BETHPAGE = 0x1,
	IMAGE_SUBTYPE_BOOTROM_SIENA = 0x2,
	IMAGE_SUBTYPE_BOOTROM_HUNTINGTON = 0x3,
	IMAGE_SUBTYPE_BOOTROM_FARMINGDALE = 0x4,
	IMAGE_SUBTYPE_BOOTROM_GREENPORT = 0x5,
	IMAGE_SUBTYPE_BOOTROM_MEDFORD = 0x6,
	IMAGE_SUBTYPE_BOOTROM_MEDFORD2 = 0x7,
	IMAGE_SUBTYPE_BOOTROM_RIVERHEAD = 0x8,
} image_subtype_bootrom_t;

typedef enum image_subtype_mcfw_e {		/* GENERATED BY scripts/genfwdef */

/* FIRMWARE_MCFW_SUBTYPE enum */
	IMAGE_SUBTYPE_MCFW_COSIM = 0x0,
	IMAGE_SUBTYPE_MCFW_HALFSPEED = 0x6,
	IMAGE_SUBTYPE_MCFW_FLORENCE = 0x7,
	IMAGE_SUBTYPE_MCFW_ZEBEDEE = 0x8,
	IMAGE_SUBTYPE_MCFW_ERMINTRUDE = 0x9,
	IMAGE_SUBTYPE_MCFW_DYLAN = 0xa,
	IMAGE_SUBTYPE_MCFW_BRIAN = 0xb,
	IMAGE_SUBTYPE_MCFW_DOUGAL = 0xc,
	IMAGE_SUBTYPE_MCFW_MR_RUSTY = 0xd,
	IMAGE_SUBTYPE_MCFW_BUXTON = 0xe,
	IMAGE_SUBTYPE_MCFW_HOPE = 0xf,
	IMAGE_SUBTYPE_MCFW_MR_MCHENRY = 0x10,
	IMAGE_SUBTYPE_MCFW_UNCLE_HAMISH = 0x11,
	IMAGE_SUBTYPE_MCFW_TUTTLE = 0x12,
	IMAGE_SUBTYPE_MCFW_FINLAY = 0x13,
	IMAGE_SUBTYPE_MCFW_KAPTEYN = 0x14,
	IMAGE_SUBTYPE_MCFW_JOHNSON = 0x15,
	IMAGE_SUBTYPE_MCFW_GEHRELS = 0x16,
	IMAGE_SUBTYPE_MCFW_WHIPPLE = 0x17,
	IMAGE_SUBTYPE_MCFW_FORBES = 0x18,
	IMAGE_SUBTYPE_MCFW_LONGMORE = 0x19,
	IMAGE_SUBTYPE_MCFW_HERSCHEL = 0x1a,
	IMAGE_SUBTYPE_MCFW_SHOEMAKER = 0x1b,
	IMAGE_SUBTYPE_MCFW_IKEYA = 0x1c,
	IMAGE_SUBTYPE_MCFW_KOWALSKI = 0x1d,
	IMAGE_SUBTYPE_MCFW_NIMRUD = 0x1e,
	IMAGE_SUBTYPE_MCFW_SPARTA = 0x1f,
	IMAGE_SUBTYPE_MCFW_THEBES = 0x20,
	IMAGE_SUBTYPE_MCFW_ICARUS = 0x21,
	IMAGE_SUBTYPE_MCFW_JERICHO = 0x22,
	IMAGE_SUBTYPE_MCFW_BYBLOS = 0x23,
	IMAGE_SUBTYPE_MCFW_GROAT = 0x24,
	IMAGE_SUBTYPE_MCFW_SHILLING = 0x25,
	IMAGE_SUBTYPE_MCFW_FLORIN = 0x26,
	IMAGE_SUBTYPE_MCFW_THREEPENCE = 0x27, /* Baildon MC firmware for R3.1 */
	IMAGE_SUBTYPE_MCFW_CYCLOPS = 0x28,
	IMAGE_SUBTYPE_MCFW_PENNY = 0x29,
	IMAGE_SUBTYPE_MCFW_BOB = 0x2a,
	IMAGE_SUBTYPE_MCFW_HOG = 0x2b,
	IMAGE_SUBTYPE_MCFW_SOVEREIGN = 0x2c,
	IMAGE_SUBTYPE_MCFW_SOLIDUS = 0x2d,
	IMAGE_SUBTYPE_MCFW_SIXPENCE = 0x2e,
	IMAGE_SUBTYPE_MCFW_CROWN = 0x2f,
	IMAGE_SUBTYPE_MCFW_SOL = 0x30,
	IMAGE_SUBTYPE_MCFW_TANNER = 0x31,
	IMAGE_SUBTYPE_MCFW_JOEY = 0x32, /* Baildon MC firmware for R3.2 and newer revisions */
	IMAGE_SUBTYPE_MCFW_GUINEA = 0x33,
	IMAGE_SUBTYPE_MCFW_BELUGA = 0x40,
	IMAGE_SUBTYPE_MCFW_KALUGA = 0x41,
} image_subtype_mcfw_t;

typedef enum image_subtype_disabled_callisto_e {/* GENERATED BY scripts/genfwdef */

/* FIRMWARE_DISABLED_CALLISTO_SUBTYPE enum */
	IMAGE_SUBTYPE_DISABLED_CALLISTO_ALL = 0x0,
} image_subtype_disabled_callisto_t;

typedef enum image_subtype_fpga_e {		/* GENERATED BY scripts/genfwdef */

/* FIRMWARE_FPGA_SUBTYPE enum */
	IMAGE_SUBTYPE_FPGA_PTP = 0x1, /* PTP peripheral */
	IMAGE_SUBTYPE_FPGA_PTP_MR_MCHENRY = 0x2, /* PTP peripheral on R7 boards */
	IMAGE_SUBTYPE_FPGA_FLORENCE = 0x3, /* Modena FPGA */
	IMAGE_SUBTYPE_FPGA_UNCLE_HAMISH = 0x4, /* Modena FPGA Unknown silicon */
	IMAGE_SUBTYPE_FPGA_UNCLE_HAMISH_A7 = 0x5, /* Modena FPGA A7 silicon */
	IMAGE_SUBTYPE_FPGA_UNCLE_HAMISH_A5 = 0x6, /* Modena FPGA A5 silicon */
	IMAGE_SUBTYPE_FPGA_SHOEMAKER = 0x7, /* Sorrento FPGA Unknown silicon */
	IMAGE_SUBTYPE_FPGA_SHOEMAKER_A5 = 0x8, /* Sorrento FPGA A5 silicon */
	IMAGE_SUBTYPE_FPGA_SHOEMAKER_A7 = 0x9, /* Sorrento FPGA A7 silicon */
} image_subtype_fpga_t;

typedef enum image_subtype_fcfw_e {		/* GENERATED BY scripts/genfwdef */

/* FIRMWARE_FCFW_SUBTYPE enum */
	IMAGE_SUBTYPE_FCFW_MODENA = 0x1,
	IMAGE_SUBTYPE_FCFW_SORRENTO = 0x2,
} image_subtype_fcfw_t;

typedef enum image_subtype_cpld_e {		/* GENERATED BY scripts/genfwdef */

/* FIRMWARE_CPLD_SUBTYPE enum */
	IMAGE_SUBTYPE_CPLD_SFA6902 = 0x1, /* CPLD on Modena (2-port) */
} image_subtype_cpld_t;

typedef enum image_subtype_license_e {		/* GENERATED BY scripts/genfwdef */

/* FIRMWARE_LICENSE_SUBTYPE enum */
	IMAGE_SUBTYPE_LICENSE_AOE = 0x1, /* AOE */
} image_subtype_license_t;

typedef enum image_subtype_mumfw_e {		/* GENERATED BY scripts/genfwdef */

/* FIRMWARE_MUMFW_SUBTYPE enum */
	IMAGE_SUBTYPE_MUMFW_MADAM_BLUE = 0x1, /* Sorrento MUM firmware */
	IMAGE_SUBTYPE_MUMFW_ICARUS = 0x2, /* Malaga MUM firmware */
	IMAGE_SUBTYPE_MUMFW_JERICHO = 0x3, /* Emma MUM firmware */
	IMAGE_SUBTYPE_MUMFW_BYBLOS = 0x4, /* Pagnell MUM firmware */
	IMAGE_SUBTYPE_MUMFW_SHILLING = 0x5, /* Bradford R1.x MUM firmware */
	IMAGE_SUBTYPE_MUMFW_FLORIN = 0x6, /* Bingley MUM firmware */
	IMAGE_SUBTYPE_MUMFW_THREEPENCE = 0x7, /* Baildon MUM firmware */
	IMAGE_SUBTYPE_MUMFW_CYCLOPS = 0x8, /* Talbot MUM firmware */
	IMAGE_SUBTYPE_MUMFW_PENNY = 0x9, /* Batley MUM firmware */
	IMAGE_SUBTYPE_MUMFW_BOB = 0xa, /* Bradford R2.x MUM firmware */
	IMAGE_SUBTYPE_MUMFW_HOG = 0xb, /* Roxburgh MUM firmware */
	IMAGE_SUBTYPE_MUMFW_SOVEREIGN = 0xc, /* Stirling MUM firmware */
	IMAGE_SUBTYPE_MUMFW_SOLIDUS = 0xd, /* Roxburgh R2 MUM firmware */
	IMAGE_SUBTYPE_MUMFW_SIXPENCE = 0xe, /* Melrose MUM firmware for Dell cards */
	IMAGE_SUBTYPE_MUMFW_CROWN = 0xf, /* Coldstream MUM firmware */
	IMAGE_SUBTYPE_MUMFW_SOL = 0x10, /* Roxburgh R2 MUM firmware for Dell cards with signed-bundle-update */
	IMAGE_SUBTYPE_MUMFW_KALUGA = 0x11, /* York MUM firmware */
	IMAGE_SUBTYPE_MUMFW_STERLET = 0x12, /* Bourn MUM firmware */
	IMAGE_SUBTYPE_MUMFW_TANNER = 0x13, /* Melrose MUM firmware for channel cards */
	IMAGE_SUBTYPE_MUMFW_JOEY = 0x14, /* Baildon MUM firmware for R3.2 and newer revisions */
	IMAGE_SUBTYPE_MUMFW_GUINEA = 0x15, /* Lincoln MUM firmware */
} image_subtype_mumfw_t;

#define IMAGE_SUBTYPE_UEFIROM_ALL IMAGE_SUBTYPE_UEFIROM_EF10
typedef enum image_subtype_uefirom_e {		/* GENERATED BY scripts/genfwdef */

/* FIRMWARE_UEFIROM_SUBTYPE enum */
	IMAGE_SUBTYPE_UEFIROM_EF10 = 0x0, /* Support for SFC91XX controller and SFC92XX controllers */
	IMAGE_SUBTYPE_UEFIROM_EF10_DELL = 0x1, /* Support SFX92XX DELL OEM controllers */
} image_subtype_uefirom_t;

typedef enum image_subtype_bundle_e {		/* GENERATED BY scripts/genfwdef */

/* FIRMWARE_BUNDLE_SUBTYPE enum */
	IMAGE_SUBTYPE_BUNDLE_DELL_X2522_25G = 0x1, /* X2522-25G for Dell with bundle update support */
	IMAGE_SUBTYPE_BUNDLE_X2552 = 0x2, /* X2552 OCP NIC - firmware bundle */
	IMAGE_SUBTYPE_BUNDLE_DELL_X2562 = 0x3, /* X2562 OCP NIC for Dell - firmware bundle */
	IMAGE_SUBTYPE_BUNDLE_X2562 = 0x4, /* X2562 OCP NIC - firmware bundle */
	IMAGE_SUBTYPE_BUNDLE_U25 = 0x5, /* U25 - firmware bundle */
} image_subtype_bundle_t;

typedef enum image_subtype_cmcfw_e {		/* GENERATED BY scripts/genfwdef */

/* FIRMWARE_CMCFW_SUBTYPE enum */
	IMAGE_SUBTYPE_CMCFW_BELUGA = 0x1, /* Riverhead VCU1525 CMC firmware */
	IMAGE_SUBTYPE_CMCFW_KALUGA = 0x2, /* York (X3x42) board CMC firmware */
} image_subtype_cmcfw_t;

#endif /* IMAGE_H */

