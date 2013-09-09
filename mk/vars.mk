CIM_SCHEMA_VERSION_MAJOR ?= 2
CIM_SCHEMA_VERSION_MINOR ?= 26
CIM_SCHEMA_VERSION_PATCHLEVEL ?= 0
CIM_SCHEMA_VERSION_EXPERIMENTAL ?= 1

CIM_SCHEMA_PATCHDIR = $(TOP)/schemas/$(CIM_SCHEMA_VERSION_MAJOR).$(CIM_SCHEMA_VERSION_MINOR)$(if $(CIM_SCHEMA_VERSION_EXPERIMENTAL),Exp)

CIM_SCHEMA_ZIP=cim_schema_$(CIM_SCHEMA_VERSION_MAJOR).$(CIM_SCHEMA_VERSION_MINOR).$(CIM_SCHEMA_VERSION_PATCHLEVEL)$(if $(CIM_SCHEMA_VERSION_EXPERIMENTAL),Experimental,Final)-MOFs.zip

DEFAULT_UPSTREAM_REPOSITORY ?= https://oktetlabs.ru/purgatorium/prj/level5/cim

CIM_SCHEMA_REPOSITORY=$(DEFAULT_UPSTREAM_REPOSITORY)

ifeq ($(CIM_SERVER),wmi)
IMP_NAMESPACE=root/cimv2
else
IMP_NAMESPACE=root/solarflare
endif
ifeq ($(CIM_SERVER),pegasus)
INTEROP_NAMESPACE=root/pg_interop
else
INTEROP_NAMESPACE=root/interop
endif
INTEROP_CLASSES=SF_RegisteredProfile SF_ReferencedProfile SF_ElementConformsToProfile

PROVIDER_LIBRARY=Solarflare
PROVIDER_VERSION_MAJOR=1
PROVIDER_VERSION_MINOR=0
PROVIDER_VERSION=$(PROVIDER_VERSION_MAJOR).$(PROVIDER_VERSION_MINOR)
PROVIDER_REVISION=0.0
PROVIDER_PLATFORM_VARIANT ?= generic

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

