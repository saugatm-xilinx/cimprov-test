include mk/rules-reg.mk

register: repository.reg interop.reg install
	$(SFCBSTAGE) -n $(IMP_NAMESPACE) -r repository.reg repository.mof
	$(SFCBSTAGE) -n $(INTEROP_NAMESPACE) -r interop.reg repository.mof
	$(SFCBREPOS)

ifeq ($(SFCB_ROOT),)
$(error No path to SFCB installation)
endif

PROVIDER_LIBPATH=$(SFCB_ROOT)/lib

endif
