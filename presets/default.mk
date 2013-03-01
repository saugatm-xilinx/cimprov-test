CIM_SERVER=pegasus

ifeq ($(CIMPLE_PLATFORM),)
userland_bitness=$(shell getconf LONG_BIT)
ifeq ($(userland_bitness),32)
CIMPLE_PLATFORM=LINUX_IX86_GNU
else ifeq ($(userland_bitness),64)
CIMPLE_PLATFORM=LINUX_X86_64_GNU
else
$(error Platform is not specified and cannot be guessed)
endif
$(info Platform detected as $(CIMPLE_PLATFORM))
endif
