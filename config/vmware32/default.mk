##########################################################################
#//#! \file ./config/vmware32/default.mk
## <L5_PRIVATE L5_SOURCE>
## \author  OktetLabs
##  \brief  CIM Provider
##   \date  2013/10/02
##    \cop  (c) Solarflare Communications Inc.
## </L5_PRIVATE>
##
##########################################################################

override CIM_SERVER=esxi
override CIMPLE_PLATFORM=LINUX_IX86_GNU
override BITNESS=32
override HOST_CIMPLE_PLATFORM=$(CIMPLE_PLATFORM)
override PROVIDER_PLATFORM=vmware
override CROSS_BUILD = 1
