/* 
 * This is NOT the original source file. Do NOT edit it.
 * To update the board and firmware ids, please edit the copy in
 * the sfregistry repo and then, in that repo,
 * make -C doc firmware_ids
 */
/**************************************************************************\
*//*! \file
** <L5_PRIVATE L5_SOURCE>
** \author  mjs
**  \brief  Solarflare board type enumerations
**   \date  2009/07/16
**    \cop  (c) Solarflare Communications Inc.
** </L5_PRIVATE>
*//*
\**************************************************************************/

#ifndef CI_MGMT_BOARD_IDS_H
#define CI_MGMT_BOARD_IDS_H

/* Reference: SF-103611-PS */

enum {
  /* Falcon-based */
  BOARD_TYPE_SFE4001 = 0x0,
  BOARD_TYPE_SFE4002 = 0x1,
  BOARD_TYPE_SFE4003 = 0x2,
  BOARD_TYPE_SFE4005 = 0x4,
  BOARD_TYPE_SFE4007 = 0x5,
  BOARD_TYPE_SFN4111T = 0x51,
  BOARD_TYPE_SFN4112F = 0x52,
  /* Siena-based */
  BOARD_TYPE_SFE4008 = 0x6,
  BOARD_TYPE_SFN5122F = 0x7, /* QT2025-based variants (R0,R1,R2) */
  BOARD_TYPE_SFN5123H = 0x8,
  BOARD_TYPE_SFN5121T = 0x9,
  BOARD_TYPE_EVT_TDB = 0xa,
  BOARD_TYPE_SFN5322F = 0xb, /* PTP NIC with FPGA */
  /* (0xc reserved to maintain the current correspondence between
   * board type IDs and firmware subtypes for Siena-based boards)
   */
  BOARD_TYPE_SFN5122F_XFI = 0xd, /* XFI-based variant (R3) - different f/w */
  BOARD_TYPE_SFN5802K = 0xe,     /* HP blade NIC */
  BOARD_TYPE_SFN5802K_R2 = 0xf,
  BOARD_TYPE_SFN6822F = 0x10,

  /* Huntington-based */
  BOARD_TYPE_SFN7122F = 0x11,
  BOARD_TYPE_EVT_FARMINGDALE = 0x12,
  BOARD_TYPE_EVT_GREENPORT  = 0x13,
  BOARD_TYPE_SFN7812KCHT  = 0x14,
  BOARD_TYPE_SFN7811KCHT  = 0x15,
  BOARD_TYPE_SFN7141Q  = 0x16,
  BOARD_TYPE_SFN7800K  = 0x17, /* KR enabled SFN7122F */
  BOARD_TYPE_SFA7942Q = 0x18, /* Sorrento */
  BOARD_TYPE_SFN7x42Q = 0x19, /* Monza */
  BOARD_TYPE_SFN7xx4F = 0x20, /* Pavia */

  /* Medford-based */
  BOARD_TYPE_EVT_MEDFORD = 0x21,
  BOARD_TYPE_SFN8022 = 0x22, /* Emerald */
  BOARD_TYPE_SFN8042 = 0x23, /* Newport */
  BOARD_TYPE_SFN8722 = 0x24, /* Malaga */
  BOARD_TYPE_SFN8022_R3 = 0x25, /* Emma */
  BOARD_TYPE_SFN8042_R3 = 0x26, /* Pagnell */
  BOARD_TYPE_SFN8041 = 0x2b, /* Talbot */

  /* Medford 2-based */
  BOARD_TYPE_EVT_MEDFORD2 = 0x27,
  BOARD_TYPE_SFN2522 = 0x28, /* Bradford */
  BOARD_TYPE_SFN2542 = 0x29, /* Bingley */
  BOARD_TYPE_SFN2722 = 0x2a, /* Baildon */
  BOARD_TYPE_X2600 = 0x2c, /* Batley */
};

#endif

