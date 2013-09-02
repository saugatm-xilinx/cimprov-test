ifeq ($(HOST_CIMPLE_PLATFORM),)
ifeq ($(origin BITNESS), undefined)
BITNESS:=$(shell getconf LONG_BIT)
endif
ifeq ($(BITNESS),32)
HOST_CIMPLE_PLATFORM=LINUX_IX86_GNU
else
ifeq ($(BITNESS),64)
HOST_CIMPLE_PLATFORM=LINUX_X86_64_GNU
else
$(error Platform is not specified and cannot be guessed)
endif
endif
$(info Platform detected as $(HOST_CIMPLE_PLATFORM))
endif
