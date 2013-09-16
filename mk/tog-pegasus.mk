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

CIMPLE_PLATFORM = $(HOST_CIMPLE_PLATFORM)
override PROVIDER_PLATFORM = linux
PROVIDER_PLATFORM_VARIANT = tog-pegasus
override CIM_SERVER=pegasus
override CIM_INTERFACE=cmpi

override CIM_SCHEMA_DIR = $(abspath $(CIM_SCHEMA_PATCHDIR))

define PROVIDER_RPM_REQUIRES
Build-Requires: tog-pegasus-devel
Requires: tog-pegasus-libs
endef