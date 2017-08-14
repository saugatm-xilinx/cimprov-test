/*************************************************************************
 ** Copyright (c) 2017 Solarflare Communications Inc. All rights reserved.
 ** Use is subject to license terms.
 **
 ** -- Solarflare Confidential
 ************************************************************************/
/* File added for compiling nicMgmtApi in a C++ environment.
 * extern C declaration was wrapped around nicMgmtApi.h to prevent name mangling.
 * */
#ifndef _SF_NIC_MGMT_API_H_
#define _SF_NIC_MGMT_API_H_

#if defined(__cplusplus)
extern "C"{
#endif

#include <nicMgmtApi.h>

#if defined(__cplusplus)
}
#endif

#endif /*_SF_NIC_MGMT_API_H_*/
