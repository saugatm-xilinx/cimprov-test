/***************************************************************************//*! \file liprovider/sf_native_vmware.h
** <L5_PRIVATE L5_SOURCE>
** \author  Solarflare
**  \brief  CIM Provider
**   \date  2017/09/05
**    \cop  (c) Solarflare Communications Inc.
** </L5_PRIVATE>
*//*
\**************************************************************************/

#ifndef SOLARFLARE_SF_NATIVE_VMWARE_H
#define SOLARFLARE_SF_NATIVE_VMWARE_H 1

#include "userMgmtSrc/sfvmk_mgmt_interface.h"

// VPD tags (used for VPD processing)
#define VPD_TAG_ID   0x02
#define VPD_TAG_END  0x0f
#define VPD_TAG_R    0x10
#define VPD_TAG_W    0x11

extern vmk_MgmtApiSignature sfvmk_mgmtSig;

namespace solarflare
{
    ///
    /// VMK Driver Management API Invocation.
    ///
    /// @param devName [in]       Pointer to interface name
    /// @param cmdCode [in]       Command code
    /// @param cmdParam [in/out]  Pointer to parameter structure
    ///
    /// @return VMK Driver Management API execution status or VMK_FAILURE
    ///
    extern int DrvMgmtCall(const char *devName, sfvmk_mgmtCbTypes_t cmdCode, void *cmdParam);
}

#endif // SOLARFLARE_SF_NATIVE_VMWARE_H
