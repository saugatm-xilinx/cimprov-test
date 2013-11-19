##########################################################################
##! \file ./config/linux64/ibmdpa.mk
## <L5_PRIVATE L5_SCRIPT>
## \author  OktetLabs
##  \brief  CIM Provider
##   \date  2013/10/02
##    \cop  (c) Solarflare Communications Inc.
## </L5_PRIVATE>
##
##########################################################################

include $(TOP)/mk/ibm.mk
override HOST_CIMPLE_PLATFORM=LINUX_X86_64_GNU
override BITNESS=64
