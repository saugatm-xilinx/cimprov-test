override BITNESS=32
include $(TOP)/mk/host.mk

override CIM_SCHEMA_VERSION_MINOR=28
override CIM_SCHEMA_VERSION_EXPERIMENTAL=
override CIM_SERVER = pegasus
override CIM_INTERFACE = cmpi
override PROVIDER_PLATFORM = windows
override INTREE_PEGASUS = 1
override USE_EXISITING_SCHEMA=1
override PEGASUS_SERVER = $(PEGASUS_BINPATH)/cimserver.exe
override PEGASUS_PREBUILT = 1
override PEGASUS_UPSTREAM_VERSION = winbuild
override PEGASUS_SETUP_SUBDIR =