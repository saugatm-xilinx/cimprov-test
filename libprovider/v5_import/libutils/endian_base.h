/*
 * Copyright 2013 - 2019 Xilinx, Inc.
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

#ifndef ENDIAN_BASE_H
#define ENDIAN_BASE_H

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
#  elif defined(__arm__)
#   define __BYTE_ORDER__ __ORDER_LITTLE_ENDIAN__
#  elif defined(__aarch64__)
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
