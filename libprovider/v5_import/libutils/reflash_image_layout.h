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
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
