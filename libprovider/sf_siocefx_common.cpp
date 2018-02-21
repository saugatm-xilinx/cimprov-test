/***************************************************************************//*! \file liprovider/sf_siocefx_common.cpp
** <L5_PRIVATE L5_SOURCE>
** \author  OktetLabs
**  \brief  CIM Provider
**   \date  2018/02/02
**    \cop  (c) Solarflare Communications Inc.
** </L5_PRIVATE>
*//*
\**************************************************************************/

#include "sf_core.h"
#include <stdint.h>

namespace solarflare
{
    ///
    /// Check whether host is little-endian.
    ///
    /// @return true if host is little-endian, false otherwise.
    static inline bool host_is_le(void)
    {
        uint32_t  x = 0xffff0000;
        uint8_t  *y = (uint8_t *)&x;

        if (*y == 0)
            return true;

        return false;
    }

    ///
    /// Convert 16bit value to another endianness.
    ///
    /// @param x      Value to convert.
    ///
    /// @return Converted value.
    static inline uint16_t change_endianness16(uint16_t x)
    {
        return ((x & 0x00ff) << 8) | ((x & 0xff00) >> 8);
    }

    ///
    /// Convert 32bit value to another endianness.
    ///
    /// @param x      Value to convert.
    ///
    /// @return Converted value.
    static inline uint32_t change_endianness32(uint32_t x)
    {
        return ((x & 0x000000ff) << 24) |
               ((x & 0x0000ff00) << 8) |
               ((x & 0x00ff0000) >> 8) |
               ((x & 0xff000000) >> 24);
    }

    // See description in sf_siocefx_common.h
    uint16_t fix_mcdi_endianness16(uint16_t x)
    {
        if (host_is_le())
            return x;
        else
            return change_endianness16(x);
    }

    // See description in sf_siocefx_common.h
    uint32_t fix_mcdi_endianness32(uint32_t x)
    {
        if (host_is_le())
            return x;
        else
            return change_endianness32(x);
    }

    ///
    /// Get value of 8bit MCDI field.
    ///
    /// @param payload    Where to get value.
    ///
    /// @return Obtained value.
    static uint8_t get_mcdi8(void *payload)
    {
        uint8_t *p = (uint8_t *)payload;

        return *p;
    }

    ///
    /// Get value of 16bit MCDI field.
    ///
    /// @param payload    Where to get value.
    ///
    /// @return Obtained value.
    static uint16_t get_mcdi16(void *payload)
    {
        uint16_t *p = (uint16_t *)payload;

        return fix_mcdi_endianness16(*p);
    }

    ///
    /// Get value of 32bit MCDI field.
    ///
    /// @param payload    Where to get value.
    ///
    /// @return Obtained value.
    static uint32_t get_mcdi32(void *payload)
    {
        uint32_t *p = (uint32_t *)payload;

        return fix_mcdi_endianness32(*p);
    }

    // See description in sf_siocefx_common.h
    uint32_t get_mcdi_field(void *payload, size_t ofst, size_t len)
    {
        uint8_t *p = (uint8_t *)payload;

        switch (len)
        {
            case 1:
                return get_mcdi8(p + ofst);

            case 2:
                return get_mcdi16(p + ofst);

            case 4:
                return get_mcdi32(p + ofst);
        }

        THROW_PROVIDER_EXCEPTION_FMT("Cannot read MCDI field of length %u",
                                     (unsigned int)len);
    }

    // See description in sf_siocefx_common.h
    uint32_t get_mcdi_field_part(uint32_t field, size_t len,
                                 size_t lbn, size_t width)
    {
        uint32_t mask;

        if (lbn > len * 8 || lbn + width > len * 8 || len > sizeof(mask))
            THROW_PROVIDER_EXCEPTION_FMT(
                "Incompatible or too big field length %u, LBN %u, width %u",
                (unsigned int)len, (unsigned int)lbn,
                (unsigned int)width);

        mask = (width == 32 ? 0xffffffff : ((1 << width) - 1));
        field = (field >> lbn) & mask;

        return field;
    }

    ///
    /// Set value of 8bit MCDI field.
    ///
    /// @param payload      Pointer to MCDI field.
    /// @param value        Value to set.
    static void set_mcdi8(void *payload, uint8_t value)
    {
        uint8_t *p = (uint8_t *)payload;

        *p = value;
    }

    ///
    /// Set value of 16bit MCDI field.
    ///
    /// @param payload      Pointer to MCDI field.
    /// @param value        Value to set.
    static void set_mcdi16(void *payload, uint16_t value)
    {
        uint16_t *p = (uint16_t *)payload;

        *p = fix_mcdi_endianness16(value);
    }

    ///
    /// Set value of 32bit MCDI field.
    ///
    /// @param payload      Pointer to MCDI field.
    /// @param value        Value to set.
    static void set_mcdi32(void *payload, uint32_t value)
    {
        uint32_t *p = (uint32_t *)payload;

        *p = fix_mcdi_endianness32(value);
    }

    // See description in sf_siocefx_common.h
    void set_mcdi_field(void *payload, size_t ofst, size_t len,
                        uint32_t value)
    {
        uint8_t *p = (uint8_t *)payload;

        switch (len)
        {
            case 1:
                set_mcdi8(p + ofst, value);
                break;

            case 2:
                set_mcdi16(p + ofst, value);
                break;

            case 4:
                set_mcdi32(p + ofst, value);
                break;

            default:
                THROW_PROVIDER_EXCEPTION_FMT(
                                "Cannot set MCDI field of length %u",
                                (unsigned int)len);
        }

        return;
    }

    // See description in sf_siocefx_common.h
    void set_mcdi_field_part(uint32_t &field, size_t len,
                             size_t lbn, size_t width, uint32_t value)
    {
        uint32_t mask;

        if (lbn > len * 8 || lbn + width > len * 8 || len > sizeof(mask))
            THROW_PROVIDER_EXCEPTION_FMT(
                "Incompatible or too big field length %u, LBN %u, width %u",
                (unsigned int)len, (unsigned int)lbn,
                (unsigned int)width);

        mask = (width == 32 ? 0xffffffff : ((1 << width) - 1));

        if (value & ~mask)
            THROW_PROVIDER_EXCEPTION_FMT(
                  "Value 0x%x is too big to fit in %u bits",
                  value, (unsigned int)width);

        mask = mask << lbn;
        field = (field & ~mask) | (value << lbn);
    }
}
