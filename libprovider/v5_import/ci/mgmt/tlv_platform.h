/**************************************************************************\
*//*! \file
** <L5_PRIVATE L5_SOURCE>
** \author  ajr
**  \brief  Platform specific TLV definitions
**   \date  2013/11/14
**    \cop  (c) Solarflare Communications Inc.
** </L5_PRIVATE>
*//*
\**************************************************************************/

/* Default values for BIU_TRGT_CTL_REG fields */
#define DEFAULT_BIU_PF_APER_HUNT     (0x17)  /* PF BAR2 (VI window) */
#define DEFAULT_BIU_VF_APER_HUNT     (0x14)  /* VF BAR0 (VI window) */
#define DEFAULT_BIU_INT_APER_HUNT    (0xE)   /* PF BAR4 and VF BAR2 (MSI-X) */

/* Default values for BAR Mask sizes */

/* The core requires I/O apertures to be 256 bytes (SF-105340-TC 5.1.7.11.5)
 * so do that  */
#define DEFAULT_PF_BAR0_APERTURE_HUNT    (0x8)
#define DEFAULT_PF_BAR2_APERTURE_HUNT    DEFAULT_BIU_PF_APER_HUNT

#define DEFAULT_VF_BAR0_APERTURE_HUNT    DEFAULT_BIU_VF_APER_HUNT
