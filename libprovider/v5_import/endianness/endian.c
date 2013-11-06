/**************************************************************************/
/*!  \file  endian.h
** \author  ajr
**  \brief  Implementation of endianness helpers
**   \date  2013/08/20
**    \cop  Copyright 2013 Solarflare Communications Inc.
*//************************************************************************/

#include <stddef.h>

#include "endian_base.h"

void bswap32_inplace(uint32_t* data, size_t count)
{
  uint8_t* bytes = (uint8_t*)data;
  size_t addr;

  for( addr = 0; addr < count; addr++ )
    data[addr] = (bytes[4 * addr] |
                  (bytes[4 * addr + 1] << 8) |
                  (bytes[4 * addr + 2] << 16) |
                  (bytes[4 * addr + 3] << 24));
}

void bswap16_inplace(uint16_t* data, size_t count)
{
  uint8_t* bytes = (uint8_t*)data;
  size_t addr;

  for( addr = 0; addr < count; addr++ )
    data[addr] = (bytes[2 * addr] |
                  (bytes[2 * addr + 1] << 8));
}

uint32_t bswap32(uint32_t n)
{
  bswap32_inplace(&n, 1);
  return n;
}

uint16_t bswap16(uint16_t n)
{
  bswap16_inplace(&n, 1);
  return n;
}
