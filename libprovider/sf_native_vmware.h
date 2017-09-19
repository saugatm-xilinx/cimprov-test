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

#include "userMgmtSrc/sfvmk_mgmtInterface.h"
#include "sf_nicMgmtApi.h"

extern vmk_MgmtApiSignature driverMgmtSig;

namespace solarflare
{
   extern int DrvMgmtCall(const char *devName, sfvmk_mgmtCbTypes_t cmdCode, void *cmdParam);
}

#endif // SOLARFLARE_SF_NATIVE_VMWARE_NVRAM_H
