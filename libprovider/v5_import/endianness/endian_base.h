/**************************************************************************/
/*!  \file  endian_base.h
** \author  ajr
**  \brief  Endianness helpers
**   \date  2013/08/20
**    \cop  Copyright 2013 Solarflare Communications Inc.
*//************************************************************************/

#ifndef ENDIAN_BASE_H
#define ENDIAN_BASE_H

#ifdef _WIN32
#include <ci/compat.h>
typedef ci_uint64 uint64_t;
typedef ci_uint32 uint32_t;
typedef ci_int32  int32_t;
typedef ci_uint16 uint16_t;
typedef ci_uint8  uint8_t;
typedef ci_int16  int16_t;
typedef ci_int8   int8_t;
#else
#include <stdint.h>
#endif

#ifndef __ORDER_LITTLE_ENDIAN__
#define __ORDER_LITTLE_ENDIAN__ 1234
#endif
#ifndef __ORDER_BIG_ENDIAN__
#define __ORDER_BIG_ENDIAN__ 4321
#endif

#ifndef __BYTE_ORDER__

# if defined(__GNUC__)
#  if defined(__i386__)
#   define __BYTE_ORDER__ __ORDER_LITTLE_ENDIAN__
#  elif defined(__x86_64__)
#   define __BYTE_ORDER__ __ORDER_LITTLE_ENDIAN__
#  elif defined(__PPC__)
#   define __BYTE_ORDER__ __ORDER_BIG_ENDIAN__
#  elif defined(__ia64__)
#   define __BYTE_ORDER__ __ORDER_LITTLE_ENDIAN__
#  elif defined(__mips__)
#   define __BYTE_ORDER__ __ORDER_LITTLE_ENDIAN__
#  else
#   error Unknown processor - GNU C
#  endif

# elif defined(_MSC_VER)
#  if defined(__i386__)
#   define __BYTE_ORDER__ __ORDER_LITTLE_ENDIAN__
#  elif defined(__x86_64__)
#   define __BYTE_ORDER__ __ORDER_LITTLE_ENDIAN__
#  else
#   error Unknown processor - MSC
#  endif
# endif

#endif

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#include "little_endian.h"
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#include "big_endian.h"
#else
#error "Invalid __BYTE_ORDER__ macro."
#endif

/* Wrappers around bswap<n>() and bswap<n>_inplace() functions. */
#define host_to_le32(n)                     _host_to_le32(n)
#define le32_to_host(n)                     _le32_to_host(n)
#define host_to_le16(n)                     _host_to_le16(n)
#define le16_to_host(n)                     _le16_to_host(n)
#define host_to_le32_inplace(data_p, count) _host_to_le32_inplace(data_p, count)
#define le32_to_host_inplace(data_p, count) _le32_to_host_inplace(data_p, count)
#define host_to_le16_inplace(data_p, count) _host_to_le16_inplace(data_p, count)
#define le16_to_host_inplace(data_p, count) _le16_to_host_inplace(data_p, count)

#define host_to_be32(n)                     _host_to_be32(n)
#define host_to_be16(n)                     _host_to_be16(n)
#define be32_to_host(n)                     _be32_to_host(n)
#define be16_to_host(n)                     _be16_to_host(n)
#define host_to_be32_inplace(data_p, count) _host_to_be32_inplace(data_p, count)
#define host_to_be16_inplace(data_p, count) _host_to_be16_inplace(data_p, count)
#define be32_to_host_inplace(data_p, count) _be32_to_host_inplace(data_p, count)
#define be16_to_host_inplace(data_p, count) _be16_to_host_inplace(data_p, count)


uint32_t bswap32(uint32_t n);
uint16_t bswap16(uint16_t n);
void bswap32_inplace(uint32_t *data, size_t count);
void bswap16_inplace(uint16_t *data, size_t count);


#endif /* ENDIAN_BASE_H */
