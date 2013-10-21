##########################################################################
#//#! \file ./mk/tog-pegasus.mk
## <L5_PRIVATE L5_SOURCE>
## \author  OktetLabs
##  \brief  CIM Provider
##   \date  2013/10/02
##    \cop  (c) Solarflare Communications Inc.
## </L5_PRIVATE>
##
##########################################################################

ifeq ($(PROVIDER_BITNESS),64)
SYSLIBDIR=lib64
else
SYSLIBDIR=lib
endif
override PEGASUS_ROOT=/usr
override PEGASUS_HOME=/var/lib/Pegasus
override PEGASUS_LIBDIR = $(PEGASUS_ROOT)/lib
override PEGASUS_INCLUDES = $(PEGASUS_ROOT)/include
override PEGASUS_BINPATH = $(PEGASUS_ROOT)/bin
override PEGASUS_SERVER = /usr/sbin/cimserver
override PEGASUS_REPODIR = $(PEGASUS_HOME)/repository
override PROVIDER_LIBPATH = $(PEGASUS_LIBDIR)/Pegasus/providers
override PROVIDER_ROOT = /usr/share/$(PROVIDER_LIBRARY)

CIMPLE_PLATFORM = $(HOST_CIMPLE_PLATFORM)
override PROVIDER_PLATFORM = linux
PROVIDER_PLATFORM_VARIANT = tog-pegasus
override CIM_SERVER=pegasus
override CIM_INTERFACE=cmpi

override CIM_SCHEMA_DIR = $(abspath $(CIM_SCHEMA_PATCHDIR))

PROVIDER_RPM_BUILD_REQUIRES = tog-pegasus-devel
PROVIDER_RPM_REQUIRES = tog-pegasus-libs
