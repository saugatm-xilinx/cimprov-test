#ifndef SOLARFLARE_SF_SIOCEFX_COMMON_H
#define SOLARFLARE_SF_SIOCEFX_COMMON_H 1

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
}

#endif // SOLARFLARE_SF_SIOCEFX_COMMON_H
