/**************************************************************************\
*//*! \file
** <L5_PRIVATE L5_HEADER >
** \author  djr
**  \brief  Definitions for the RBMW protocol.
**   \date  2003/03/28
**    \cop  (c) Level 5 Networks Limited.
** </L5_PRIVATE>
*//*
\**************************************************************************/

/*! \cidoxg_include_ci_net  */

#ifndef __CI_NET_RBMW_H__
#define __CI_NET_RBMW_H__

#include <ci/compat.h>


/**********************************************************************
 * Header.
 */

/*! RBMW Packet header */
typedef struct {
  ci_uint8   ether_dhost[6];
  ci_uint8   ether_shost[6];
  ci_uint16  ether_type;
  ci_uint16  ci_ver_src;	/*!< 4 bits version + 12 bits source */
  ci_uint16  ci_aperture;	/*!< 12 bits + 4 bits nonce          */
  ci_uint16  ci_seq;
} ci_rbmw_packet_header;


#define CI_RBMW_PROTOCOL_ETHER_TYPE	CI_BSWAPC_BE16(0xC101)
#define CI_ETHERTYPE_RBMW		CI_RBMW_PROTOCOL_ETHER_TYPE

/* Extract fields from header */

#define CI_RBMW_PACKET_VERSION(ph)		\
  (CI_BSWAP_BE16((ph)->ci_ver_src) >> 12u)

#define CI_RBMW_PACKET_SOURCE(ph)		\
  (CI_BSWAP_BE16((ph)->ci_ver_src) & 0x0fff)

#define CI_RBMW_PACKET_APERTURE(ph)	CI_BSWAP_BE16((ph)->ci_aperture)
#define CI_RBMW_PACKET_SEQ(ph)		CI_BSWAP_BE16((ph)->ci_seq)

/* Put fields into header. */

#define CI_RBMW1_PACKET_VER_SOURCE(src)		\
  CI_BSWAPC_BE16((1u << 12u) | (src))


/**********************************************************************
 * Bursts.
 */

#define CI_RBMW_ESCAPE_BV0	0x01	/* byte enables */
#define CI_RBMW_ESCAPE_BV1	0x02
#define CI_RBMW_ESCAPE_BV2	0x04
#define CI_RBMW_ESCAPE_BV3	0x08
#define CI_RBMW_ESCAPE_SOB	0x10	/* next word is start of burst */
#define CI_RBMW_ESCAPE_EOB	0x20	/* next word is end of burst   */
#define CI_RBMW_ESCAPE_CKS	0x40	/* next-but-one is checksum    */
#define CI_RBMW_ESCAPE_RESERVED	0x80	/* must be zero                */

#define CI_RBMW_MAX_BURST_WORDS	512	/* max data words between CRCs */

#define CI_RBMW_ESCAPE_WORD(flags)		\
  CI_BSWAP_BE32(((flags) << 24u) | 0xC1E5CA)

#define CI_RBMW_ADDRESS_WORD(flags, dword_offset)	\
  CI_BSWAP_BE32(((flags) << 24u) | (dword_offset))


#define CI_RBMW1_IS_ESCAPE_WORD(w)			\
  ((CI_BSWAP_BE32(w) & 0x00ffffff) == 0x00C1E5CA)

#define CI_RBMW2_IS_ESCAPE_WORD(w)			\
  ((CI_BSWAP_BE32(w) & 0x00ffffff) == 0x00E5CA9E)

#define CI_RBMW_ESCAPE_WORD_FLAGS(w)	(CI_BSWAP_BE32(w) >> 24u)

#define CI_RBMW_ADDRESS_WORD_OFFSET(w)	(CI_BSWAP_BE32(w) & 0x00ffffff)
#define CI_RBMW_ADDRESS_WORD_FLAGS(w)	(CI_BSWAP_BE32(w) >> 24u)


/*
** According to Dave C's verilog files:
**
**   X25: (0 5 12 16)
**   USB: (0 2 15 16)
*/
#define CI_RBMW1_X25_16_POLY		0x11021
#define CI_RBMW1_USB_16_POLY		0x18005

/* v2 bits 0 1 2 4 5 7 8 10 11 12 16 22 23 26 32 */
#define CI_RBMW2_32_POLY		0x04c11db7
/*#define CI_RBMW2_32_POLY		0xedb88320*/



#endif  /* __CI_NET_RBMW_H__ */

/*! \cidoxg_end */
