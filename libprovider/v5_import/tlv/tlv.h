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

/* This is a modified copy of src/tools/mc-comms/tlv.h.
 * Handlers for adding, deleting, modifying, etc. tags within a partition have
 * been added.
 * The v5 build system does not seem to allow us to conveniently share these
 * files.
 */

#ifndef TLV_H
#define TLV_H

#ifdef _WIN32
#include <ci/compat.h>
#ifndef uint64_t
#define uint64_t ci_uint64
#endif
#ifndef uint32_t
#define uint32_t ci_uint32
#endif
#ifndef uint16_t
#define uint16_t ci_uint16
#endif
#ifndef uint8_t
#define uint8_t ci_uint8
#endif
#ifndef int32_t
#define int32_t ci_int32
#endif
#ifndef int16_t
#define int16_t ci_int16
#endif
#ifndef int8_t
#define int8_t ci_int8
#endif
#else
#include <stdint.h>
#endif


/**
 * Status codes returned by the tlv_xxx() functions.
 */
enum {
  TLV_OK = 0,
  TLV_NOT_FOUND,
  TLV_NO_SPACE,
  TLV_BAD_ARG,
  TLV_CORRUPT_BLOCK
};

/**
 * A TLV cursor tracks a block of memory containing TLV data, and a current
 * position within it.
 */
typedef struct tlv_cursor_s {
  uint32_t *block;
  uint32_t *current;
  uint32_t *end;
  uint32_t *limit;
} tlv_cursor_t;

/**
 * Initialise an empty block of memory for TLV manipulation (just writes a
 * TLV_TAG_END to the first word).
 */
extern void tlv_init_block(uint32_t *block);

/**
 * Return the number of bytes actually used within the block, including the
 * end tag.
 */
extern size_t tlv_block_length_used(tlv_cursor_t *cursor);

/**
 * Return a pointer to the end tag of the last valid segment in the partition.
 */
extern uint32_t *tlv_last_segment_end(tlv_cursor_t *cursor);

/**
 * Return the tag ID of the item at the cursor.
 */
extern uint32_t tlv_tag(tlv_cursor_t *cursor);

/**
 * Return the length of the item at the cursor.
 */
extern size_t tlv_length(tlv_cursor_t *cursor);

/**
 * Return a pointer to the value field of the item at the cursor.
 */
extern uint8_t *tlv_value(tlv_cursor_t *cursor);

/**
 * Return a pointer to the entire item (including tag and length) at the
 * cursor.
 */
extern uint8_t *tlv_item(tlv_cursor_t *cursor);

/**
 * Verify that the end tag can be found from the cursor position.
 */
extern int tlv_require_end(tlv_cursor_t *cursor);

/**
 * Sanity-check the current state described by the cursor.
 */
extern int tlv_validate_state(tlv_cursor_t *cursor);

/**
 * Initialise a cursor, positioning it at the start of the specified block.
 */
extern int tlv_init_cursor(tlv_cursor_t *cursor, uint32_t *block,
                           uint32_t *limit);

/**
 * Wrapper for tlv_init_cursor, taking uint8_t * and size instead of uint32_t *
 * and limit.
 */
extern int tlv_init_cursor_from_size(tlv_cursor_t *cursor, uint8_t *block,
                                     size_t size);

 /**
 * Rewind the cursor to the start of the block.
 */
extern int tlv_rewind(tlv_cursor_t *cursor);

/**
 * Advance the cursor to the next item.
 */
extern int tlv_advance(tlv_cursor_t *cursor);

/**
 * Find an item with the specified tag within the block.
 */
extern int tlv_find(tlv_cursor_t *cursor, uint32_t tag);

/**
 * Append an item to the block.
 */
extern int tlv_append(tlv_cursor_t *cursor, uint32_t tag, const uint8_t *data,
                      size_t len);

/**
 * Insert an item within the block, at the current cursor position.
 */
extern int tlv_insert(tlv_cursor_t *cursor, uint32_t tag,
                      const uint8_t *data, size_t len);

/**
 * Write a tlv item at the current cursor position.
 */
int write_tlv_item(tlv_cursor_t *cursor, void *item_with_hdr);

/**
 * Delete the item at the cursor.
 */
extern int tlv_delete(tlv_cursor_t *cursor);

/**
 * Modify the data of the item at the cursor.
 */
extern int tlv_modify(tlv_cursor_t *cursor, const uint8_t *data, size_t len);

/**
 * Locate and modify an item if it already exists, or append if it doesn't.
 */
extern int tlv_update_or_append(tlv_cursor_t *cursor, uint32_t tag,
                                const uint8_t *data, size_t len);

/**
 * Dump the item at the cursor.
 */
extern void tlv_dump(tlv_cursor_t *cursor);

/**
 * Dump the entire block (the cursor's current position is preserved).
 */
extern void tlv_dump_all(tlv_cursor_t *cursor);

#endif /* TLV_H */

