##########################################################################
#//#! \file ./mk/identity.mk
## <L5_PRIVATE L5_SOURCE>
## \author  OktetLabs
##  \brief  CIM Provider
##   \date  2013/10/02
##    \cop  (c) Solarflare Communications Inc.
## </L5_PRIVATE>
##
##########################################################################

PROVIDER_LIBRARY=Solarflare
PROVIDER_NAME=solarflare-cim-provider
PROVIDER_VERSION_MAJOR=1
PROVIDER_VERSION_MINOR=0
PROVIDER_VERSION=$(PROVIDER_VERSION_MAJOR).$(PROVIDER_VERSION_MINOR)
PROVIDER_REVISION=1.0
VIB_ID=Solarflare_bootbank_$(PROVIDER_NAME)_$(PROVIDER_VERSION)-$(PROVIDER_REVISION)
PROVIDER_DESCRIPTION = Solarflare NIC CIM Provider
PROVIDER_VENDOR = Solarflare Communications
PROVIDER_VENDOR_SHORT= Solarflare
PROVIDER_VENDOR_EMAIL = support@solarflare.com
PROVIDER_RELEASE_DATE = 2013-10-17T05:03:09.233000+00:00
