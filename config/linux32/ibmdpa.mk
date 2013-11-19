##########################################################################
##! \file ./config/linux32/ibmdpa.mk
## <L5_PRIVATE L5_SCRIPT>
## \author  OktetLabs
##  \brief  Configuration for 32-bit linux with IBM DPA
##   \date  2013/10/02
##    \cop  (c) Solarflare Communications Inc.
## </L5_PRIVATE>
##
##########################################################################
include $(TOP)/mk/ibm.mk

##! \file
# <example>
override HOST_CIMPLE_PLATFORM = LINUX_IX86_GNU
override BITNESS = 	32
# </example>
