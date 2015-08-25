/**************************************************************************/
/*!  \file  mdio_clause45.h
** \author  bwh
**  \brief  Functions for IEEE 802.3 clause 45 MDIO
**   \date  2008/10/06
**    \cop  Copyright 2008 Solarflare Communications Inc.
*//************************************************************************/

#ifndef SFUTILS_MDIO_CLAUSE45_H
#define SFUTILS_MDIO_CLAUSE45_H

#include <stdint.h>

/* Numbering of the MDIO Manageable Devices (MMDs) */
/* Physical Medium Attachment/ Physical Medium Dependent sublayer */
#define MDIO_MMD_PMAPMD 1
/* WAN Interface Sublayer */
#define MDIO_MMD_WIS	2
/* Physical Coding Sublayer */
#define MDIO_MMD_PCS	3
/* PHY Extender Sublayer */
#define MDIO_MMD_PHYXS	4
/* Extender Sublayer */
#define MDIO_MMD_DTEXS	5
/* Transmission convergence */
#define MDIO_MMD_TC	6
/* Auto negotiation */
#define MDIO_MMD_AN	7
/* Clause 22 extension */
#define MDIO_MMD_C22EXT	29

typedef struct mdio_clause45_context* mdio_clause45_handle;

extern mdio_clause45_handle mdio_clause45_open(const char* ifname, int phy_id);

/* Dummy phy_id for automatic selection using ethtool */
#define MDIO_CLAUSE45_PHY_ID_AUTO (-1)

extern void mdio_clause45_close(mdio_clause45_handle h);

extern int mdio_clause45_read(mdio_clause45_handle h, unsigned dev,
                              uint16_t addr);
extern int mdio_clause45_write(mdio_clause45_handle h, unsigned dev,
                               uint16_t addr, uint16_t value);

#endif /* SFUTILS_MDIO_CLAUSE45_H */
