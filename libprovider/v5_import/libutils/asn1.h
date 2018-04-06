/**************************************************************************\
*//*! \file
** <L5_PRIVATE L5_SOURCE>
** \author  mw, mc
**  \brief  Header definitions for ASN.1 parser
**   \date  2016/07/28
**    \cop  (c) Solarflare Communications Inc.
** </L5_PRIVATE>
*//*
\**************************************************************************/

#include <stdint.h>

#define MIN(a,b) ((a) < (b) ? (a) : (b))

typedef enum asn_tag_e {
  ASN_TAG_CONSTR_ = 0x20,
  ASN_TAG_CONTEXT_ = 0x80,
  ASN_TAG_INTEGER = 0x02,
  ASN_TAG_BIT_STRING = 0x03,
  ASN_TAG_OCTET_STRING = 0x04,
  ASN_TAG_OBJ_ID = 0x06,
  ASN_TAG_SEQ = 0x10 + ASN_TAG_CONSTR_,
  ASN_TAG_SET = 0x11 + ASN_TAG_CONSTR_,
} asn_tag_t;

#define ASN_TAG_PRIM_CONTEXT(num) (ASN_TAG_CONTEXT_ + num)
#define ASN_TAG_CONSTR_CONTEXT(num) (ASN_TAG_CONTEXT_ + num + ASN_TAG_CONSTR_)

typedef enum asn_flag_e {
  /* An optional item of a sequence */
  ASN_FLAG_OPTIONAL = 0x01,
  /* Explicitly tagged TLV. The TLV proper is wrapped within an outer TLV with a context-specific tag */
  ASN_FLAG_EXPLICIT = 0x02,
  /* The last element of array */
  ASN_FLAG_LAST_ITEM = 0x04,
  /* The next element of array refers to the same TLV. */
  ASN_FLAG_CONTINUED = 0x08,
} asn_flag_t;

typedef enum asn_todo_e {
  ASN_TODO_NOTHING = 0,
  /* Verify object ID. 'required_obj_id' must be provided. */
  ASN_TODO_CHECK_OBJ_ID,
  /* Parse a nested sequence. 'nested_seq' must be provided. */
  ASN_TODO_PARSE_SEQ,
  /* Parse a nested set of sequences of the same type. 'nested_seq' must be provided. */
  ASN_TODO_PARSE_SET_OF_SEQ,
  /* Run a handler. 'handler' must be provided. */
  ASN_TODO_HANDLER,
  /* Select a handler based on object ID in the current TLV,
   * then run this handler for the immediately following TLV.
   * This item covers two TLVs. First of them must be an object ID.
   * 'handlers_table' must be provided. */
  ASN_TODO_HANDLER_FROM_TABLE,

  /* Only for internal use within asn_parse() */
  ASN_TODO_TOP_LEVEL,
} asn_todo_t;

typedef struct asn_tlv_location_s {
  unsigned int offset_in_file;
  size_t total_size;
  uint8_t hdr_size;
} asn_tlv_location_t;

typedef enum asn_error_e {
  ASN_ERROR_NONE = 0,
  /* File format appears corrupted. */
  ASN_ERROR_CORRUPTED,
  /* File contains an unsupported encoding. */
  ASN_ERROR_NOT_SUPPORTED,
  /* File content doesn't match definition. */
  ASN_ERROR_WRONG_CONTENT,
  /* An object ID other than required. */
  ASN_ERROR_WRONG_OBJ_ID,
} asn_error_t;

typedef asn_error_t (*asn_handler_t)(asn_tlv_location_t);

typedef const struct asn_handlers_table_entry_s {
  const uint8_t *obj_id;
  asn_handler_t handler;
} asn_handlers_table_entry_t;

/* This is used to represent ASN.1 definitions.
 * An object of this type represents an item of an ASN.1 sequence.
 * Use an array of this type to define an ASN.1 sequence. */
typedef const struct asn_seq_item_s {
  uint8_t tag;
  uint8_t flags;
  uint8_t todo;
  union {
    asn_handler_t handler;
    asn_handlers_table_entry_t *handlers_table;
    const struct asn_seq_item_s *nested_seq;
    const uint8_t *required_obj_id;
  }payload;
} asn_seq_item_t;

/* ASN TLV header represented as a stucture */
typedef struct asn_tlv_hdr_s {
  uint8_t tag;
  uint32_t len;
  uint32_t hdr_len;
} asn_tlv_hdr_t;

/* Info related to a single nesting level */
typedef struct asn_level_status_s {
  /* The TLV whose content is being parsed */
  asn_tlv_location_t tlv_loc;
  /* We may be parsing a sequence or a set of sequences,
   * or reading in the top level TLV. */
  asn_todo_t action;
  /* Location of the next inner TLV to read
   * relative to the beginning of the outer one. */
  size_t cursor;
  /* Definition of the sequence being parsed. If a sequence is being parsed,
   * then this points to the currently expected item. NULL value denotes that
   * no more item is expected in the current sequence. If a set of sequences
   * is being parsed, then this points to the beginning (first item) of the
   * definition shared by all sequences in the set. */
  asn_seq_item_t *seq_item;
} asn_level_status_t;

/* Function prototype of an image reader */
typedef int (*asn_reader_fn)(void *context, uint8_t *buffer, unsigned int offset, size_t num_bytes_to_read);

/* Register an image-buffer for use by the parser */
void asn_register_image(const uint8_t *image_buffer, size_t image_buffer_len);

/* Parse a stream of bytes extracting the TLV header parameters into a structure object */
extern asn_error_t asn_tlv_header_parse(unsigned int offset_in_file, asn_tlv_hdr_t *tlv_hdr);

asn_error_t asn_parse(asn_seq_item_t *top_level_seq, unsigned int offset, uint32_t image_len);

/* Parse an image and verify that it is in a valid ASN.1 format as specified by top_level_seq */

