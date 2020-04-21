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
** \author  mc
**  \brief  Definitions for nvram-representation of signed firmware images
**   \date  2016/07/28
**    \cop  (c) Solarflare Communications Inc.
** </L5_PRIVATE>
*//*
\**************************************************************************/

/* These structures define the layouts for the signed firmware image binary
 * saved in NVRAM. The original image is in the Cryptographic message
 * syntax (CMS) format which contains the bootable firmware binary plus the
 * signatures. The entire image is written into NVRAM to enable the firmware
 * to validate the signatures. However, the bootrom still requires the
 * bootable-image to start at offset 0 of the NVRAM partition. Hence the image
 * is parsed upfront by host utilities (sfupdate) and written into nvram as
 * 'signed_image_chunks' described by a header.
 *
 * This file is used by the MC as well as host-utilities (sfupdate).
 */


#ifndef CI_MGMT_SIGNED_IMAGE_LAYOUT_H
#define CI_MGMT_SIGNED_IMAGE_LAYOUT_H

/* Signed image chunk type identifiers */
enum {
  SIGNED_IMAGE_CHUNK_CMS_HEADER,        /* CMS header describing the signed data */
  SIGNED_IMAGE_CHUNK_REFLASH_HEADER,    /* Reflash header */
  SIGNED_IMAGE_CHUNK_IMAGE,             /* Bootable binary image */
  SIGNED_IMAGE_CHUNK_REFLASH_TRAILER,   /* Reflash trailer */
  SIGNED_IMAGE_CHUNK_SIGNATURE,         /* Remaining contents of the signed image,
                                         * including the certifiates and signature */
  NUM_SIGNED_IMAGE_CHUNKS,
};

/* Magic */
#define SIGNED_IMAGE_CHUNK_HDR_MAGIC 0xEF105161 /* EF10 SIGned Image */

/* Initial version definition - version 1 */
#define SIGNED_IMAGE_CHUNK_HDR_VERSION 0x1

/* Header length is 32 bytes */
#define SIGNED_IMAGE_CHUNK_HDR_LEN   32
/* Structure describing the header of each chunk of signed image
 * as stored in nvram
 */
typedef struct signed_image_chunk_hdr_e {
  /* Magic field to recognise a valid entry
   * should match SIGNED_IMAGE_CHUNK_HDR_MAGIC
   */
  uint32_t magic;
  /* Version number of this header */
  uint32_t version;
  /* Chunk type identifier */
  uint32_t id;
  /* Chunk offset */
  uint32_t offset;
  /* Chunk length */
  uint32_t len;
  /* Reserved for future expansion of this structure - always set to zeros */
  uint32_t reserved[3];
} signed_image_chunk_hdr_t;

#endif /* CI_MGMT_SIGNED_IMAGE_LAYOUT_H */
