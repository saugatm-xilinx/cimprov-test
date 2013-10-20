##########################################################################
#//#! \file ./config/windows64/wmi.mk
## <L5_PRIVATE L5_SOURCE>
## \author  OktetLabs
##  \brief  CIM Provider
##   \date  2013/10/02
##    \cop  (c) Solarflare Communications Inc.
## </L5_PRIVATE>
##
##########################################################################

override PROVIDER_BITNESS=64
include $(TOP)/mk/host.mk

CIM_SERVER = wmi
CIM_INTERFACE = wmi
PROVIDER_PLATFORM = windows
