/*
 * This is NOT the original source file. Do NOT edit it.
 * To update the tlv layout, please edit the copy in
 * the sfregistry repo and then, in that repo,
 * "make tlv_headers" or "make export" to
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
/**************************************************************************\
*//*! \file
** <L5_PRIVATE L5_SOURCE>
** \author  ajr
**  \brief  Platform specific TLV definitions
**   \date  2013/11/14
**    \cop  (c) Solarflare Communications Inc.
** </L5_PRIVATE>
*//*
\**************************************************************************/

/* Default, min and max values for BIU_TRGT_CTL_REG fields. */
#define DEFAULT_BIU_PF_APER_HUNT     (0x17)  /* PF BAR2 (VI window) */
#define     MIN_BIU_PF_APER_HUNT     (0x0D)
#define     MAX_BIU_PF_APER_HUNT     (0x17)

#define DEFAULT_BIU_VF_APER_HUNT     (0x14)  /* VF BAR0 (VI window) */
#define     MIN_BIU_VF_APER_HUNT     (0x0D)
#define     MAX_BIU_VF_APER_HUNT     (0x17)

#define DEFAULT_BIU_INT_APER_HUNT    (0xE)   /* PF BAR4 and VF BAR2 (MSI-X) */
#define     MIN_BIU_INT_APER_HUNT    (0x8)
#define     MAX_BIU_INT_APER_HUNT    (0xF)

/* Default values for BAR Mask sizes */

/* The core requires I/O apertures to be 256 bytes (SF-105340-TC 5.1.7.11.5)
 * so do that  */
#define DEFAULT_PF_BAR0_APERTURE_HUNT    (0x8)
#define DEFAULT_PF_BAR2_APERTURE_HUNT    DEFAULT_BIU_PF_APER_HUNT

#define DEFAULT_VF_BAR0_APERTURE_HUNT    DEFAULT_BIU_VF_APER_HUNT
