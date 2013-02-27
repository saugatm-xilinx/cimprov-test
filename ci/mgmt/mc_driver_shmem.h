/**************************************************************************\ 
*//*! \file 
** <L5_PRIVATE L5_SOURCE> 
** \author gnb
**  \brief Driver Command protocol shared-memoory implementation details
**   \date   2008/04/30
**    \cop  (c) Level 5 Networks Limited. 
** </L5_PRIVATE> 
*//* 
\**************************************************************************/

/*! \cidoxg_mgmt_mc_driver_pcol */

#ifndef _SIENA_MC_DRIVER_SHMEM_H
#define _SIENA_MC_DRIVER_SHMEM_H

#include "mc_driver_pcol.h"

/* ============ Managment Controller Driver Interface Offsets */

/* ============ Load and Execute Offsets */

#define MC_TESTEXEC_OFFS (MC_SMEM_P1_PDU_OFST + MC_SMEM_PDU_LEN)
#define MC_TESTEXEC_LEN  (MC_SMEM_P0_STATUS_OFST - MC_TESTEXEC_OFFS)

/*! The last word of shmem is used for (test) MC->host general status */
#define MC_SHMEM_STATUS_WORD_OFFS   MC_SMEM_P1_STATUS_OFST

/*! Copycode buffer length in words */
#define MC_SHMEM_COPYCODE_XFER_WORDS				\
  ((MC_SMEM_P0_STATUS_OFST - MC_TESTEXEC_OFFS) /		\
   FMCR_CC_TREG_SMEM_STEP)

/* ============ Circular debug buffer (used for routing UART output to SHM) */

#define MC_DEBUGBUF_WPTR (MC_TESTEXEC_OFFS)
#define MC_DEBUGBUF_RPTR (MC_TESTEXEC_OFFS + 4)
#define MC_DEBUGBUF_START (MC_TESTEXEC_OFFS + 8)
#define MC_DEBUGBUF_END (MC_SMEM_P0_STATUS_OFST)
#define MC_DEBUGBUF_LEN (MC_DEBUGBUF_END - MC_DEBUGBUF_START)

#endif /* _SIENA_MC_DRIVER_SHMEM_H */
/*! \cidoxg_end */
