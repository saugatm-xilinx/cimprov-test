/***************************************************************************//*! \file liprovider/sf_siocefx_common.h
** <L5_PRIVATE L5_SOURCE>
** \author  OktetLabs
**  \brief  CIM Provider
**   \date  2013/10/02
**    \cop  (c) Solarflare Communications Inc.
** </L5_PRIVATE>
*//*
\**************************************************************************/

#ifndef SOLARFLARE_SF_SIOCEFX_COMMON_H
#define SOLARFLARE_SF_SIOCEFX_COMMON_H 1

#include <stdint.h>
#include "efx_ioctl.h"
#include "ci/mgmt/mc_flash_layout.h"
#include "efx_regs_mcdi.h"
#include "ci/tools/byteorder.h"
#include "ci/tools/bitfield.h"

// These macros are used for setting proper value
// for SIOCEFX ioctl.
#define SET_PAYLOAD_DWORD(_payload, _ofst, _val) \
    CI_POPULATE_DWORD_1((_payload).dword[_ofst], \
                        CI_DWORD_0, (_val))

#define PAYLOAD_DWORD(_payload, _ofst) \
    CI_DWORD_FIELD((_payload).dword[_ofst], CI_DWORD_0)

namespace solarflare
{
    ///
    /// Auxiliary type for MCDI data processing.
    ///
    typedef union {
        uint8_t     u8[MCDI_CTL_SDU_LEN_MAX];
        uint16_t    u16[MCDI_CTL_SDU_LEN_MAX/2];
        ci_dword_t  dword[MCDI_CTL_SDU_LEN_MAX/4];
    } payload_t;

    ///
    /// Convert 16bit value to another endianness if host is not
    /// little-endian.
    ///
    /// @param x    Value.
    ///
    /// @return Converted value.
    uint16_t fix_mcdi_endianness16(uint16_t x);

    ///
    /// Convert 32bit value to another endianness if host is not
    /// little-endian.
    ///
    /// @param x    Value.
    ///
    /// @return Converted value.
    uint32_t fix_mcdi_endianness32(uint32_t x);

    ///
    /// Get value of a MCDI payload field.
    ///
    /// @param payload      MCDI payload.
    /// @param ofst         Field offset.
    /// @param len          Field length.
    ///
    /// @return Field value.
    uint32_t get_mcdi_field(void *payload, size_t ofst, size_t len);

    ///
    /// Get value of a MCDI subfield (stored in specific bits of
    /// a field).
    ///
    /// @param field        Field value.
    /// @param len          Field length.
    /// @param lbn          Number of the first bit of subfield.
    /// @param width        Number of bits in subfield.
    ///
    /// @return Subfield value.
    uint32_t get_mcdi_field_part(uint32_t field, size_t len,
                                 size_t lbn, size_t width);

    ///
    /// Set value of a MCDI payload field.
    ///
    /// @param payload      MCDI payload.
    /// @param ofst         Field offset.
    /// @param len          Field length.
    /// @param value        Value to set.
    void set_mcdi_field(void *payload, size_t ofst, size_t len,
                        uint32_t value);

    ///
    /// Set value of a MCDI subfield (stored in specific bits of
    /// a field).
    ///
    /// @param field        Field value in which to change specific bits.
    /// @param len          Field length.
    /// @param lbn          Number of the first bit of subfield.
    /// @param width        Number of bits in subfield.
    /// @param value        Subfield value.
    void set_mcdi_field_part(uint32_t &field, size_t len,
                             size_t lbn, size_t width, uint32_t value);
}

///
/// Get value of a MCDI field.
///
/// @param payload_     MCDI request payload.
/// @param field_       Field name (without MC_CMD_ and suffix,
///                     e.g. PRIVILEGE_MODIFY_IN_FUNCTION).
#define GET_MCDI_FIELD(payload_, field_) \
    get_mcdi_field(payload_, MC_CMD_ ## field_ ## _OFST, \
                   MC_CMD_ ## field_ ## _LEN)

///
/// Get value stored in specific bits of MCDI field.
///
/// @param field_   Field name (example: PRIVILEGE_MODIFY_IN_FUNCTION).
/// @param part_    Field part name (example: PF).
/// @param value_   Field value.
#define GET_MCDI_FIELD_PART(field_, part_, value_) \
    get_mcdi_field_part(value_, MC_CMD_ ## field_ ## _LEN, \
                        MC_CMD_ ## field_ ## _ ## part_ ## _LBN, \
                        MC_CMD_ ## field_ ## _ ## part_ ## _WIDTH)

///
/// Set value of a MCDI field.
///
/// @param payload_     MCDI request payload.
/// @param field_       Field name (without MC_CMD_ and suffix,
///                     like PRIVILEGE_MODIFY_IN_FUNCTION).
/// @param value_       Value to set.
#define SET_MCDI_FIELD(payload_, field_, value_) \
    set_mcdi_field(payload_, MC_CMD_ ## field_ ## _OFST, \
                   MC_CMD_ ## field_ ## _LEN, value_)

///
/// Set value of a subfield (specific bits in MCDI field).
///
/// @param field_     Field name (example: PRIVILEGE_MODIFY_IN_FUNCTION).
/// @param part_      Field part name (example: PF).
/// @param value_     Field value in which to set subfield.
/// @param part_val_  Subfield value to set.
#define SET_MCDI_FIELD_PART(field_, part_, value_, part_val_) \
    set_mcdi_field_part(value_, MC_CMD_ ## field_ ## _LEN, \
                        MC_CMD_ ## field_ ## _ ## part_ ## _LBN, \
                        MC_CMD_ ## field_ ## _ ## part_ ## _WIDTH, \
                        part_val_)

#endif // SOLARFLARE_SF_SIOCEFX_COMMON_H
