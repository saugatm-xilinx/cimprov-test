##########################################################################
#//#! \file ./config/linux32/ibmdpa.mk
## <L5_PRIVATE L5_SOURCE>
## \author  OktetLabs
##  \brief  CIM Provider
##   \date  2013/10/02
##    \cop  (c) Solarflare Communications Inc.
## </L5_PRIVATE>
##
##########################################################################

include $(TOP)/mk/ibm.mk
override HOST_CIMPLE_PLATFORM=LINUX_IX86_GNU
override BITNESS=32

