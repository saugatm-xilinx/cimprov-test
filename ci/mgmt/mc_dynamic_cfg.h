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

