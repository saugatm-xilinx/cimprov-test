##########################################################################
##! \file ./mk/sles10.mk
## <L5_PRIVATE L5_SCRIPT>
## \author  OktetLabs
##  \brief  CIM Provider
##   \date  2013/10/02
##    \cop  (c) Solarflare Communications Inc.
## </L5_PRIVATE>
##
##########################################################################

override CIM_SERVER = pegasus
override CIM_INTERFACE = cmpi
override CIMPLE_PLATFORM = $(HOST_CIMPLE_PLATFORM)
override PEGASUS_ROOT = /tmp/pegasus
override PEGASUS_HOME = /tmp/pegasus
override PROVIDER_ROOT = /tmp/solarflare
override CIM_SCHEMA_DIR = $(abspath $(CIM_SCHEMA_PATCHDIR))
override PROVIDER_PLATFORM = linux
override PROVIDER_PLATFORM_VARIANT = sles10
override DIST_IS_SPECIAL = 1
override CROSS_BUILD = 1
RUNASROOT = sudo 
CURL = curl -u$(LOGNAME)
