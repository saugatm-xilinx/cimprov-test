/**************************************************************************/
/*!  \file  sfu_endian.h
** \author  andrey
**  \brief  Functions for number conversion to little-/big-endian view.
**   \date  2014/06/04
**    \cop  Copyright 2014 Solarflare Communications Inc.
*//************************************************************************/

#ifndef SFUTILS_ENDIAN_H
#define SFUTILS_ENDIAN_H

#include <endian.h>

#define EFX_WANT_KERNEL_TYPES
#include "efx_linux_types.h"

#if __BYTE_ORDER == __BIG_ENDIAN
static inline u16 cpu_to_be16(u16 value)
{
	return value;
}
static inline u32 cpu_to_be32(u32 value)
{
	return value;
}
static inline u64 cpu_to_be64(u64 value)
{
	return value;
}

static inline u16 cpu_to_le16(u16 value)
{
	return (value >> 8) | (value << 8);
}
static inline u32 cpu_to_le32(u32 value)
{
	return cpu_to_le16(value >> 16) | (cpu_to_le16(value) << 16);
}
static inline u64 cpu_to_le64(u64 value)
{
	return cpu_to_le32(value >> 32) | ((u64)cpu_to_le32(value) << 32);
}

#else
static inline u16 cpu_to_be16(u16 value)
{
	return (value >> 8) | (value << 8);
}
static inline u32 cpu_to_be32(u32 value)
{
	return cpu_to_be16(value >> 16) | (cpu_to_be16(value) << 16);
}
static inline u64 cpu_to_be64(u64 value)
{
	return cpu_to_be32(value >> 32) | ((u64)cpu_to_be32(value) << 32);
}

static inline u16 cpu_to_le16(u16 value)
{
	return value;
}
static inline u32 cpu_to_le32(u32 value)
{
	return value;
}
static inline u64 cpu_to_le64(u64 value)
{
	return value;
}
#endif

#define be_to_cpu16 cpu_to_be16
#define be_to_cpu32 cpu_to_be32
#define be_to_cpu64 cpu_to_be64

#define le_to_cpu16 cpu_to_le16
#define le_to_cpu32 cpu_to_le32
#define le_to_cpu64 cpu_to_le64

#define ntohll cpu_to_be64
#define htonll cpu_to_be64

#endif /* SFUTILS_ENDIAN_H */
