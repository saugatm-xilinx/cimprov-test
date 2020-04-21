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
** \author  mchopra
**  \brief  Reflash image header definitions
**   \date  2019/02/04
**    \cop  (c) Solarflare Communications Inc.
** </L5_PRIVATE>
*//*
\**************************************************************************/

#ifndef REFLASH_IMAGE_LAYOUT_H
#define REFLASH_IMAGE_LAYOUT_H

#ifdef _MSC_VER
#pragma pack(push,1)
#else
#pragma pack(1)
#endif

/*
 * Refer to document SF-102785-PS for details of the header structure.
 */
#define IMAGE_HEADER_MAGIC     0x0106F1A5
#define IMAGE_HEADER_VERSION   0x00000004

/*
 * Controller versions (denoted by PCI device id)
 */
#define IMAGE_CONTROLLER_RESERVED 0x00000000
#define IMAGE_CONTROLLER_0703     0x00000001      /* Falcon A0/A1 */
#define IMAGE_CONTROLLER_0710     0x00000002      /* Falcon B0 */
#define IMAGE_CONTROLLER_0803     0x00000003      /* Siena */
#define IMAGE_CONTROLLER_0903     0x00000004      /* Huntington */

/*
 * Format of reflash header
 */
typedef struct image_header_s {
  uint32_t ih_magic;
  uint32_t ih_version;
  uint32_t ih_type;
  uint32_t ih_subtype;
  uint32_t ih_code_size;
  uint32_t ih_size;

  /* The first and second parts of these unions implement Option A
   * and Option B respectively, as defined in SF-102785-PS
   */

  union {
    uint32_t ih_controller_version_min;
    struct {
      uint16_t ih_controller_version_min_short;
      uint8_t ih_extra_version_a;
      uint8_t ih_extra_version_b;
    };
  };
  union {
    uint32_t ih_controller_version_max;
    struct {
      uint16_t ih_controller_version_max_short;
      uint8_t ih_extra_version_c;
      uint8_t ih_extra_version_d;
    };
  };

  uint16_t ih_code_version_a;
  uint16_t ih_code_version_b;
  uint16_t ih_code_version_c;
  uint16_t ih_code_version_d;
} image_header_t;

#ifdef _MSC_VER
#pragma pack(pop)
#else
#pragma pack()
#endif

#define IMAGE_HEADER_SIZE 40
/*
 * Format of reflash trailer
 */
typedef struct image_trailer_s {
  uint32_t crc32;
} image_trailer_t;

#endif /* REFLASH_IMAGE_LAYOUT_H */
