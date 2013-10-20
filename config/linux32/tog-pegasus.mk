##########################################################################
#//#! \file ./config/linux32/tog-pegasus.mk
## <L5_PRIVATE L5_SOURCE>
## \author  OktetLabs
##  \brief  CIM Provider
##   \date  2013/10/02
##    \cop  (c) Solarflare Communications Inc.
## </L5_PRIVATE>
##
##########################################################################

include $(TOP)/mk/host.mk
override PROVIDER_BITNESS=32
include $(TOP)/mk/tog-pegasus.mk
