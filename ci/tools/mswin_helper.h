/**************************************************************************\
*//*! \file
** <L5_PRIVATE L5_HEADER >
** \author  jfox
**  \brief  Microsoft Windows Helper Routines (mainly routines for pretty printing windows API flags and enums).
**   \date  2006/06/30
**    \cop  (c) Level 5 Networks Limited.
** </L5_PRIVATE>
*//*
\**************************************************************************/

/*! \cidoxg_include_ci_tools */

#ifndef __CI_TOOLS_MSWIN_HELPER_H__
#define __CI_TOOLS_MSWIN_HELPER_H__

#ifdef _WIN32

#include <ci/compat.h>

#include <stdio.h>
#include <assert.h>
#include <winsock2.h>
#include <ws2spi.h>
#include <mswsock.h>
#include <limits.h>

const char* ci_mswinh_get_wsaerrstr(int err);


char* ci_mswinh_dump_wsaprotocol_info(LPWSAPROTOCOL_INFOW lpProtocolInfo, char* buf, int buf_size);

char* ci_mswinh_dump_event_mask(long event_mask, char* buf, int buf_size);

char* ci_mswinh_dump_guid_to_str(const GUID* guid, char* buf, int buf_size);

char* ci_mswinh_dump_sockaddr(const struct sockaddr* saddr, int saddrlen, char* buf, int buf_size);

char* ci_mswinh_rxtx_flags(DWORD flags, char* buf, int buf_size);
char* ci_mswinh_txfile_flags(DWORD flags, char* buf, int buf_size);

const char* ci_mswinh_get_sock_opt_level_str(int level);

const char* ci_mswinh_get_sock_opt_name_str(int level, int optname);
const char* ci_mswinh_get_ioctl_code_str(DWORD ioctl_code);


#endif
#endif
/*! \cidoxg_end */
