##########################################################################
##! \file ./mk/ibm.mk
## <L5_PRIVATE L5_SOURCE>
## \author  OktetLabs
##  \brief  Configuration for IBM linux systems
##   \date  2013/10/02
##    \cop  (c) Solarflare Communications Inc.
## </L5_PRIVATE>
##
##########################################################################
## <example>
override CIM_SERVER = pegasus
override CIM_INTERFACE = cmpi
override CIMPLE_PLATFORM = $(HOST_CIMPLE_PLATFORM)
override PEGASUS_ROOT = /usr
override PEGASUS_HOME = /opt/ibm/icc
override PROVIDER_ROOT = /opt/solarflare
override CIM_SCHEMA_DIR = $(abspath $(CIM_SCHEMA_PATCHDIR))
override PROVIDER_PLATFORM = linux
override PROVIDER_PLATFORM_VARIANT = ibmdpa
## </example>
CURL = curl -u$(LOGNAME)

PROVIDER_RPM_REQUIRES = ibmcimom
CIMOM_SERVICE_NAME = cimserverd
CIMMOFR = $(PEGASUS_BINPATH)/cimmofr
ROOT_NAMESPACE = root/ibmse