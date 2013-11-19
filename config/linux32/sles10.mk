##########################################################################
##! \file ./config/linux32/sles10.mk
## <L5_PRIVATE L5_SCRIPT>
## \author  OktetLabs
##  \brief  Configuration for 32-bit SLES 10
##   \date  2013/10/02
##    \cop  (c) Solarflare Communications Inc.
## </L5_PRIVATE>
##
##########################################################################

include $(TOP)/mk/sles10.mk

##! \file
# <example>
override SLES10_BUILD_HOST = ibm360m4b
override HOST_CIMPLE_PLATFORM = LINUX_IX86_GNU
override BITNESS = 32
# </example>