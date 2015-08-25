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

#include "tlv.h"
#include "endian_base.h"
#include "mc_driver_pcol.h"

#ifdef STANDALONE

#include <stdint.h>
#define TLV_TAG_END 0xEEEEEEEE

#else

#include <tlv_layout.h>
#endif


static unsigned int tlv_n_words_for_len(size_t len)
{
  /* An item is a tag, length in bytes, and data padded to a 32-bit boundary */
  return 1 + 1 + ((len + 3) / 4);
}

static unsigned int tlv_n_words_for_item(uint32_t *item)
{
  return tlv_n_words_for_len((size_t)host_to_le32(item[1]));
}

static uint32_t *tlv_next_item_ptr(uint32_t *item)
{
  return item + tlv_n_words_for_item(item);
}

void tlv_init_block(uint32_t *block)
{
  *block = host_to_le32(TLV_TAG_END);
}

uint32_t tlv_tag(tlv_cursor_t *cursor)
{
  return le32_to_host(cursor->current[0]);
}

size_t tlv_length(tlv_cursor_t *cursor)
{
  return (cursor->current[0] == host_to_le32(TLV_TAG_END))
           ? 0U : (size_t)le32_to_host(cursor->current[1]);
}

uint8_t *tlv_value(tlv_cursor_t *cursor)
{
  return (cursor->current[0] == host_to_le32(TLV_TAG_END))
           ? NULL : (uint8_t *)&cursor->current[2];
}

uint8_t *tlv_item(tlv_cursor_t *cursor)
{
  return (cursor->current[0] == host_to_le32(TLV_TAG_END))
           ? NULL : (uint8_t *)cursor->current;
}

int tlv_validate_state(tlv_cursor_t *cursor)
{
  /* Check cursor position */
  if ((cursor->current < cursor->block) || (cursor->current >= cursor->limit)) {
    cursor->current = NULL;
    return TLV_BAD_ARG;
  }

  /* The end tag need not be followed by anything else */
  if (cursor->current[0] == host_to_le32(TLV_TAG_END)) {
    cursor->end = cursor->current;
    return TLV_OK;
  }

  /* Otherwise we need at least a tag and length to read the length */
  if (cursor->current > (cursor->limit - 2)) {
    cursor->current = NULL;
    return TLV_CORRUPT_BLOCK;
  }

  /* And we must have the specified amount of data and at least another tag */
  if (tlv_next_item_ptr(cursor->current) > (cursor->limit - 1)) {
    cursor->current = NULL;
    return TLV_CORRUPT_BLOCK;
  }

  return TLV_OK;
}

int tlv_init_cursor(tlv_cursor_t *cursor, uint32_t *block, uint32_t *limit)
{
  cursor->block = block;
  cursor->current = block;
  cursor->end = NULL;
  cursor->limit = limit;
  return tlv_validate_state(cursor);
}


int tlv_init_cursor_from_size(tlv_cursor_t *cursor, uint8_t *block, size_t size)
{
  uint32_t *limit;
  limit = (uint32_t *)(block + size - sizeof(uint32_t));
  return (tlv_init_cursor(cursor, (uint32_t *)block, limit));
}


int tlv_rewind(tlv_cursor_t *cursor)
{
  cursor->current = cursor->block;
  return tlv_validate_state(cursor);
}

int tlv_advance(tlv_cursor_t *cursor)
{
  int rc;

  /* Check the item at the cursor */
  rc = tlv_validate_state(cursor);
  if (rc != 0)
    return rc;

  /* If we were at the end tag, we've finished now */
  if (cursor->current == cursor->end) {
    cursor->current = NULL;
    return TLV_NOT_FOUND;
  }

  /* Advance and check the new item */
  cursor->current = tlv_next_item_ptr(cursor->current);
  return tlv_validate_state(cursor);
}

int tlv_find(tlv_cursor_t *cursor, uint32_t tag)
{
  int rc;

  rc = tlv_rewind(cursor);
  while (rc == TLV_OK) {
    if (cursor->current[0] == host_to_le32(tag))
      return TLV_OK;
    else
      rc = tlv_advance(cursor);
  }

  return rc;
}

int tlv_require_end(tlv_cursor_t *cursor)
{
  uint32_t *saved_current;
  int rc;

  if (cursor->end != NULL)
    return 0;

  saved_current = cursor->current;

  rc = tlv_find(cursor, TLV_TAG_END);
  if (rc != TLV_OK)
    return rc;

  cursor->current = saved_current;
  return TLV_OK;
}

size_t tlv_block_length_used(tlv_cursor_t *cursor)
{
  if (tlv_validate_state(cursor) != TLV_OK)
    return 0;
  if (tlv_require_end(cursor) != TLV_OK)
    return 0;
  return (size_t)((uint8_t *)cursor->end + 4 - (uint8_t *)cursor->block);
}

uint32_t *tlv_last_segment_end(tlv_cursor_t *cursor)
{
  tlv_cursor_t segment_cursor;
  uint32_t *last_segment_end = cursor->block;
  uint32_t *segment_start = cursor->block;

  /* Go through each segment and check that it has an end tag. If there is no
   * end tag then the previous segment was the last valid one, so return the
   * pointer to its end tag. */
  while(1){
    if (tlv_init_cursor(&segment_cursor, segment_start, cursor->limit) != TLV_OK)
      break;
    if (tlv_require_end(&segment_cursor) != TLV_OK)
      break;
    last_segment_end = segment_cursor.end;
    segment_start = segment_cursor.end + 1;
  }

  return last_segment_end;
}

static uint32_t *tlv_write(tlv_cursor_t *cursor, uint32_t tag,
                           const uint8_t *data, size_t len)
{
  uint32_t *ptr;

  ptr = cursor->current;

  /* Write tag and length */
  *ptr++ = host_to_le32(tag);
  *ptr++ = host_to_le32(len);

  /* Write data, if any */
  if (len > 0) {
    /* Ensure that any unwritten portion of the last 32-bit word is zero */
    ptr[(len - 1) / 4] = 0;
    memcpy(ptr, data, len);
    ptr += (len + 3) / 4;
  }

  return ptr;
}

int tlv_append(tlv_cursor_t *cursor, uint32_t tag,
               const uint8_t *data, size_t len)
{
  int rc;

  /* Set the cursor to the current end tag and insert the data there, shifting
     up the end tag and any segments after it. */
  rc = tlv_find(cursor, TLV_TAG_END);
  if (rc != TLV_OK)
    return rc;

  return tlv_insert(cursor, tag, data, len);
}

int tlv_insert(tlv_cursor_t *cursor, uint32_t tag,
               const uint8_t *data, size_t len)
{
  unsigned int delta;
  uint32_t *last_segment_end;
  int rc;

  rc = tlv_validate_state(cursor);
  if (rc != TLV_OK)
    return rc;

  if (tag == TLV_TAG_END)
    return TLV_BAD_ARG;

  delta = tlv_n_words_for_len(len);

  rc = tlv_require_end(cursor);
  if (rc != 0)
    return rc;

  last_segment_end = tlv_last_segment_end(cursor);

  /* Range check */
  if ((last_segment_end + 1 + delta) > cursor->limit)
    return TLV_NO_SPACE;

  /* Shuffle things up, leaving new space at cursor->current */
  memmove(cursor->current + delta, cursor->current,
          (last_segment_end + 1 - cursor->current) * sizeof(uint32_t));

  /* Adjust end pointer */
  cursor->end += delta;

  /* Write new data */
  tlv_write(cursor, tag, data, len);

  return TLV_OK;
}

int write_tlv_item(tlv_cursor_t *cursor, void *item_with_hdr)
{
  uint32_t *ptr = item_with_hdr;
  int rc;

  /* The item to write must already be little-endian. */
  rc = tlv_update_or_append(
      cursor, le32_to_host(ptr[0]), (uint8_t *)&ptr[2], le32_to_host(ptr[1]));
  if (rc != TLV_OK) {
    fprintf(stderr, "Unexpected error %d updating TLV tag 0x%08x?\n", rc,
            (unsigned int)ptr[0]);
  }
  return rc;
}

int tlv_delete(tlv_cursor_t *cursor)
{
  unsigned int delta;
  uint32_t *last_segment_end;
  int rc;

  rc = tlv_validate_state(cursor);
  if (rc != TLV_OK)
    return rc;

  if (cursor->current[0] == host_to_le32(TLV_TAG_END))
    return TLV_BAD_ARG;

  delta = tlv_n_words_for_item(cursor->current);

  rc = tlv_require_end(cursor);
  if (rc != 0)
    return rc;

  last_segment_end = tlv_last_segment_end(cursor);

  /* Shuffle things down, destroying the item at cursor->current */
  memmove(cursor->current, cursor->current + delta,
          (last_segment_end + 1 - cursor->current) * sizeof(uint32_t));

  /* Reset the new space at the end */
  memset(last_segment_end + 1 - delta, 0xff, delta * sizeof(uint32_t));

  /* Adjust end pointer */
  cursor->end -= delta;

  return TLV_OK;
}

int tlv_modify(tlv_cursor_t *cursor, const uint8_t *data, size_t len)
{
  unsigned int old_n_words, new_n_words, delta;
  uint32_t tag;
  uint32_t *last_segment_end;
  uint32_t *position;
  int rc;

  rc = tlv_validate_state(cursor);
  if (rc != TLV_OK)
    return rc;

  tag = le32_to_host(cursor->current[0]);

  if (tag == TLV_TAG_END)
    return TLV_BAD_ARG;

  old_n_words = tlv_n_words_for_item(cursor->current);
  new_n_words = tlv_n_words_for_len(len);

  rc = tlv_require_end(cursor);
  if (rc != 0)
    return rc;

  last_segment_end = tlv_last_segment_end(cursor);

  if (new_n_words > old_n_words) {
    /* Need more space */
    delta = new_n_words - old_n_words;
    position = cursor->current + old_n_words;

    /* Range check */
    if ((last_segment_end + 1 + delta) > cursor->limit)
      return TLV_NO_SPACE;

    /* Shuffle things up, making new space at cursor->current + old_n_words */
    memmove(position + delta, position,
            (last_segment_end + 1 - position) * sizeof(uint32_t));

    /* Adjust end pointer */
    cursor->end += delta;
  }
  else if (new_n_words < old_n_words) {
    /* Need less space */
    delta = old_n_words - new_n_words;
    position = cursor->current + new_n_words;

    /* Shuffle things down, removing words at cursor->current + new_n_words */
    memmove(position, position + delta,
            (last_segment_end + 1 - position) * sizeof(uint32_t));

    /* Reset the new space at the end */
    memset(last_segment_end + 1 - delta, 0xff, delta * sizeof(uint32_t));

    /* Adjust end pointer */
    cursor->end -= delta;
  }

  /* Write new data */
  tlv_write(cursor, tag, data, len);

  return TLV_OK;
}


int tlv_update_or_append(tlv_cursor_t *cursor, uint32_t tag,
                         const uint8_t *data, size_t len)
{
  int rc;

  rc = tlv_find(cursor, tag);
  if (rc == TLV_OK)
    return tlv_modify(cursor, data, len);
  else if (rc == TLV_NOT_FOUND)
    return tlv_append(cursor, tag, data, len);
  else
    return rc;
}


/*-----------------------------------------------------------------------------
 * Debugging
 *-----------------------------------------------------------------------------
 */

static void hexdump(const void *data, unsigned int len, unsigned int width)
{
  const uint8_t *ptr;
  unsigned int count, offset;
  uint8_t byte;
  char ascii[65];

  ptr = data;
  count = 0;
  offset = 0;
  memset(ascii, 0, sizeof(ascii));
  while (len-- > 0) {
    byte = *ptr++;
    if (count == 0)
      printf("  +%04x:", offset);
    printf(" %02x", byte);
    ascii[count % width] = ((byte >= 32) && (byte < 127)) ? byte : '.';
    count++;
    offset++;
    if (count == width) {
      printf("   %s\n", ascii);
      count = 0;
      memset(ascii, 0, sizeof(ascii));
    }
  }
  if (count != 0) {
    while (count++ < width)
      printf(" xx");
    printf("   %s\n", ascii);
  }
}

void tlv_dump(tlv_cursor_t *cursor)
{
  uint32_t tag;
  size_t length;
  const uint8_t *value;

  tag = tlv_tag(cursor);
  length = tlv_length(cursor);
  value = tlv_value(cursor);

  if (tag == TLV_TAG_END) {
    printf("Tag 0x%08x: END (at offset 0x%04x)\n", (unsigned)tag,
           (unsigned int)((uint8_t *)cursor->current
                          - (uint8_t *)cursor->block));
    return;
  }

  printf("Tag 0x%08x: %zu bytes (at offset 0x%04x)\n", (unsigned int)tag,
         length, (unsigned int)((uint8_t *)cursor->current
                                - (uint8_t *)cursor->block));

  hexdump(value, length, 4);
}

void tlv_dump_all(tlv_cursor_t *cursor)
{
  int rc;
  tlv_cursor_t cursor_copy;

  cursor_copy = *cursor;

  rc = tlv_find(&cursor_copy, TLV_TAG_END);
  if (rc != 0) {
    printf("*** Find end tag: rc=%d ***\n", rc);
    hexdump(cursor->block, (cursor->limit - cursor->block) * sizeof(uint32_t),
            4);
    return;
  }

  rc = tlv_rewind(&cursor_copy);
  while (rc == TLV_OK) {
    tlv_dump(&cursor_copy);
    rc = tlv_advance(&cursor_copy);
  }

  if (rc != TLV_NOT_FOUND)
    printf("*** WARNING: rc=%d\n", rc);

  printf("=====\n");
}

