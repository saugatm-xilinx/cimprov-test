##########################################################################
##! \file ./config/linux64/sles10.mk
## <L5_PRIVATE L5_SCRIPT>
## \author  OktetLabs
##  \brief  Configuration for 64-bit SLES 10 Linux
##   \date  2013/10/02
##    \cop  (c) Solarflare Communications Inc.
## </L5_PRIVATE>
##
##########################################################################
# <example>
include $(TOP)/mk/sles10.mk
override SLES10_BUILD_HOST = ibm360m4b
override HOST_CIMPLE_PLATFORM = LINUX_X86_64_GNU
override BITNESS = 64
# </example>