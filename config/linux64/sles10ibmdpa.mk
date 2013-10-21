##########################################################################
#//#! \file ./config/linux64/sles10ibmdpa.mk
## <L5_PRIVATE L5_SOURCE>
## \author  OktetLabs
##  \brief  CIM Provider
##   \date  2013/10/02
##    \cop  (c) Solarflare Communications Inc.
## </L5_PRIVATE>
##
##########################################################################

include $(TOP)/mk/sles10.mk
include $(TOP)/mk/ibm.mk
override SLES10_BUILD_HOST=ibm360m4b
override HOST_CIMPLE_PLATFORM=LINUX_X86_64_GNU
override PROVIDER_PLATFORM_VARIANT=sles10ibmdpa
override BITNESS=64
