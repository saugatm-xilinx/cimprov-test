##########################################################################
##! \file ./config/local.mk
## <L5_PRIVATE L5_SCRIPT>
## \author  OktetLabs
##  \brief  Local development configuration
##   \date  2013/10/02
##    \cop  (c) Solarflare Communications Inc.
## </L5_PRIVATE>
##
##########################################################################

include $(TOP)/mk/host.mk

## \file
## <exmaple>
CIMPLE_PLATFORM = $(HOST_CIMPLE_PLATFORM)
PROVIDER_PLATFORM = linux
PROVIDER_PLATFORM_VARIANT = intree
CIM_SERVER = pegasus
CIM_INTERFACE = cmpi
override INTREE_PEGASUS = 1
override PEGASUS_MANAGEABLE = 1
## </example>