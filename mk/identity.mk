##########################################################################
##! \file ./mk/identity.mk
## <L5_PRIVATE L5_SCRIPT>
## \author  OktetLabs
##  \brief  CIM Provider
##   \date  2013/10/02
##    \cop  (c) Solarflare Communications Inc.
## </L5_PRIVATE>
##
##########################################################################

##! Base name for CIM provider library (many other values depend on it)
PROVIDER_LIBRARY = solarflare_nic_provider

##! Base name for a provider package
PROVIDER_NAME = solarflare-cim-provider

##! Below variables PROVIDER_VERSION_MAJOR, PROVIDER_VERSION_MINOR &
##! PROVIDER_REVISION are updated by scripts/createtag.
##! Major version of the CIM provider
PROVIDER_VERSION_MAJOR = 2

##! Minor version of the CIM provider
PROVIDER_VERSION_MINOR = 1
PROVIDER_VERSION = $(PROVIDER_VERSION_MAJOR).$(PROVIDER_VERSION_MINOR)

##! Revision string (#.#) of the CIM provider
PROVIDER_REVISION = 0.26
VIB_ID = Solarflare_bootbank_$(PROVIDER_NAME)_$(PROVIDER_VERSION)-$(PROVIDER_REVISION)

##! Description string of the CIM provider
PROVIDER_DESCRIPTION = Solarflare NIC CIM Provider

##! Vendor name for the CIM provider
PROVIDER_VENDOR = Solarflare Communications

##! Short name of the vendor of the CIM provider
PROVIDER_VENDOR_SHORT = Solarflare
PROVIDER_VENDOR_VMWARE_CODE = SFC

##! Support email of the CIM provider vendor
PROVIDER_VENDOR_EMAIL = support@solarflare.com
PROVIDER_RELEASE_DATE = $(shell date +%Y-%m-%dT%H:%M:%S%:z)
