/**************************************************************************/
/*!  \file  dynamic_cfg.h
** \author  bwh
**  \brief  Dynamic config maintenance for Siena and later NICs
**   \date  2009/10/19
**    \cop  Copyright 2009 Solarflare Communications Inc.
*//************************************************************************/

#ifndef SFUTILS_DYNAMIC_CFG_H
#define SFUTILS_DYNAMIC_CFG_H

#include <stdint.h>
#include <errno.h>

#include "firmware.h"
#include "nv.h"

#define DYNAMIC_CFG_FAMILY_MAX 0xff

struct dynamic_cfg_context;

/* Do we have dynamic config access. Returns 0 on success
** or ENOTSUPP, in which case nv_dev may be non-NULL to
** indicate another device has access. */
extern int
dynamic_cfg_accessible(const struct sfu_device* dev,
                       const struct sfu_device** nv_dev);

/* Open and close dynamic config context */
extern struct dynamic_cfg_context*
dynamic_cfg_open(struct sfu_device* dev, enum nv_part_type partn_type);

extern void dynamic_cfg_close(struct dynamic_cfg_context* context);

/* Get version for given firmware type */
extern struct firmware_version
dynamic_cfg_get_version(const struct dynamic_cfg_context* context,
                        image_type_t type);
bool
bundle_meta_get_version(const struct sfu_device *dev, image_type_t image_type,
                        struct firmware_version *out_version);
/* Get family version string; return the actual string length or -1
 * if not found.  The string length is at most DYNAMIC_CFG_FAMILY_MAX. */
extern int dynamic_cfg_get_family(const struct dynamic_cfg_context* context,
                                  char* buf, size_t len);

/* Set subtype and version for given firmware type */
extern int
dynamic_cfg_set_version(struct dynamic_cfg_context* context, image_type_t type,
                        uint32_t subtype, struct firmware_version version,
                        bool set_subtype);

extern int dynamic_cfg_get_string(const struct dynamic_cfg_context* context,
                                  char key1, char key2,
                                  char* buf, size_t buf_len);
extern int dynamic_cfg_set_string(const struct dynamic_cfg_context* context,
                                  char key1, char key2,
                                  const char* version);

/* Set family version string.  The string length must be at most
 * DYNAMIC_CFG_FAMILY_MAX */
extern int dynamic_cfg_set_family(const struct sfu_device* dev,
                                  struct dynamic_cfg_context* context,
                                  const char* version);

extern int dynamic_cfg_set_oem_mcfw_version(
    const struct sfu_device* dev,
    const struct dynamic_cfg_context* context,
    const char* version);

extern int dynamic_cfg_set_bootrom_version(
    const struct sfu_device* dev,
    const struct dynamic_cfg_context* context,
    const char* version);
extern int dynamic_cfg_set_uefi_version(
    const struct sfu_device* dev,
    const struct dynamic_cfg_context* context,
    const char* version);
/* Commit changes to NVRAM */
extern int dynamic_cfg_sync(struct dynamic_cfg_context* context,
                            void (*progress)(const char* status, int pc));

extern int
dynamic_cfg_get_buf(struct dynamic_cfg_context* context,
                    void **buf, size_t *buf_size);

extern int dynamic_cfg_set_generation(void *data, size_t len,
                                      unsigned int generation);

extern void reset_dyn_vpd(const struct dynamic_cfg_context *context);

#endif /* SFUTILS_DYNAMIC_CFG_H */
