##########################################################################
#//#! \file ./esxi_solarflare/oem/oem.mk
## <L5_PRIVATE L5_SOURCE>
## \author  OktetLabs
##  \brief  CIM Provider
##   \date  2013/10/02
##    \cop  (c) Solarflare Communications Inc.
## </L5_PRIVATE>
##
##########################################################################


OEM_MODULES = solarflare

include $(OEM_DIR)/solarflare.inc

solarflare: $(CIMDE_PKG_DSTS) $(oem-NON_CIMDE_DEPS) $(solarflare-base_PREP_DEPS) $(solarflare-base_BUILD_DEPS) $(solarflare-base_PKG_DEPS)
	cd $(OEM_DIR) ; \
	$(MAKE) -f $@.make
