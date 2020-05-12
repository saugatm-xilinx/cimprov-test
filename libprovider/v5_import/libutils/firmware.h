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

/**************************************************************************/
/*!  \file  firmware.h
** \author  bwh
**  \brief  Firmware interface definitions
**   \date  2008/10/06
**    \cop  Copyright 2008 Solarflare Communications Inc.
*//************************************************************************/

#ifndef SFUTILS_FIRMWARE_H
#define SFUTILS_FIRMWARE_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#include "image.h"
#include "signed_image_layout.h"

struct firmware_version {
  uint16_t a, b, c, d;
};

struct sfu_device;
struct dynamic_cfg_context;

typedef enum image_format_e {
  /* There is no image data available, i.e. when there is no default image. */
  IMAGE_FORMAT_NO_IMAGE = 0,
  /* The image data is not valid for any of the supported formats. */
  IMAGE_FORMAT_INVALID = 1,
  /* The image is a valid unsigned image, i.e. .dat file format. */
  IMAGE_FORMAT_UNSIGNED = 2,
  /* The image is a valid signed image, i.e. .p7b file format. */
  IMAGE_FORMAT_SIGNED = 3,
} image_format_t;

typedef struct sfupdate_image_s {
  image_format_t format;
  const uint8_t* raw_image;
  size_t raw_image_len;

  const uint8_t* cms_header;
  size_t cms_header_len;

  image_header_t* reflash_header;
  size_t reflash_header_len;

  const uint8_t* image_code;
  size_t image_code_len;

  image_trailer_t* reflash_trailer;
  size_t reflash_trailer_len;

  const uint8_t* signature;
  size_t signature_len;
} sfupdate_image_t;

/*
 * Firmware operations.  These deal with in-memory firmware images which
 * have headers in native byte order and may not include a trailing CRC.
 */
struct firmware_ops {
  /* Type and subtype of firmware image these operations handle. */
  unsigned type;
  unsigned subtype;
  /* Name of the firmware subtype for display to user. */
  const char* subtype_name_user;
  /* The default (built-in) firmware image. */
  const struct {
    const uint8_t* image_data; /* NULL if there is no default image. */
    const size_t image_size;            /* Zero if there is no default image. */
  } default_image;
  /* Does this type of firmware apply to device with given identifiers? */
  bool (*match_device)(const struct firmware_ops* ops,
                       const struct sfu_device* device);
  void* (*open)(const struct firmware_ops* ops, const struct sfu_device*);
  void (*close)(void* handle);
  /* Get the version from the currently installed firmware.
   * Return 0.0.0.0 if unknown. */
  struct firmware_version
  (*get_version)(void* handle, struct dynamic_cfg_context* dynamic_cfg);
  /* Read the currently installed firmware.  Return NULL on failure.
   * The image will later be freed with free().  Call the progress
   * function periodically with the status and percentage complete. */
  sfupdate_image_t* (*read_image)(void* handle,
                               void (*progress)(const char* status, int pc));
  /* Write new firmware.  Return 0 on success, non-zero on failure.
   * Call the progress function periodically with the status and percentage
   * complete. */
  int (*write_image)(void* handle, sfupdate_image_t* image, bool full_erase,
                     void (*progress)(const char* status, int pc));
  unsigned flags;
  /* Path to the image within a directory of images. */
  const char* image_path;
};

extern int init_image_from_buffer(const uint8_t *image_buffer,
                                  size_t image_buffer_len,
                                  sfupdate_image_t* image);
extern int init_ipxe_image_from_buffer(const uint8_t *image_buffer,
                                       size_t image_buffer_len,
                                       sfupdate_image_t* image);
extern int init_image_from_fw_ops(const struct firmware_ops* const fw_ops,
                                  sfupdate_image_t* image);

extern int cms_parse(const uint8_t* image_buffer, size_t image_buffer_len,
              uint32_t *content_offset_out, uint32_t *content_length_out);

extern void fw_report_verify_result(unsigned rc);

#define SFUPDATE_DEFAULT_IMAGE_INIT(data, size) \
  { .image_data = (data), .image_size = (size) }


/* Non-Dell images should appear in sfdupupdate but not sfupdate */
#if defined(SFUPDATE_WITH_DEFAULT_IMAGES) && !defined(SFDUPUPDATE_BUILD)
#define SFUPDATE_DEFINE_IMAGE(name) \
  const uint8_t name[] __attribute__((aligned(4)))

#define SFUPDATE_USE_IMAGE(name) SFUPDATE_DEFAULT_IMAGE_INIT(name, sizeof(name))
#define SFUPDATE_NO_IMAGE()      SFUPDATE_DEFAULT_IMAGE_INIT(NULL, 0)
#else
#define SFUPDATE_DEFINE_IMAGE(name) \
  const uint8_t name[] __attribute__((unused))

#define SFUPDATE_USE_IMAGE(name) SFUPDATE_DEFAULT_IMAGE_INIT(NULL, 0)
#define SFUPDATE_NO_IMAGE()      SFUPDATE_DEFAULT_IMAGE_INIT(NULL, 0)
#endif

/* Dell images should appear in both sfupdate and sfdupupdate */
#ifdef SFUPDATE_WITH_DEFAULT_IMAGES
#define SFUPDATE_DEFINE_DELL_IMAGE(name) \
  const uint8_t name[] __attribute__((aligned(4)))
#define SFUPDATE_USE_DELL_IMAGE(name) SFUPDATE_DEFAULT_IMAGE_INIT(name, sizeof(name))
#else
#define SFUPDATE_DEFINE_DELL_IMAGE(name) \
  const uint8_t name[] __attribute__((unused))
#define SFUPDATE_USE_DELL_IMAGE(name) SFUPDATE_DEFAULT_IMAGE_INIT(NULL, 0)
#endif

enum firmware_op_flags {
  /* Read or write requires interface to be down */
  firmware_rw_req_if_down = 1,
  /* Firmware is shared between all ports of a controller */
  firmware_shared = 4,
  /* MC should be reset after writing this firmware */
  firmware_w_req_mc_reset = 8,
  /* Wait until the platform is ready before performing operations */
  firmware_w_req_wait_platform = 32,
  /* AOE should be reset after writing this firmware */
  firmware_w_req_aoe_reset = 64,
  /* MUM should be reset after writing this firmware */
  firmware_w_req_mum_reset = 128,
  /* Only erase image-len bytes in the partition */
  firmware_erase_on_demand = 256,
};

/* Boot ROM firmware */
extern const struct firmware_ops gpxe_farmingdale_firmware_ops;
extern const struct firmware_ops gpxe_greenport_firmware_ops;
extern const struct firmware_ops gpxe_medford_firmware_ops;
extern const struct firmware_ops gpxe_medford2_firmware_ops;

/* MC firmware (Huntington) */
extern const struct firmware_ops mc_unobtanium_firmware_ops;
extern const struct firmware_ops mc_tuttle_firmware_ops;
extern const struct firmware_ops mc_kapteyn_firmware_ops;
extern const struct firmware_ops mc_whipple_firmware_ops;
extern const struct firmware_ops mc_forbes_firmware_ops;
extern const struct firmware_ops mc_longmore_firmware_ops;
extern const struct firmware_ops mc_herschel_firmware_ops;
extern const struct firmware_ops mc_shoemaker_firmware_ops;
extern const struct firmware_ops mc_ikeya_firmware_ops;
extern const struct firmware_ops mc_gehrels_firmware_ops;
extern const struct firmware_ops mc_kowalski_firmware_ops;

/* MC firmware (Medford) */
extern const struct firmware_ops mc_sparta_firmware_ops;
extern const struct firmware_ops mc_thebes_firmware_ops;
extern const struct firmware_ops mc_icarus_firmware_ops;
extern const struct firmware_ops mc_jericho_firmware_ops;
extern const struct firmware_ops mc_byblos_firmware_ops;
extern const struct firmware_ops mc_cyclops_firmware_ops;

/* MC firmware (Medford2) */
extern const struct firmware_ops mc_shilling_firmware_ops;
extern const struct firmware_ops mc_florin_firmware_ops;
extern const struct firmware_ops mc_bob_firmware_ops;
extern const struct firmware_ops mc_hog_firmware_ops;
extern const struct firmware_ops mc_sovereign_firmware_ops;
extern const struct firmware_ops mc_solidus_firmware_ops;
extern const struct firmware_ops mc_crown_firmware_ops;
extern const struct firmware_ops mc_sixpence_firmware_ops;
extern const struct firmware_ops mc_sol_firmware_ops;
extern const struct firmware_ops mc_threepence_firmware_ops;
extern const struct firmware_ops mc_joey_firmware_ops;
extern const struct firmware_ops mc_tanner_firmware_ops;
extern const struct firmware_ops mc_guinea_firmware_ops;

/* FC firmware */
extern const struct firmware_ops fc_sorrento_firmware_ops;

/* AOE FPGA bitfiles */
extern const struct firmware_ops fpga_shoemaker_firmware_ops;
extern const struct firmware_ops fpga_shoemaker_a7_firmware_ops;
extern const struct firmware_ops fpga_shoemaker_a5_firmware_ops;
extern const struct firmware_ops fpgadiag_shoemaker_firmware_ops;
extern const struct firmware_ops fpgadiag_shoemaker_a7_firmware_ops;
extern const struct firmware_ops fpgadiag_shoemaker_a5_firmware_ops;

/* MUM firmware */
extern const struct firmware_ops mum_madam_blue_firmware_ops;
extern const struct firmware_ops mum_icarus_firmware_ops;
extern const struct firmware_ops mum_jericho_firmware_ops;
extern const struct firmware_ops mum_byblos_firmware_ops;
extern const struct firmware_ops mum_cyclops_firmware_ops;
extern const struct firmware_ops mum_shilling_firmware_ops;
extern const struct firmware_ops mum_florin_firmware_ops;
extern const struct firmware_ops mum_bob_firmware_ops;
extern const struct firmware_ops mum_hog_firmware_ops;
extern const struct firmware_ops mum_sovereign_firmware_ops;
extern const struct firmware_ops mum_solidus_firmware_ops;
extern const struct firmware_ops mum_crown_firmware_ops;
extern const struct firmware_ops mum_sixpence_firmware_ops;
extern const struct firmware_ops mum_sol_firmware_ops;
extern const struct firmware_ops mum_threepence_firmware_ops;
extern const struct firmware_ops mum_joey_firmware_ops;
extern const struct firmware_ops mum_tanner_firmware_ops;
extern const struct firmware_ops mum_guinea_firmware_ops;

/* UEFI ROM */
extern const struct firmware_ops uefi_rom_firmware_ops;

/* Bundles */
extern const struct firmware_ops bundle_dell_x2522_25g_firmware_ops;
extern const struct firmware_ops bundle_dell_x2562_firmware_ops;
extern const struct firmware_ops bundle_x2552_firmware_ops;
extern const struct firmware_ops bundle_x2562_firmware_ops;
extern const struct firmware_ops bundle_u25_firmware_ops;

#define IPXE_ROM_HEADER 0xaa55
#define IPXE_VERSION_A 9
#define IPXE_VERSION_B 9
#define IPXE_VERSION_C 9
#define IPXE_VERSION_D 9999


#endif /* SFUTILS_FIRMWARE_H */
