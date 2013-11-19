##########################################################################
##! \file ./config/linux32/tog-pegasus.mk
## <L5_PRIVATE L5_SCRIPT>
## \author  OktetLabs
##  \brief  Configuration for 32-bit Linux with OpenPegasus
##   \date  2013/10/02
##    \cop  (c) Solarflare Communications Inc.
## </L5_PRIVATE>
##
##########################################################################

include $(TOP)/mk/host.mk

##! \file
# <example>
override PROVIDER_BITNESS = 32
# </example>

include $(TOP)/mk/tog-pegasus.mk
