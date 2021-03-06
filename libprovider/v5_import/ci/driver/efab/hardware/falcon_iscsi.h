  /**************************************************************************\
*//*! \file
   ** <L5_PRIVATE L5_HEADER >
   ** \author  mjs
   **  \brief  EtherFabric NIC - EFXXXX (aka Falcon) iSCSI interface
   **   \date  2006/05
   **    \cop  (c) Level 5 Networks Limited.
   ** </L5_PRIVATE>
      *//*
        \************************************************************************* */

/*! \cidoxg_include_ci_driver_efab_hardware  */

#ifndef __CI_DRIVER_EFAB_HARDWARE_FALCON_ISCSI_H__
#define __CI_DRIVER_EFAB_HARDWARE_FALCON_ISCSI_H__

/*----------------------------------------------------------------------------
 *
 * Interface to hardware configuration for iSCSI digest offload
 *
 *---------------------------------------------------------------------------*/

extern void falcon_iscsi_update_tx_q_flags(struct efhw_nic * nic, uint dmaq,
					   uint flags);

extern void falcon_iscsi_update_rx_q_flags(struct efhw_nic * nic, uint dmaq,
					   uint flags);

#endif /* __CI_DRIVER_EFAB_HARDWARE_FALCON_ISCSI_H__ */

/*! \cidoxg_end */
