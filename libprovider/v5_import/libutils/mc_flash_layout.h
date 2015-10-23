/**************************************************************************\ 
*//*! \file 
** <L5_PRIVATE L5_SOURCE> 
** \author gnb, mjs
**  \brief Flash layout for the Siena MC.
**   \date   2008/09/09
**    \cop  (c) Level 5 Networks Limited. 
** </L5_PRIVATE> 
*//* 
\**************************************************************************/

#ifndef MC_FLASH_LAYOUT_H
#define MC_FLASH_LAYOUT_H

/* Fixed locations near the start of flash (which may be in the internal PHY
 * firmware header) point to the boot header.
 *
 * - parsed by MC boot ROM and firmware
 * - reserved (but not parsed) by PHY firmware
 * - opaque to driver
 */

#define SIENA_MC_BOOT_PHY_FW_HDR_LEN (0x20)

#define SIENA_MC_BOOT_PTR_LOCATION (0x18)      /* First thing we try to boot */
#define SIENA_MC_BOOT_ALT_PTR_LOCATION (0x1c)  /* Alternative if that fails */


/* The boot header area is 512 bytes long and allows the main firmware image to
 * be located.
 *
 * - parsed by MC boot ROM and firmware
 * - opaque to driver (board type can be read via GET_BOARD_CFG MCDI command)
 */

#define SIENA_MC_BOOT_HDR_LEN (0x200)

#define SIENA_FC_BOOT_MAGIC (0xA0EFC001)
#define SIENA_MC_BOOT_MAGIC (0x51E4A001)
#define SIENA_MC_BOOT_VERSION (1)

#define SIENA_MUM_BOOT_MAGIC (0xA0EE3E01)



/*Structures supporting an arbitrary number of binary blobs in the flash image
  intended to house code and tables for the satellite cpus*/
/*thanks to random.org for:*/
#define BLOBS_HEADER_MAGIC (0xBDA3BBD4)
#define BLOB_HEADER_MAGIC  (0xA1478A91)

typedef struct blobs_hdr_s {
  uint32_t magic;
  uint32_t no_of_blobs;
} blobs_hdr_t;

typedef struct blob_hdr_s {
  uint32_t magic;
  uint32_t cpu_type;
  uint32_t build_variant;
  uint32_t offset;
  uint32_t length;
  uint32_t checksum;
} blob_hdr_t;

#define BLOB_CPU_TYPE_TXDI_TEXT (0)
#define BLOB_CPU_TYPE_RXDI_TEXT (1)
#define BLOB_CPU_TYPE_TXDP_TEXT (2)
#define BLOB_CPU_TYPE_RXDP_TEXT (3)
#define BLOB_CPU_TYPE_RXHRSL_HR_LUT (4)
#define BLOB_CPU_TYPE_RXHRSL_HR_LUT_CFG (5)
#define BLOB_CPU_TYPE_TXHRSL_HR_LUT (6)
#define BLOB_CPU_TYPE_TXHRSL_HR_LUT_CFG (7)
#define BLOB_CPU_TYPE_RXHRSL_HR_PGM  (8)
#define BLOB_CPU_TYPE_RXHRSL_SL_PGM  (9)
#define BLOB_CPU_TYPE_TXHRSL_HR_PGM  (10)
#define BLOB_CPU_TYPE_TXHRSL_SL_PGM  (11)
#define BLOB_CPU_TYPE_RXDI_VTBL0 (12)
#define BLOB_CPU_TYPE_TXDI_VTBL0 (13)
#define BLOB_CPU_TYPE_RXDI_VTBL1 (14)
#define BLOB_CPU_TYPE_TXDI_VTBL1 (15)
#define BLOB_CPU_TYPE_DUMPSPEC (32)
#define BLOB_CPU_TYPE_MC_XIP   (33)

#define BLOB_CPU_TYPE_INVALID (31)


typedef struct siena_mc_boot_hdr_s {
  uint32_t magic;                  /* = SIENA_MC_BOOT_MAGIC */
  uint16_t hdr_version;            /* this structure definition is version 1 */
  uint8_t  board_type;
  uint8_t  firmware_version_a;
  uint8_t  firmware_version_b;
  uint8_t  firmware_version_c;
  uint16_t checksum;               /* of whole header area + firmware image */
  uint16_t firmware_version_d;
  uint8_t  mcfw_subtype;
  uint8_t  generation;             /* Valid for medford, SBZ for earlier chips */
  uint32_t firmware_text_offset;   /* offset to firmware .text */
  uint32_t firmware_text_size;     /* length of firmware .text, in bytes */
  uint32_t firmware_data_offset;   /* offset to firmware .data */
  uint32_t firmware_data_size;     /* length of firmware .data, in bytes */
  uint8_t  spi_rate;               /* SPI rate for reading image, 0 is BootROM default */
  uint8_t  spi_phase_adj;          /* SPI SDO/SCL phase adjustment, 0 is default (no adj) */
  uint16_t xpm_sector;             /* The sector that contains the key, or 0xffff if unsigned (medford) SBZ (earlier) */
  uint32_t reserved_c[7];          /* (set to 0) */
} siena_mc_boot_hdr_t;

#define SIENA_MC_BOOT_HDR_PADDING \
  (SIENA_MC_BOOT_HDR_LEN - sizeof(siena_mc_boot_hdr_t))


/* Legacy boot header (version 0), in case anything wants to know about it.
 */

typedef struct siena_mc_boot_hdr_v0_s {
  uint32_t magic; //set to SIENA_MC_BOOT_MAGIC
  uint16_t hdr_version;
  uint8_t  board_type;
  uint8_t firmware_major_version;
  uint16_t firmware_minor_version;
  uint16_t checksum; //checksum covers whole 512-byte header and firmware image
  uint32_t flags; //currently only one flag defined "other SPI set"
  uint32_t firmware_text_offset; // Offset to firmware .text
  uint32_t firmware_text_size; // Code length of firmware, in bytes
  uint32_t firmware_data_offset; // Offset to firmware .data
  uint32_t firmware_data_size; // Initialised data length, in bytes
  uint32_t device_data_base; //if flag "other SPI set" then
                           //stored on different device
  uint32_t pxe_image_base;
  uint32_t iscsi_config_base; //if flag "other SPI set" then
                            //stored on different device
  uint32_t board_cfg_base;
  uint32_t reserved[4]; //set to 0
} siena_mc_boot_hdr_v0_t;

#define SIENA_MC_BOOT_HDR_V0_PADDING \
  (SIENA_MC_BOOT_HDR_LEN - sizeof(siena_mc_boot_hdr_v0_t))


/* Static configuration header, which lives at the start of the per-port static
 * config erase sector.
 *
 * - parsed by MC firmware
 * - driver may parse static_vpd_offset to find VPD within sector
 * - remaining fields opaque to driver (fields necessary for driver can be read
 *   via GET_BOARD_CFG MCDI command)
 */

#define SIENA_MC_STATIC_CONFIG_MAGIC (0xBDCF5555)
#define SIENA_MC_STATIC_CONFIG_VERSION (0)

/* This is the maximum length of the header area (including all the DBI items,
 * but *not* including VPD).  Increasing this increases DMEM usage: a cache is
 * used so that we can verify the checksum and then apply the DBI config if it
 * is OK, without reading anything more than once (DBI config must happen as
 * rapidly as possible).
 */
#define SIENA_MC_STATIC_CONFIG_MAX_LENGTH (0x100)

typedef struct siena_mc_static_config_hdr_s {
  uint32_t magic;               /* = SIENA_MC_STATIC_CONFIG_MAGIC */
  uint16_t length;              /* of header area (i.e. not including VPD) */
  uint8_t  version;
  uint8_t  csum;                /* over header area (i.e. not including VPD) */
  uint32_t static_vpd_offset;
  uint32_t static_vpd_length;
  uint32_t capabilities;
  uint8_t  mac_addr_base[6];
  uint8_t  green_mode_cal;    /* Green mode calibration result */
  uint8_t  green_mode_valid;  /* Whether cal holds a valid value */
  uint16_t mac_addr_count;
  uint16_t mac_addr_stride;
  uint16_t calibrated_vref; /* Vref as measured during production */
  uint16_t adc_vref;        /* Vref as read by ADC */
  uint32_t reserved2[1];        /* (write as zero) */
  uint32_t num_dbi_items;
  struct {
    uint16_t addr;
    uint16_t byte_enables;
    uint32_t value;
  } dbi[];
} siena_mc_static_config_hdr_t;


/* The dynamic configuration layout has been moved to a separate header; it is
 * of interest to some user tools.
 */
#include <mc_dynamic_cfg.h>


/* Legacy board config header (version 2), in case anything wants to know about
 * it.
 */

#define SIENA_MC_BOARD_CFG_HDR_V2_MAGIC (0xBDCFBDCF)
#define SIENA_MC_BOARD_CFG_HDR_V2_VERSION (0x02)

typedef struct siena_mc_board_cfg_hdr_v2_s {
  uint32_t magic;
  uint16_t length;
  uint8_t  version;
  uint8_t  csum; /* 0 for don't care */
  uint8_t  mac_addr_0[6];
  uint8_t  pad_0[2];
  uint8_t  mac_addr_1[6];
  uint8_t  pad_1[2];
  uint8_t  mac_addr_2[6];
  uint8_t  pad_2[2];
  uint8_t  mac_addr_3[6];
  uint8_t  pad_3[2];
  uint8_t  phy0_type;
  uint8_t  phy0_mdio_bus;
  uint8_t  phy0_devaddr;
  uint8_t  phy1_type;
  uint8_t  phy1_mdio_bus;
  uint8_t  phy1_devaddr;
} siena_mc_board_cfg_hdr_v2_t;


/* Expansion ROM headers (for both plain ROM and magic combined ROM).
 *
 * Combined ROM header is:
 *
 * - parsed by MC firmware
 * - opaque to driver
 */

#define SIENA_MC_EXPROM_SINGLE_MAGIC (0xAA55)  /* little-endian uint16_t */

#define SIENA_MC_EXPROM_COMBO_MAGIC (0xB0070102)  /* little-endian uint32_t */
#define SIENA_MC_EXPROM_COMBO_V2_MAGIC (0xB0070103)  /* little-endian uint32_t */

typedef struct siena_mc_combo_rom_hdr_s {
  uint32_t magic;         /* = SIENA_MC_EXPROM_COMBO_MAGIC or SIENA_MC_EXPROM_COMBO_V2_MAGIC */
  union {
    struct {
      uint32_t len1;          /* length of first image */
      uint32_t len2;          /* length of second image */
      uint32_t off1;          /* offset of first byte to edit to combine images */
      uint32_t off2;          /* offset of second byte to edit to combine images */
      uint16_t infoblk0_off;  /* infoblk offset */
      uint16_t infoblk1_off;  /* infoblk offset */
      uint8_t  infoblk_len;   /* length of space reserved for one infoblk structure */
      uint8_t  reserved[7];   /* (set to 0) */
    } v1;
    struct {
      uint32_t len1;          /* length of first image */
      uint32_t len2;          /* length of second image */
      uint32_t off1;          /* offset of first byte to edit to combine images */
      uint32_t off2;          /* offset of second byte to edit to combine images */
      uint16_t infoblk_off;   /* infoblk start offset */
      uint16_t infoblk_count; /* infoblk count  */
      uint8_t  infoblk_len;   /* length of space reserved for one infoblk structure */
      uint8_t  reserved[7];   /* (set to 0) */
    } v2;
  } data;
} siena_mc_combo_rom_hdr_t;

#define SIENA_MC_EXPROM_INFOBLK_MAGIC (0xB0079006)

typedef struct siena_mc_combo_rom_infoblk_s {
  uint32_t magic;         /* = SIENA_MC_EXPROM_INFOBLK_MAGIC */
  uint8_t length;         /* = total length of infoblock */
  uint8_t version;
  uint8_t csum;
  uint8_t pad;
  uint8_t mac_addr[8];    /* Mac address (aligned) */
  uint8_t boot_type;
  uint8_t mpio_priority;
  uint8_t banner_delay;
  uint8_t bootskip_delay;
} siena_mc_combo_rom_infoblk_t;

/* gPXE configuration area.
 *
 * A bytes-sum-to-zero checksum applies across the whole area, so we need to
 * know how long it is (it's not necessarily the same length as the flash
 * partition allocated to it).
 */

#define SIENA_MC_EXPROM_CONFIG_LENGTH (0x1000)


/* External PHY firmware MDIO boot image header.
 *
 * - parsed by MC firmware
 * - of no relevance to driver
 */

#define SIENA_MC_MDIO_BOOT_MAGIC (0x3D10B007)  /* little-endian uint32_t */

/* Legacy (version 0) MDIO boot header
 */
typedef struct siena_mc_mdio_boot_hdr_v0_s {
  uint32_t magic;         /* = SIENA_MC_MDIO_BOOT_MAGIC */
  uint16_t version;       /* version of MDIO boot image format (this is v0) */
  uint8_t  csum;          /* checksum over whole boot image */
  uint8_t  reserved_a;    /* (set to 0) */
  uint32_t length;        /* length of boot image (including this header) */
  uint8_t  board_type;    /* type of board this image is for */
  uint8_t  phy_type;      /* type of PHY this image is for */
  uint16_t reserved_b;    /* (set to 0) */
} siena_mc_mdio_boot_hdr_v0_t;

/* Current (version 1) MDIO boot header
 */
typedef struct siena_mc_mdio_boot_hdr_s {
  /* Initial portion is compatible with v0 header: */
  uint32_t magic;          /* = SIENA_MC_MDIO_BOOT_MAGIC */
  uint16_t version;        /* version of MDIO boot image format (this is v1) */
  uint8_t  csum;           /* checksum over whole boot image */
  uint8_t  reserved_a;     /* (set to 0) */
  uint32_t length;         /* length of boot image (including this header) */
  uint8_t  board_type;     /* type of board this image is for */
  uint8_t  phy_type;       /* type of PHY this image is for */
  uint16_t reserved_b;     /* (set to 0) */
  /* New fields added for v1 header: */
  uint32_t fwver_code;     /* f/w version code (may be used by MC driver) */
  char     fwver_desc[12]; /* f/w version description (for GET_PHY_CFG) */
} siena_mc_mdio_boot_hdr_t;


/* Callisto PHY firmware image header.
 *
 * - parsed by MC firmware (for image validation only)
 * - of no relevance to driver
 */
typedef struct siena_mc_callisto_hdr_s {
  uint32_t code_len;       /* code length in bytes (starts at 0x20) */
  uint32_t dest_addr;      /* destination address */
  uint16_t app_csum;       /* application checksum (over code area) */
  uint16_t boot_cfg;       /* boot config settings */
  uint16_t hdr_csum;       /* header checksum (over preceding bytes) */
  uint16_t reserved_a;     /* reserved for future use */
  uint32_t reserved_b[2];  /* reserved for future use */
  uint32_t boot_ptr;       /* reserved for use as MC f/w boot pointer */
  uint32_t alt_boot_ptr;   /* reserved for use as MC f/w alt. boot pointer */
} siena_mc_callisto_hdr_t;


#endif

