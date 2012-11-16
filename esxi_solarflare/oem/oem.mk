
OEM_MODULES=solarflare

include $(OEM_DIR)/solarflare.inc

solarflare: $(CIMDE_PKG_DSTS) $(oem-NON_CIMDE_DEPS) $(solarflare-base_PREP_DEPS) $(solarflare-base_BUILD_DEPS) $(solarflare-base_PKG_DEPS)
	cd $(OEM_DIR) ; \
	$(MAKE) -f $@.make
