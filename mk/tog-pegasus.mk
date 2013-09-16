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
PROVIDER_PLATFORM = linux
PROVIDER_PLATFORM_VARIANT = tog-pegasus
CIM_SERVER=pegasus
CIM_INTERFACE=cmpi
