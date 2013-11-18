##########################################################################
#//#! \file ./mk/esxi.mk
## <L5_PRIVATE L5_SOURCE>
## \author  OktetLabs
##  \brief  CIM Provider
##   \date  2013/10/02
##    \cop  (c) Solarflare Communications Inc.
## </L5_PRIVATE>
##
##########################################################################

CIM_INTERFACE = cmpi
override CIM_SCHEMA_VERSION_MINOR = 17
CIM_SCHEMA_ADDON_MOFS = DMTF/System/CIM_RecordLogCapabilities.mof
CIM_SCHEMA_DIR = $(abspath $(CIM_SCHEMA_PATCHDIR))
CIM_SCHEMA_ROOTFILE = $(CIM_SCHEMA_DIR)/DMTF/cimv$(CIM_SCHEMA_VERSION_MAJOR)$(CIM_SCHEMA_VERSION_MINOR).mof
override DIST_IS_SPECIAL = 1
override PROVIDER_TARBALL = $(esxi_archive_TARGET)
override target_CPPFLAGS = 
override target_CXXFLAGS = -Wno-unused -Wno-unused-parameter
override target_LDFLAGS = -pthread -lrt
