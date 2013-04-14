/**************************************************************************\
*//*! \file
** <L5_PRIVATE L5_HEADER >
** \author  cgg
**  \brief  Configuration options for iscsi
**   \date  2006/10/21
**    \cop  (c) Solarflare Communications
** </L5_PRIVATE>
*//*
\**************************************************************************/

/*! \cidoxg_include_ci_internal  */

#ifndef __CI_INTERNAL_ISCSI_CONFIG_OPT_H__
#define __CI_INTERNAL_ISCSI_CONFIG_OPT_H__

/* This header is solely for configuration/compilation options!!

/* DO NOT place anything other than CPP directives (e.g. no C) in this file */


/*! If this is 0 we link the iSCSI device on Windows using the WDM driver,
    otherwise we link to the new Windows bus driver
    - we now always want to use the bus driver when __ci_wdf__ is set
*/
#ifdef __ci_wdf__
#define CI_CFG_WIN_ISCSI_LINK_TO_BUS     1
#else
#define CI_CFG_WIN_ISCSI_LINK_TO_BUS     0
#endif


#endif /* __CI_INTERNAL_ISCSI_CONFIG_OPT_H__ */
