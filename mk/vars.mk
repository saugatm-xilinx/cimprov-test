##########################################################################
##! \file ./mk/vars.mk
## <L5_PRIVATE L5_SCRIPT>
## \author  OktetLabs
##  \brief  CIM Provider
##   \date  2013/10/02
##    \cop  (c) Solarflare Communications Inc.
## </L5_PRIVATE>
##
##########################################################################

include $(TOP)/mk/identity.mk

PROVIDER_BITNESS ?= $(BITNESS)

##! CIM schema major version
CIM_SCHEMA_VERSION_MAJOR ?= 2

##! CIM schema minor version
## This is what changes mostly when a schema changes
CIM_SCHEMA_VERSION_MINOR ?= 26

##! CIM schema patchlevel version
CIM_SCHEMA_VERSION_PATCHLEVEL ?= 0

##! CIM schema experimental features switch
## Set to 1 if CIM schema includes experimental features
CIM_SCHEMA_VERSION_EXPERIMENTAL ?= 1

##! Directory for CIM schema patches for Pegasus
CIM_SCHEMA_PATCHDIR = $(TOP)/schemas/$(CIM_SCHEMA_VERSION_MAJOR).$(CIM_SCHEMA_VERSION_MINOR)$(if $(CIM_SCHEMA_VERSION_EXPERIMENTAL),Exp)

CIM_SCHEMA_ZIP=cim_schema_$(CIM_SCHEMA_VERSION_MAJOR).$(CIM_SCHEMA_VERSION_MINOR).$(CIM_SCHEMA_VERSION_PATCHLEVEL)$(if $(CIM_SCHEMA_VERSION_EXPERIMENTAL),Experimental,Final)-MOFs.zip
CIM_SCHEMA_ROOTFILE?=$(CIM_SCHEMA_DIR)/CIM_Schema.mof

DEFAULT_UPSTREAM_REPOSITORY ?= https://oktetlabs.ru/purgatorium/prj/level5/cim

##! Repository for CIM schema zip files
CIM_SCHEMA_REPOSITORY=$(DEFAULT_UPSTREAM_REPOSITORY)

ifeq ($(CIM_SERVER),wmi)
##! Principal namespace for CIM provider classes
IMP_NAMESPACE=root/cimv2
else
IMP_NAMESPACE=root/solarflare
endif
##! System-wide root namespace
ROOT_NAMESPACE?=root/cimv2
ifeq ($(CIM_SERVER),pegasus)
##! Interoperability classes namespace
INTEROP_NAMESPACE=root/pg_interop
else
INTEROP_NAMESPACE=root/interop
endif
INTEROP_CLASSES=SF_RegisteredProfile SF_ReferencedProfile SF_ElementConformsToProfile

##! Platform variant tag
PROVIDER_PLATFORM_VARIANT ?= generic
CONFIGTAG = $(subst /,-,$(CONFIG))

##! Name of the provider package (base name for RPM names etc)
PROVIDER_PACKAGE ?= solarflare_provider_ibm
PROVIDER_TARBALL_DIR ?= $(PROVIDER_PACKAGE)-$(CONFIGTAG)-$(PROVIDER_VERSION).$(PROVIDER_REVISION)
PROVIDER_TARBALL = $(PROVIDER_TARBALL_DIR).tar.gz

ifeq ($(MAKECMDGOALS), dist)
ifeq ($(DIST_IS_SPECIAL),)
##! List of all files that are to be packaged into a tarball
## \note The bulk of files comes from Mercurial manifest, so Mercurial must be installed
## for packaging to work
PROVIDER_DIST_FILES := $(addprefix $(TOP)/,$(shell $(HGLISTALL))
PROVIDER_DIST_FILES += $(CIM_SCHEMA_ZIP)
PROVIDER_DIST_FILES += lib$(PROVIDER_LIBRARY).spec
endif
endif

ifeq ($(CIM_SERVER),pegasus)
USE_REGMOD ?= 1
endif

override host_CPPFLAGS += -D_GNU_SOURCE -I$(TOP)

ifneq ($(CIM_SERVER),esxi)
override host_CXXFLAGS += -fPIC  -pthread
override host_CXXFLAGS += -Wall -W -Wno-unused -Wnon-virtual-dtor
override host_CXXFLAGS += -g

ifeq ($(HOST_CIMPLE_PLATFORM),LINUX_IX86_GNU)
override host_CXXFLAGS += -m32
endif

endif
override host_CXXFLAGS += -fvisibility=hidden

ifneq ($(CIM_SERVER),esxi)
override host_CPPFLAGS += -DSF_IMPLEMENTATION_NS=\"$(IMP_NAMESPACE)\"
override host_CPPFLAGS += -DSF_INTEROP_NS=\"$(INTEROP_NAMESPACE)\"
endif

override host_SYSLIBRARIES += pthread

ifeq ($(PROVIDER_PLATFORM), linux)
override host_SYSLIBRARIES += pci
endif

