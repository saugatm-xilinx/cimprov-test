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

typedef uint32_t image_trailer_t;

typedef struct sfupdate_image_s {
  image_format_t format;
  const uint8_t* raw_image;

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


#ifdef SFUPDATE_WITH_DEFAULT_IMAGES
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


enum firmware_op_flags {
  /* Read or write requires interface to be down */
  firmware_rw_req_if_down = 1,
  /* Read or write requires phy_flash_cfg attribute to be set */
  firmware_rw_req_phy_flash_cfg = 2,
  /* Firmware is shared between all ports of a controller */
  firmware_shared = 4,
  /* MC should be reset after writing this firmware */
  firmware_w_req_mc_reset = 8,
  /* MC should reprogram CPLD after writing this firmware */
  firmware_w_req_cpld_program = 16,
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
extern const struct firmware_ops gpxe_bethpage_firmware_ops;
extern const struct firmware_ops gpxe_siena_firmware_ops;
extern const struct firmware_ops gpxe_farmingdale_firmware_ops;
extern const struct firmware_ops gpxe_greenport_firmware_ops;
extern const struct firmware_ops gpxe_medford_firmware_ops;
extern const struct firmware_ops gpxe_medford2_firmware_ops;

/* MC firmware (Siena) */
extern const struct firmware_ops mc_florence_firmware_ops;
extern const struct firmware_ops mc_zebedee_firmware_ops;
extern const struct firmware_ops mc_mr_rusty_firmware_ops;
extern const struct firmware_ops mc_ermintrude_firmware_ops;
extern const struct firmware_ops mc_buxton_firmware_ops;
extern const struct firmware_ops mc_brian_firmware_ops;
extern const struct firmware_ops mc_mr_mchenry_firmware_ops;
extern const struct firmware_ops mc_uncle_hamish_firmware_ops;
extern const struct firmware_ops mc_dylan_firmware_ops;

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

/* PHY firmware */
extern const struct firmware_ops qt2025c_firmware_ops;
extern const struct firmware_ops qt2025_kr_firmware_ops;
extern const struct firmware_ops calip_firmware_ops;
extern const struct firmware_ops siena_sft9001b_firmware_ops;

/* FC firmware */
extern const struct firmware_ops fc_modena_firmware_ops;
extern const struct firmware_ops fc_sorrento_firmware_ops;

/* FPGA bitfiles */
extern const struct firmware_ops fpga_ptp_firmware_ops;
extern const struct firmware_ops cpld_sfa6902_firmware_ops;
extern const struct firmware_ops fpga_mr_mchenry_firmware_ops;

/* AOE FPGA bitfiles */
extern const struct firmware_ops fpga_uncle_hamish_firmware_ops;
extern const struct firmware_ops fpga_uncle_hamish_a7_firmware_ops;
extern const struct firmware_ops fpga_uncle_hamish_a5_firmware_ops;
extern const struct firmware_ops fpgadiag_uncle_hamish_firmware_ops;
extern const struct firmware_ops fpgadiag_uncle_hamish_a7_firmware_ops;
extern const struct firmware_ops fpgadiag_uncle_hamish_a5_firmware_ops;
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


/* UEFI ROM */
extern const struct firmware_ops uefi_rom_firmware_ops;

#define IPXE_ROM_HEADER 0xaa55
#define IPXE_VERSION_A 9
#define IPXE_VERSION_B 9
#define IPXE_VERSION_C 9
#define IPXE_VERSION_D 9999


#endif /* SFUTILS_FIRMWARE_H */
