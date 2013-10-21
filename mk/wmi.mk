##########################################################################
#//#! \file ./mk/wmi.mk
## <L5_PRIVATE L5_SOURCE>
## \author  OktetLabs
##  \brief  CIM Provider
##   \date  2013/10/02
##    \cop  (c) Solarflare Communications Inc.
## </L5_PRIVATE>
##
##########################################################################

CIM_SCHEMA_DIR = $(abspath $(TOP)/schemas/windows)
override CIM_SCHEMA_VERSION_MINOR = 0
override USE_EXISITING_SCHEMA = 1

