/**************************************************************************/
/*!  \file  little_endian.h
** \author  ajr
**  \brief  Endianness helpers for little endian platforms.
**   \date  2013/08/20
**    \cop  Copyright 2013 Solarflare Communications Inc.
*//************************************************************************/

#ifndef LITTLE_ENDIAN_H
#define LITTLE_ENDIAN_H


#define _host_to_le32(n) (n)
#define _host_to_le16(n) (n)
#define _le32_to_host(n) (n)
#define _le16_to_host(n) (n)
#define _host_to_le32_inplace(data_p, count) (void)(0)
#define _host_to_le16_inplace(data_p, count) (void)(0)
#define _le32_to_host_inplace(data_p, count) (void)(0)
#define _le16_to_host_inplace(data_p, count) (void)(0)

#define _host_to_be32(n) bswap32(n)
#define _be32_to_host(n) bswap32(n)
#define _host_to_be16(n) bswap16(n)
#define _be16_to_host(n) bswap16(n)
#define _host_to_be32_inplace(data_p, count) bswap32_inplace(data_p, count)
#define _be32_to_host_inplace(data_p, count) bswap32_inplace(data_p, count)
#define _host_to_be16_inplace(data_p, count) bswap16_inplace(data_p, count)
#define _be16_to_host_inplace(data_p, count) bswap16_inplace(data_p, count)


#endif /* LITTLE_ENDIAN_H */
