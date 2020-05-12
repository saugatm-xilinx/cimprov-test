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
