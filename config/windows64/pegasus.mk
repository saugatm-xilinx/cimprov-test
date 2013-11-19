##########################################################################
##! \file ./config/windows64/pegasus.mk
## <L5_PRIVATE L5_SCRIPT>
## \author  OktetLabs
##  \brief  CIM Provider
##   \date  2013/10/02
##    \cop  (c) Solarflare Communications Inc.
## </L5_PRIVATE>
##
##########################################################################

override PROVIDER_BITNESS=64
include $(TOP)/mk/host.mk
include $(TOP)/mk/pegasus-win.mk
