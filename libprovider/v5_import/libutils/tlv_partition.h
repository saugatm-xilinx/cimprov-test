/*
 * Copyright 2012 - 2019 Xilinx, Inc.
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

extern uint32_t tlv_get_partition_generation(void *data);
extern int tlv_set_partition_generation(nvram_partition_t *partition, uint32_t generation);
extern int tlv_increment_partition_generation(nvram_partition_t *partition);

extern int tlv_zero_partition_preset(nvram_partition_t *partition);

/* Initialize a buffer with an empty dynamic config. */
int tlv_init_dynamic_config_buf(char* buf, unsigned buf_size);

#endif /* TLV_PARTITION_H */

