CIM_SERVER=pegasus

ifeq ($(CIMPLE_PLATFORM),)
BITNESS?=$(shell getconf LONG_BIT)
ifeq ($(BITNESS),32)
CIMPLE_PLATFORM=LINUX_IX86_GNU
else ifeq ($(BITNESS),64)
CIMPLE_PLATFORM=LINUX_X86_64_GNU
else
$(error Platform is not specified and cannot be guessed)
endif
$(info Platform detected as $(CIMPLE_PLATFORM))
endif
