CPPFLAGS += -DCIMPLE_CMPI_MODULE
ifneq ($(CIM_SERVER),esxi)
CPPFLAGS += -Icmpi
endif

include cimple/cmpi/sources.mk

