/****************************************************************************
 * FSBC: (F)low (S)pec (B)inary (C)ode.
 *
 * fsbc.h: Defines an extensible binary encoding for packet flow
 * specifications.
 *
 *
 * Copyright 2012-2012: Solarflare Communications, Inc.,
 *                      9501 Jeronimo Road, Suite 250,
 *                      Irvine, CA 92618, USA
 *
 * Developed and maintained by Solarflare Communications:
 *                      <onload-dev@solarflare.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation, incorporated herein by reference.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 ****************************************************************************
 */

#ifndef __CI_FSBC_H__
#define __CI_FSBC_H__


/**********************************************************************
 * Binary encoding of flow specifications.  Describes a set of header
 * fields that must be matched to identify a flow.
 */

/**
 * Identifies a particular field in a packet header.
 */
enum fsbc_field_id {     /* payload bytes */
  FSBC_ETH_CAST,         /* 1 */
  FSBC_ETH_SHOST,        /* 6 */
  FSBC_ETH_DHOST,        /* 6 */
  FSBC_ETH_VLAN,         /* 2 */
  FSBC_ETH_VLAN2,        /* 2 */
  FSBC_ETH_PRIO,         /* 1 */
  FSBC_ETH_PRIO2,        /* 1 */
  FSBC_ETH_TYPE,         /* 2 */
  FSBC_IP_PROTOCOL,      /* 1 */
  FSBC_IP4_SADDR,        /* 4 */
  FSBC_IP4_DADDR,        /* 4 */
  FSBC_INET_SPORT,       /* 2 */
  FSBC_INET_DPORT,       /* 2 */
  /* Important: Insert new header fields here, and update
   * fsbc_field_id_to_payload_len() below.
   */
  FSBC_FIELD_ID_END      /* this may increase in future! */
};


/* Values for the FSBC_ETH_CAST field. */
#define FSBC_ETH_UNICAST    0
#define FSBC_ETH_MULTICAST  1


enum fsbc_flags {
  FSBC_FF_OPTIONAL        = 0x1,
};


/**
 * The encoding of a flow-spec is: A [struct fsbc_flow] followed by a
 * sequence of field-specs, each consisting of a [struct fsbc_field]
 * followed by a payload.
 */
struct fsbc_flow {
  /** Total length of the flow-spec, in bytes. */
  int           length;
  /** Bitmask of essential features needed to decode this flow. */
  unsigned      requires;
  /** Offset of last fsbc_field from start of fsbc_flow. */
  int           last_offset;
  /** Offset of first fsbc_field from start of fsbc_flow.  (ie. The first
   * fsbc_field need not immediately follow the fsbc_flow.  This is to
   * allow for future extensions to the header).
   */
  unsigned char first_offset;
};


/**
 * Header for each field-spec within a flow-spec.
 *
 * NB. Following each fsbc_field there is expected to be payload of the
 * length given by fsbc_field_id_to_payload_len().  The gap between each
 * [struct fsbc_field] is permitted to be larger than this to allow for
 * future extensions that are backwards compatible.
 */
struct fsbc_field {
  /** Value from [enum fsbc_field_id]. */
  unsigned char  field_id;
  /** Number of bytes from start of this header to the start of the next. */
  unsigned char  next_offset;
  /* Flags from [enum fsbc_flags]. */
  unsigned short flags;
};


/**
 * Parse string representation of a flow and return an fsbc_flow.
 * Ownership of the memory is transferred to the caller, and should be
 * freed with free() or equiv.
 *
 * The pointer identified by [*flow] must either be NULL, or point at an
 * existing valid flow-spec that is augmented with the new spec specified
 * in the string.
 *
 * Returns 0 on success; -ENOMEM if unable to allocate memory; -EINVAL if
 * the string cannot be parsed; EIO if a name lookup failed.
 */
extern int fsbc_flow_from_string(const char* str, struct fsbc_flow** flow);


/**
 * Return a string representation of a flow.
 *
 * Returns 0 on success; -ENOMEM if unable to allocate memory; -EINVAL if
 * the flow is invalid.
 */
extern int fsbc_flow_to_string(const struct fsbc_flow* flow, char** str_out);


/**
 * Add a field to a flow.
 *
 * Returns 0 on success; -ENOMEM if unable to allocate memory; -EINVAL if
 * any parameters are invalid.
 */
extern int fsbc_flow_extend(struct fsbc_flow** flow,
                            enum fsbc_field_id field_id,
                            const void* field_val);


/**
 * Add a field to a flow.  You probably want fsbc_flow_extend() rather than
 * this one.
 *
 * Returns 0 on success; -ENOMEM if unable to allocate memory; -EINVAL if
 * any parameters are invalid.
 */
extern int fsbc_flow_extend4(struct fsbc_flow** flow,
                             enum fsbc_field_id field_id,
                             const void* field_val, int field_len,
                             unsigned flags);


/**
 * Return the expected payload length for a given field.
 */
static inline int fsbc_field_id_to_payload_len(enum fsbc_field_id field_id)
{
  static const int field_id_to_len[] = {
    1, /* eth_cast    */
    6, /* eth_shost   */
    6, /* eth_dhost   */
    2, /* eth_vlan    */
    2, /* eth_vlan2   */
    1, /* eth_prio    */
    1, /* eth_prio2   */
    2, /* eth_type    */
    1, /* ip_protocol */
    4, /* ip4_saddr   */
    4, /* ip4_daddr   */
    2, /* inet_sport  */
    2, /* inet_dport  */
  };
  return ((unsigned) field_id < FSBC_FIELD_ID_END)
    ? field_id_to_len[field_id]
    : -1;
}


#endif  /* __CI_FSBC_H__ */
