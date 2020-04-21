/*
 * This is NOT the original source file. Do NOT edit it.
 * To update the image layout headers, please edit the copy in
 * the sfregistry repo and then, in that repo,
 * "make layout_headers" or "make export" to
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
** \author gnb, mjs
**  \brief Dynamic configuration flash layout for the Siena MC.
**   \date   2008/09/09
**    \cop  (c) Level 5 Networks Limited.
** </L5_PRIVATE>
*//*
\**************************************************************************/

#ifndef MC_DYNAMIC_CFG_H
#define MC_DYNAMIC_CFG_H

/* Dynamic configuration header, which lives at the start of the per-port
 * dynamic config erase sector.
 *
 * - MC firmware parses dynamic_vpd_offset to find VPD within sector (for
 *   serving VPD via PCI only; VPD content is opaque to MC)
 * - remaining fields are for driver use only; opaque to MC
 *
 * Drivers should cope with a missing dynamic_config structure, and reinialize
 * it as required.
 */

#define SIENA_MC_DYNAMIC_CONFIG_MAGIC (0xBDCFDDDD)
#define SIENA_MC_DYNAMIC_CONFIG_VERSION (0)

typedef struct siena_mc_fw_version_s {
  uint32_t fw_subtype;
  uint16_t version_w;
  uint16_t version_x;
  uint16_t version_y;
  uint16_t version_z;
} siena_mc_fw_version_t;

typedef struct siena_mc_dynamic_config_hdr_s {
  uint32_t magic;               /* = SIENA_MC_DYNAMIC_CONFIG_MAGIC */
  uint16_t length;              /* of header area (i.e. not including VPD) */
  uint8_t  version;
  uint8_t  csum;                /* over header area (i.e. not including VPD) */
  uint32_t dynamic_vpd_offset;
  uint32_t dynamic_vpd_length;
  uint32_t num_fw_version_items;
  siena_mc_fw_version_t fw_version[];
} siena_mc_dynamic_config_hdr_t;

#endif

