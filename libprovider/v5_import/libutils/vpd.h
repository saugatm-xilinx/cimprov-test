/**************************************************************************/
/*!  \file  vpd.h
** \author  bwh
**  \brief  Functions for manipulating Vital Product Data in memory
**   \date  2009/10/19
**    \cop  Copyright 2009 Solarflare Communications Inc.
*//************************************************************************/

#ifndef SFUTILS_VPD_H
#define SFUTILS_VPD_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Initial content of dynamic VPD */
extern const uint8_t vpd_dynamic_init[];

/* Test whether VPD is valid.  This only accepts a single VPD-R tag
 * followed by an end tag. */
extern bool vpd_is_valid(const uint8_t* vpd, size_t len);

/* The following functions generally assume the VPD is valid */

/* Get the length (in bytes) of a VPD tags chain. */
extern size_t vpd_used_len(const uint8_t* vpd);
/* Find keyword/value; return starting offset or 0 if not found */
extern size_t vpd_find(const uint8_t* vpd, char key1, char key2);
/* Delete keyword/value if present */
extern void vpd_delete(uint8_t* vpd, char key1, char key2);
/* Set keyword/value */
extern int vpd_set(uint8_t* vpd, size_t len, char key1, char key2,
                   const uint8_t* value, size_t value_len);

#endif /* SFUTILS_VPD_H */
