##########################################################################
##! \file ./config/vmware32/default.mk
## <L5_PRIVATE L5_SCRIPT>
## \author  smitra
##  \brief  ESXi Native 6.5 configuration
##   \date  2017/07/20
##    \cop  (c) Solarflare Communications Inc.
## </L5_PRIVATE>
##
##########################################################################
# <example>
override CIM_SERVER = esxi_native
override IS_ESXI = 1
override CIMPLE_PLATFORM = LINUX_IX86_GNU
override BITNESS = 32
override HOST_CIMPLE_PLATFORM = $(CIMPLE_PLATFORM)
override PROVIDER_PLATFORM = vmware
override CROSS_BUILD = 1
# </example>
