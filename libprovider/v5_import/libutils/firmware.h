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

#if defined(__sun) && defined (__SVR4)
 #include <sys/byteorder.h>
 #if defined(_BIG_ENDIAN)
  #define FIRMWARE_HOST_IS_BIG_ENDIAN 1
 #else
  #define FIRMWARE_HOST_IS_BIG_ENDIAN 0
 #endif
#elif defined(__FreeBSD__)
 #include <sys/endian.h>
 #if (defined(_BYTE_ORDER) && (_BYTE_ORDER == _BIG_ENDIAN))
  #define FIRMWARE_HOST_IS_BIG_ENDIAN 1
 #else
  #define FIRMWARE_HOST_IS_BIG_ENDIAN 0
 #endif
#else
 #include <endian.h>
 #if (defined(__BYTE_ORDER) && (__BYTE_ORDER == __BIG_ENDIAN))
  #define FIRMWARE_HOST_IS_BIG_ENDIAN 1
 #else
  #define FIRMWARE_HOST_IS_BIG_ENDIAN 0
 #endif
#endif

#include "image.h"

#if FIRMWARE_HOST_IS_BIG_ENDIAN
#define FIRMWARE_U32(word) \
  word >> 24, (word >> 16) & 0xFF, (word >> 8) & 0xFF, word & 0xFF
#define FIRMWARE_U16(word) \
  (word >> 8) & 0xFF, word & 0xFF
#define FIRMWARE_U8(word) \
  word & 0xFF

#endif

struct firmware_version {
  uint16_t a, b, c, d;
};

struct sfu_device;
struct dynamic_cfg_context;

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
  /* The default (built-in) image; NULL if there is no default image. */
  const image_header_t* default_image;
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
  image_header_t* (*read_image)(void* handle,
                               void (*progress)(const char* status, int pc));
  /* Write new firmware.  Return 0 on success, non-zero on failure.
   * Call the progress function periodically with the status and percentage
   * complete. */
  int (*write_image)(void* handle, const image_header_t* image,
                     void (*progress)(const char* status, int pc));
  unsigned flags;
};

#ifdef SFUPDATE_WITH_DEFAULT_IMAGES
#define SFUPDATE_DEFINE_IMAGE(name) \
  const uint8_t name[] __attribute__((aligned(4)))
#define SFUPDATE_USE_IMAGE(name) ((const image_header_t *)name)
#else
#define SFUPDATE_DEFINE_IMAGE(name) \
  const uint8_t name[] __attribute__((unused))
#define SFUPDATE_USE_IMAGE(name) (NULL)
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
};

/* Boot ROM firmware */
extern const struct firmware_ops gpxe_falcon_firmware_ops;
extern const struct firmware_ops gpxe_bethpage_firmware_ops;
extern const struct firmware_ops gpxe_siena_firmware_ops;
extern const struct firmware_ops gpxe_farmingdale_firmware_ops;
extern const struct firmware_ops gpxe_greenport_firmware_ops;
extern const struct firmware_ops gpxe_medford_firmware_ops;
extern const image_header_t* gpxe_clear_firmware;

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

/* PHY firmware */
extern const struct firmware_ops sfx7101b_firmware_ops;
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


#endif /* SFUTILS_FIRMWARE_H */
