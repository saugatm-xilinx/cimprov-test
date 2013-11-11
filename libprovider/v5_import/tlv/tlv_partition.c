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

/* This is a modified copy of src/tools/mc-comms/tlv.c.
 * Handlers for adding, deleting, modifying, etc. tags within a partition have
 * been added.
 * The v5 build system does not seem to allow us to conveniently share these
 * files.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "tlv_partition.h"
#include "endian_base.h"

#ifdef STANDALONE

#include <stdint.h>
#define TLV_TAG_END 0xEEEEEEEE

#else

#include <tlv_layout.h>
#endif

#include "mc_driver_pcol.h"


const char *partition_type_desc(uint16_t type)
{
  switch (type) {
  case NVRAM_PARTITION_TYPE_MC_FIRMWARE:
    return "MC firmware image";
  case NVRAM_PARTITION_TYPE_MC_FIRMWARE_BACKUP:
    return "Backup MC firmware image";
  case NVRAM_PARTITION_TYPE_EXPANSION_ROM:
    return "Expansion ROM image";
  case NVRAM_PARTITION_TYPE_STATIC_CONFIG:
    return "Static configuration";
  case NVRAM_PARTITION_TYPE_DYNAMIC_CONFIG:
    return "Dynamic configuration";
  case NVRAM_PARTITION_TYPE_EXPROM_CONFIG_PORT0:
    return "Exp ROM configuration (port 0)";
  case NVRAM_PARTITION_TYPE_EXPROM_CONFIG_PORT1:
    return "Exp ROM configuration (port 1)";
  case NVRAM_PARTITION_TYPE_EXPROM_CONFIG_PORT2:
    return "Exp ROM configuration (port 2)";
  case NVRAM_PARTITION_TYPE_EXPROM_CONFIG_PORT3:
    return "Exp ROM configuration (port 3)";
  case NVRAM_PARTITION_TYPE_LOG:
    return "Non-volatile log";
  case NVRAM_PARTITION_TYPE_DUMP:
    return "Device state dump storage";
  case NVRAM_PARTITION_TYPE_RECOVERY_MAP:
    return "Recovery partition map";
  case NVRAM_PARTITION_TYPE_PARTITION_MAP:
    return "Partition map";
  default:
    return "<unknown>";
  }
}

static void read_tlv_item(tlv_cursor_t *cursor, void *item_with_hdr,
                          size_t max_len)
{
  void *src;
  size_t len;

  src = tlv_item(cursor);
  assert(src != NULL);
  len = 8 + tlv_length(cursor);
  if (len > max_len)
    len = max_len;
  memcpy(item_with_hdr, src, len);
}

static uint32_t checksum_tlv_partition(nvram_partition_t *partition)
{
  tlv_cursor_t *cursor;
  uint32_t *ptr;
  uint32_t *end;
  uint32_t csum;
  size_t len;

  cursor = &partition->tlv_cursor;
  len = tlv_block_length_used(cursor);
  assert((len & 3) == 0);

  csum = 0;
  ptr = partition->data;
  end = &ptr[len >> 2];

  while (ptr < end)
    csum += le32_to_host(*ptr++);

  return csum;
}

extern int tlv_init_partition_from_buffer(nvram_partition_t *partition,
                                          void* buffer, size_t buffer_size)
{
  int rc;

  partition->data = buffer;
  partition->length = buffer_size;

  /* The partition starts with a partition header. */
  partition->type = le16_to_host(
      ((struct tlv_partition_header*)partition->data)->type_id);

  /* Initialise the partition cursor. */
  tlv_init_cursor(&(partition->tlv_cursor),
                  partition->data,
                  (uint32_t*)((uint8_t*)partition->data + partition->length));

  if ((rc = tlv_validate_partition(partition))) {
    memset(partition, 0, sizeof(*partition));
    return rc;
  }

  return 0;
}

int tlv_validate_partition(nvram_partition_t *partition)
{
  const char *desc;
  tlv_cursor_t *cursor;
  int rc;
  struct tlv_partition_header header;
  struct tlv_partition_trailer trailer;

  desc = partition_type_desc(partition->type);
  cursor = &partition->tlv_cursor;

  if (tlv_validate_state(cursor) != TLV_OK) {
    printf("(%s: TLV cursor in broken state initially)\n", desc);
    return 1;
  }

  tlv_rewind(cursor);
  if (tlv_tag(cursor) == TLV_TAG_INVALID) {
    printf("(%s: empty flash found at start of partition)\n", desc);
    return 1;
  }

  read_tlv_item(cursor, &header, sizeof(header));
  host_to_le32_inplace(&header.tag, 1);
  host_to_le32_inplace(&header.length, 1);
  host_to_le16_inplace(&header.type_id, 1);
  host_to_le16_inplace(&header.reserved, 1);
  host_to_le32_inplace(&header.generation, 1);
  host_to_le32_inplace(&header.total_length, 1);

  if (header.tag != TLV_TAG_PARTITION_HEADER) {
    printf("(%s: first tag %x is not the partition header magic tag %x)\n", desc, (unsigned)header.tag, TLV_TAG_PARTITION_HEADER);
    return 1;
  }
  if (header.length != (sizeof(header) - 8)) {
    printf("(%s: partition header tag is wrong size: got %u, but wanted %u)\n",
           desc, (unsigned)header.length, (unsigned int)(sizeof(header) - 8));
    return 1;
  }

  rc = tlv_find(cursor, TLV_TAG_PARTITION_TRAILER);

  if (rc == TLV_NOT_FOUND) {
    printf("(%s: no partition trailer found)\n", desc);
    return 1;
  }
  else if (rc != TLV_OK) {
    fprintf(stderr, "Unexpected error %d stripping partition trailer?\n", rc);
    return 1;
  }

  read_tlv_item(cursor, &trailer, sizeof(trailer));
  host_to_le32_inplace(&trailer.tag, 1);
  host_to_le32_inplace(&trailer.length, 1);
  host_to_le32_inplace(&trailer.generation, 1);
  host_to_le32_inplace(&trailer.checksum, 1);

  assert(trailer.tag == TLV_TAG_PARTITION_TRAILER);
  if (trailer.length != (sizeof(trailer) - 8)) {
    printf("(%s: partition trailer tag is wrong size: got %u, but wanted %u)\n",
           desc, (unsigned)trailer.length, (unsigned int)(sizeof(trailer) - 8));
    return 1;
  }

  tlv_advance(cursor);

  if (tlv_tag(cursor) != TLV_TAG_END) {
    printf("(%s: partition trailer is not the final tag)\n", desc);
    return 1;
  }

  /* Note that we may observe a generation of 0 here if the partition has been
   * reset but not yet synchronised to NVRAM.
   * See clear_dynamic_config_partition() and dynamic_cfg_sync().
   */
  if (header.generation != trailer.generation) {
    printf("(%s: header and trailer generation counts do not match)\n", desc);
    return 1;
  }

  if (header.total_length != tlv_block_length_used(cursor)) {
    printf("(%s: total length field in header does not match content)\n", desc);
    return 1;
  }

  if (checksum_tlv_partition(partition) != 0) {
    printf("(%s: checksum is incorrect)\n", desc);
    tlv_dump_all(cursor);
    return 1;
  }

  return 0;
}

int tlv_update_partition_len_and_cks(tlv_cursor_t *cursor)
{
  nvram_partition_t partition;
  struct tlv_partition_header *header;
  struct tlv_partition_trailer *trailer;
  size_t new_len;

  /* We just modified the partititon, so the total length may not be valid.
   * Don't use tlv_init_partition_from_buffer() or tlv_find(), which perform
   * some sanity checks that may fail here.
   */
  partition.data = cursor->block;
  memcpy(&partition.tlv_cursor, cursor, sizeof(*cursor));
  header = (struct tlv_partition_header*)partition.data;
  /* Sanity check. */
  if (le32_to_host(header->tag) != TLV_TAG_PARTITION_HEADER)
    return TLV_CORRUPT_BLOCK;
  new_len =  tlv_block_length_used(&partition.tlv_cursor);
  if (new_len == 0) {
    return TLV_CORRUPT_BLOCK;
  }
  header->total_length = host_to_le32(new_len);

  trailer = (struct tlv_partition_trailer*)
    ((uint8_t*)header + new_len - sizeof(*trailer) - sizeof(uint32_t));
  trailer->checksum = host_to_le32(
      le32_to_host(trailer->checksum) - checksum_tlv_partition(&partition));

  return 0;
}

int tlv_modify_in_partition(
    tlv_cursor_t *cursor, const uint8_t *data, size_t len)
{
  int rc;

  if ((rc = tlv_modify(cursor, data, len)))
    return rc;

  return tlv_update_partition_len_and_cks(cursor);
}

int tlv_delete_from_partition(tlv_cursor_t *cursor)
{
  int rc;

  if ((rc = tlv_delete(cursor)) ||
      (rc = tlv_update_partition_len_and_cks(cursor)))
    return rc;

  return TLV_OK;
}

int tlv_append_to_partition(nvram_partition_t *partition, uint32_t tag,
                            const uint8_t *data, size_t len)
{
  int rc;
  tlv_cursor_t *cursor = &partition->tlv_cursor;

  rc = tlv_require_end(cursor);
  if (rc)
    return rc;

  /* cursor->end should point to the current end tag at the end of the used
   * portion of the block.  We need space to write a new tag, new length,
   * and the padded data.
   */
  if (cursor->limit < (cursor->end + 1 + 1 + ((len + 3) / 4) + 1))
    return TLV_NO_SPACE;

  /* Insert the new tag before the end of the partition. */
  if ((rc = tlv_find(cursor, TLV_TAG_PARTITION_TRAILER)))
    return rc;
  if ((rc = tlv_insert(cursor, tag, data, len)))
    return rc;

  if ((rc = tlv_update_partition_len_and_cks(cursor)))
    return rc;

  return TLV_OK;
}

int tlv_update_or_append_to_partition(nvram_partition_t *partition,
                                      uint32_t tag,
                                      const uint8_t *data, size_t len)
{
  int rc;
  tlv_cursor_t *cursor = &partition->tlv_cursor;

  rc = tlv_find(cursor, tag);
  if (rc == TLV_OK) {
    if ((rc = tlv_modify(cursor, data, len)))
      return rc;
    return tlv_update_partition_len_and_cks(cursor);
  } else if (rc == TLV_NOT_FOUND) {
    return tlv_append_to_partition(partition, tag, data, len);
  } else {
    return rc;
  }
}

int tlv_increment_partition_generation(nvram_partition_t *partition)
{
  int rc;
  tlv_cursor_t *cursor = &partition->tlv_cursor;
  struct tlv_partition_header  *header;
  struct tlv_partition_trailer *trailer;

  if ((rc = tlv_validate_partition(partition)))
    return rc;

  header = (struct tlv_partition_header*)cursor->block;
  header->generation = host_to_le32(le32_to_host(header->generation) + 1);

  rc = tlv_find(cursor, TLV_TAG_PARTITION_TRAILER);
  assert(rc == TLV_OK);
  trailer = (struct tlv_partition_trailer*)cursor->current;
  trailer->generation = header->generation;

  rc = tlv_update_partition_len_and_cks(cursor);
  assert(rc == TLV_OK);

  return rc;
}

int tlv_init_dynamic_config_buf(char* _buf, unsigned buf_size)
{
  uint32_t *buf = (uint32_t*)_buf;
  int rc;
  tlv_cursor_t cursor;
  nvram_partition_t partition;
  struct tlv_partition_header header;
  struct tlv_partition_trailer trailer;

  unsigned min_buf_size =
    sizeof(struct tlv_partition_header) + sizeof(struct tlv_partition_trailer);
  if (buf_size < min_buf_size)
    return TLV_BAD_ARG;

  memset(buf, 0xff, buf_size);

  tlv_init_block(buf);
  tlv_init_cursor(&cursor, buf,
                  (uint32_t*)((uint8_t*)buf + buf_size));

  header.tag = host_to_le32(TLV_TAG_PARTITION_HEADER);
  header.length = host_to_le32(sizeof(header) - 8);
  header.type_id = host_to_le16(NVRAM_PARTITION_TYPE_DYNAMIC_CONFIG);
  header.reserved = 0;
  header.generation = host_to_le32(1);
  header.total_length = 0;  /* This will be fixed below. */
  write_tlv_item(&cursor, &header);

  trailer.tag = host_to_le32(TLV_TAG_PARTITION_TRAILER);
  trailer.length = host_to_le32(sizeof(trailer) - 8);
  trailer.generation = host_to_le32(header.generation);
  trailer.checksum = 0;  /* This will be fixed below. */
  write_tlv_item(&cursor, &trailer);

  tlv_update_partition_len_and_cks(&cursor);

  /* Check that the partition is valid. */
  rc = tlv_init_partition_from_buffer(&partition, buf, buf_size);

  return rc;
}
