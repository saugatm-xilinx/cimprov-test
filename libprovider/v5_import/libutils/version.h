/* sfutils version numbers */

#ifndef __SFUTILS_VERSION_H__
#define __SFUTILS_VERSION_H__

/* These numbers are updated manually, each time the software is "released" */

#define SFU_MAJ_RELEASE_VERSION            (7)
#define SFU_MIN_RELEASE_VERSION            (1)
#define SFU_MAINT_RELEASE_VERSION          (0)
#define SFU_INCREMENTING_VERSION           (1001)
#define SFU_VERSION_STRING                 "v7.1.0.1001"

/* Don't show incrementing version (build number) by default in Windows .rc files */
#define SFU_RC_VERSION_STRING              "v7.1.0"

#define SFU_COPYRIGHT_STRING \
  "Copyright Solarflare Communications 2006-2017, Level 5 Networks 2002-2005 "
#define SFU_COMPANY_NAME                   "Solarflare"
#define SFU_PRODUCT_NAME                   "Solarstorm"

#endif  /* __SFUTILS_VERSION_H__ */
