##########################################################################
##! \file ./config/native-vmware/default-64.mk
## <L5_PRIVATE L5_SCRIPT>
## \author  Bhupendra
##  \brief  ESXi Native 6.8 configuration
##   \date  2018/11/21
##    \cop  (c) Solarflare Communications Inc.
## </L5_PRIVATE>
##
##########################################################################
# <example>
override CIM_SERVER = esxi_native
override IS_ESXI = 1
override CIMPLE_PLATFORM = LINUX_IX86_GNU
override BITNESS = 64
override HOST_CIMPLE_PLATFORM = $(CIMPLE_PLATFORM)
override PROVIDER_PLATFORM = vmware
override CROSS_BUILD = 1
# </example>
