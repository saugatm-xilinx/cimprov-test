##########################################################################
#//#! \file ./config/local.mk
## <L5_PRIVATE L5_SOURCE>
## \author  OktetLabs
##  \brief  CIM Provider
##   \date  2013/10/02
##    \cop  (c) Solarflare Communications Inc.
## </L5_PRIVATE>
##
##########################################################################

include $(TOP)/mk/host.mk
CIMPLE_PLATFORM = $(HOST_CIMPLE_PLATFORM)
PROVIDER_PLATFORM = linux
PROVIDER_PLATFORM_VARIANT = intree
CIM_SERVER=pegasus
CIM_INTERFACE=cmpi
override INTREE_PEGASUS=1
