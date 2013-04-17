/**************************************************************************\ 
*//*! \file 
** <L5_PRIVATE L5_SOURCE> 
** \author  mjs
**  \brief  TLV item layouts for EF10 static and dynamic config in NVRAM
**   \date  2012/11/20
**    \cop  (c) Solarflare Communications Inc.
** </L5_PRIVATE> 
*//* 
\**************************************************************************/

/* These structures define the layouts for the TLV items stored in static and
 * dynamic configuration partitions in NVRAM for EF10 (Huntington etc.).
 *
 * They contain the same sort of information that was kept in the
 * siena_mc_static_config_hdr_t and siena_mc_dynamic_config_hdr_t structures
 * (defined in <ci/mgmt/mc_flash_layout.h> and <ci/mgmt/mc_dynamic_cfg.h>) for
 * Siena.
 *
 * These are used directly by the MC and should also be usable directly on host
 * systems which are little-endian and do not do strange things with structure
 * padding.  (Big-endian host systems will require some byte-swapping.)
 *
 *                                    -----
 *
 * Please refer to SF-108797-SW for a general overview of the TLV partition
 * format.
 *
 *                                    -----
 *
 * The current tag IDs have a general structure: with the exception of the
 * special values defined in the document, they are of the form 0xLTTTNNNN,
 * where:
 *
 *   -  L is a location, indicating where this tag is expected to be found:
 *      0 for static configuration, or 1 for dynamic configuration.   Other
 *      values are reserved.
 *
 *   -  TTT is a type, which is just a unique value.  The same type value
 *      might appear in both locations, indicating a relationship between
 *      the items (e.g. static and dynamic VPD below).
 *
 *   -  NNNN is an index of some form.  Some item types are per-port, some
 *      are per-PF, some are per-partition-type.
 *
 *                                    -----
 *
 * As with the previous Siena structures, each structure here is laid out
 * carefully: values are aligned to their natural boundary, with explicit
 * padding fields added where necessary.  (No, technically this does not
 * absolutely guarantee portability.  But, in practice, compilers are generally
 * sensible enough not to introduce completely pointless padding, and it works
 * well enough.)
 */


#ifndef CI_MGMT_TLV_LAYOUT_H
#define CI_MGMT_TLV_LAYOUT_H


/* ----------------------------------------------------------------------------
 *  General structure (defined by SF-108797-SW)
 * ----------------------------------------------------------------------------
 */


/* The "end" tag.
 *
 * (Note that this is *not* followed by length or value fields: anything after
 * the tag itself is irrelevant.)
 */

#define TLV_TAG_END                     (0xEEEEEEEE)


/* Other special reserved tag values.
 */

#define TLV_TAG_SKIP                    (0x00000000)
#define TLV_TAG_INVALID                 (0xFFFFFFFF)


/* TLV partition header.
 *
 * In a TLV partition, this must be the first item in the sequence, at offset
 * 0.
 */

#define TLV_TAG_PARTITION_HEADER        (0xEF10DA7A)

struct tlv_partition_header {
  uint32_t tag;
  uint32_t length;
  uint16_t type_id;
  uint16_t reserved;
  uint32_t generation;
  uint32_t total_length;
};


/* TLV partition trailer.
 *
 * In a TLV partition, this must be the last item in the sequence, immediately
 * preceding the TLV_TAG_END word.
 */

#define TLV_TAG_PARTITION_TRAILER       (0xEF101A57)

struct tlv_partition_trailer {
  uint32_t tag;
  uint32_t length;
  uint32_t generation;
  uint32_t checksum;
};


/* ----------------------------------------------------------------------------
 *  Configuration items
 * ----------------------------------------------------------------------------
 */


/* NIC global capabilities.
 */

#define TLV_TAG_GLOBAL_CAPABILITIES     (0x00010000)

struct tlv_global_capabilities {
  uint32_t tag;
  uint32_t length;
  uint32_t flags;
};


/* Per-port MAC address allocation.
 *
 * There are <count> addresses, starting at <base_address> and incrementing
 * by adding <stride> to the low-order byte(s).
 */

#define TLV_TAG_PORT_MAC(port)          (0x00020000 + (port))

struct tlv_port_mac {
  uint32_t tag;
  uint32_t length;
  uint8_t  base_address[6];
  uint16_t reserved;
  uint16_t count;
  uint16_t stride;
};


/* Static VPD.
 *
 * This is the portion of VPD which is set at manufacturing time and not
 * expected to change.  It is formatted as a standard PCI VPD block.
 */

#define TLV_TAG_PF_STATIC_VPD(pf)       (0x00030000 + (pf))

struct tlv_pf_static_vpd {
  uint32_t tag;
  uint32_t length;
  uint8_t  bytes[];
};


/* Dynamic VPD.
 *
 * This is the portion of VPD which may be changed (e.g. by firmware updates).
 * It is formatted as a standard PCI VPD block.
 */

#define TLV_TAG_PF_DYNAMIC_VPD(pf)      (0x10030000 + (pf))

struct tlv_pf_dynamic_vpd {
  uint32_t tag;
  uint32_t length;
  uint8_t  bytes[];
};


/* "DBI" PCI config space changes.
 *
 * This is a set of edits made to the default PCI config space values before
 * the device is allowed to enumerate.
 */

#define TLV_TAG_PF_DBI(pf)              (0x00040000 + (pf))

struct tlv_pf_dbi {
  uint32_t tag;
  uint32_t length;
  struct {
    uint16_t addr;
    uint16_t byte_enables;
    uint32_t value;
  } items[];
};


/* Partition subtype codes.
 *
 * A subtype may optionally be stored for each type of partition present in
 * the NVRAM.  For example, this may be used to allow a generic firmware update
 * utility to select a specific variant of firmware for a specific variant of
 * board.
 *
 * The description[] field is an optional string which is returned in the
 * MC_CMD_NVRAM_METADATA response if present.
 */

#define TLV_TAG_PARTITION_SUBTYPE(type) (0x00050000 + (type))

struct tlv_partition_subtype {
  uint32_t tag;
  uint32_t length;
  uint32_t subtype;
  uint8_t  description[];
};


/* Partition version codes.
 *
 * A version may optionally be stored for each type of partition present in
 * the NVRAM.  This provides a standard way of tracking the currently stored
 * version of each of the various component images.
 */

#define TLV_TAG_PARTITION_VERSION(type) (0x10060000 + (type))

struct tlv_partition_version {
  uint32_t tag;
  uint32_t length;
  uint16_t version_w;
  uint16_t version_x;
  uint16_t version_y;
  uint16_t version_z;
};

/* Global PCIe configuration */

#define TLV_TAG_GLOBAL_PCIE_CONFIG (0x10070000)

struct tlv_pcie_config {
  uint32_t tag;
  uint32_t length;
  int16_t max_pf_number;                        /**< Largest PF RID (lower PFs may be hidden) */
  uint16_t pf_aper;                             /**< BIU aperture for PF BAR2 */
  uint16_t vf_aper;                             /**< BIU aperture for VF BAR0 */
  uint16_t int_aper;                            /**< BIU aperture for PF BAR4 and VF BAR2 */  
};

/* Per-PF configuration. Note that not all these fields are necessarily useful
 * as the apertures are constrained by the BIU settings (the one case we do
 * use is to make BAR2 bigger than the BIU thinks to reserve space), but we can
 * tidy things up later */

#define TLV_TAG_PF_PCIE_CONFIG(pf)  (0x10080000 + (pf))

struct tlv_per_pf_pcie_config {
  uint32_t tag;
  uint32_t length;
  uint8_t vfs_total;
  uint8_t port_allocation;  
  uint16_t vectors_per_pf;
  uint16_t vectors_per_vf;
  uint8_t pf_bar0_aperture;
  uint8_t pf_bar2_aperture;
  uint8_t vf_bar0_aperture;
  uint8_t vf_base;  
  uint16_t supp_pagesz;
  uint16_t msix_vec_base;
};


/* Development ONLY. This is a single TLV tag for all the gubbins
 * that can be set through the MC command-line other than the PCIe
 * settings. This is a temporary measure. */
#define TLV_TAG_TMP_GUBBINS (0x10090000)

struct tlv_tmp_gubbins {
  uint32_t tag;
  uint32_t length;
  /* Consumed by dpcpu.c */
  uint64_t tx0_tags;     /* Bitmap */
  uint64_t tx1_tags;     /* Bitmap */
  uint64_t dl_tags;      /* Bitmap */
  uint32_t flags;
#define TLV_DPCPU_TX_STRIPE (1) /* TX striping is on */
#define TLV_DPCPU_BIU_TAGS  (2) /* Use BIU tag manager */
#define TLV_DPCPU_TX0_TAGS  (4) /* tx0_tags is valid */
#define TLV_DPCPU_TX1_TAGS  (8) /* tx1_tags is valid */
#define TLV_DPCPU_DL_TAGS  (16) /* dl_tags is valid */
  /* Consumed by features.c */
  uint32_t dut_features;        /* All 1s -> leave alone */
  int8_t with_rmon;             /* 0 -> off, 1 -> on, -1 -> leave alone */
  /* Consumed by clocks_hunt.c */
  int8_t clk_mode;             /* 0 -> off, 1 -> on, -1 -> leave alone */
  /* Consumed by sram.c */
  int8_t rx_dc_size;           /* -1 -> leave alone */
  int8_t tx_dc_size;
  int16_t num_q_allocs;
};

/* Global port configuration */
#define TLV_TAG_GLOBAL_PORT_CONFIG      (0x000a0000)

struct tlv_global_port_config {
  uint32_t tag;
  uint32_t length;
  uint32_t ports_per_core;
  uint32_t max_port_speed;
};


/* Firmware options.
 *
 * This is intended for user-configurable selection of optional firmware
 * features and variants.
 *
 * Initially, this consists only of the satellite CPU firmware variant
 * selection, but this tag could be extended in the future (using the
 * tag length to determine whether additional fields are present).
 */

#define TLV_TAG_FIRMWARE_OPTIONS        (0x100b0000)

struct tlv_firmware_options {
  uint32_t tag;
  uint32_t length;
  uint32_t firmware_variant;
#define TLV_FIRMARE_VARIANT_DEFAULT      (0)
#define TLV_FIRMARE_VARIANT_LOW_LATENCY  (1)
};

/* Voltage settings
 * 
 * Intended for boards with A0 silicon where the core voltage may
 * need tweaking. Most likely set once when the pass voltage is 
 * determined. */

#define TLV_TAG_0V9_SETTINGS (0x000c0000)

struct tlv_0v9_settings {
  uint32_t tag;
  uint32_t length;  
  uint16_t flags; /* Boards with high 0v9 settings may need active cooling */
#define TLV_TAG_0V9_REQUIRES_FAN (1)
  uint16_t target_voltage; /* In millivolts */
  /* Since the limits are meant to be centred to the target (and must at least
   * contain it) they need setting as well. */
  uint16_t warn_low;       /* In millivolts */
  uint16_t warn_high;      /* In millivolts */
  uint16_t panic_low;      /* In millivolts */
  uint16_t panic_high;     /* In millivolts */   
};


/* Clock configuration */

#define TLV_TAG_CLOCK_CONFIG            (0x000d0000)

struct tlv_clock_config {
  uint32_t tag;
  uint32_t length;  
  uint16_t clk_sys;        /* MHz */
  uint16_t clk_dpcpu;      /* MHz */
  uint16_t clk_icore;      /* MHz */
  uint16_t clk_pcs;        /* MHz */
};


#endif /* CI_MGMT_TLV_LAYOUT_H */

