##########################################################################
##! \file ./mk/host.mk
## <L5_PRIVATE L5_SCRIPT>
## \author  OktetLabs
##  \brief  CIM Provider
##   \date  2013/10/02
##    \cop  (c) Solarflare Communications Inc.
## </L5_PRIVATE>
## 
##########################################################################

##! Host system platform name for CIMPLE
## Suppored is one of:
## - LINUX_IX86_GNU
## - LINUX_X86_64_GNU
## \note It is the platform where the provider is developped, not
## where it runs. It affects build-time CIMPLE components only.
ifeq ($(HOST_CIMPLE_PLATFORM),)
ifeq ($(origin BITNESS), undefined)
##! Host system bitness 
BITNESS := $(shell getconf LONG_BIT)
endif
ifeq ($(BITNESS),32)
HOST_CIMPLE_PLATFORM = LINUX_IX86_GNU
else
ifeq ($(BITNESS),64)
HOST_CIMPLE_PLATFORM = LINUX_X86_64_GNU
else
$(error Platform is not specified and cannot be guessed)
endif
endif
$(info Platform detected as $(HOST_CIMPLE_PLATFORM))
endif
