include mk/rules-	reg.mk


ifeq ($(SFCB_ROOT),)
$(error No path to SFCB installation)
endif

PROVIDER_LIBPATH=$(SFCB_ROOT)/lib

endif
