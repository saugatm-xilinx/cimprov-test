##########################################################################
##! \file ./config/linux32/sles10ibmdpa.mk
## <L5_PRIVATE L5_SCRIPT>
## \author  OktetLabs
##  \brief  32-bit SLES 10 Linux with IBM DPA
##   \date  2013/10/02
##    \cop  (c) Solarflare Communications Inc.
## </L5_PRIVATE>
##
##########################################################################
# <example>
include $(TOP)/mk/sles10.mk
include $(TOP)/mk/ibm.mk
override SLES10_BUILD_HOST=ibm360m4b
override HOST_CIMPLE_PLATFORM=LINUX_IX86_GNU
override PROVIDER_PLATFORM_VARIANT=sles10ibmdpa
override BITNESS=32
# </example>