/**************************************************************************\
*//*! \file
** <L5_PRIVATE L5_SOURCE>
** \author  mjs
**  \brief  TLV manipulation
**   \date  2012/11/20
**    \cop  (c) Solarflare Communications Inc.
** </L5_PRIVATE>
*//*
\**************************************************************************/

#ifndef TLV_PARTITION_H
#define TLV_PARTITION_H

#include "tlv.h"

typedef struct nvram_partition_s {
  uint16_t type;
  uint8_t chip_select;
  uint8_t flags;
  /* The full length of the NVRAM partition.
   * This is different from tlv_partition_header.total_length, which can be
   * smaller.
   */
  uint32_t length;
  uint32_t erase_size;
  uint32_t *data;
  tlv_cursor_t tlv_cursor;
} nvram_partition_t;


extern const char *partition_type_desc(uint16_t type);

/* NVRAM partition helpers. */
extern int tlv_init_partition_from_buffer(nvram_partition_t *partition,
                                          void* buffer, size_t buffer_size);
extern int tlv_validate_partition(nvram_partition_t *partition);

/* Operations on tags within the partition. */

extern int tlv_update_partition_len_and_cks(tlv_cursor_t *cursor);

extern int tlv_modify_in_partition(
    tlv_cursor_t *cursor, const uint8_t *data, size_t len);

extern int tlv_delete_from_partition(tlv_cursor_t *cursor);

extern int tlv_append_to_partition(nvram_partition_t *partition, uint32_t tag,
                                   const uint8_t *data, size_t len);

extern int tlv_update_or_append_to_partition(
    nvram_partition_t *partition, uint32_t tag,
    const uint8_t *data, size_t len);

extern int tlv_increment_partition_generation(nvram_partition_t *partition);

/* Initialize a buffer with an empty dynamic config. */
int tlv_init_dynamic_config_buf(char* buf, unsigned buf_size);

#endif /* TLV_PARTITION_H */

