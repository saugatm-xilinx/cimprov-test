include mk/def.mk

.DEFAULT_GOAL := all

PRESET ?= default
include presets/$(PRESET).mk

include mk/vars.mk

.PHONY: all

include cimple/lib/sources.mk
include cimple/tools/lib/sources.mk
ifeq ($(CIM_SERVER),sfcb)
include mk/sfcb.mk
else ifeq ($(CIM_SERVER),esxi)
include mk/esxi.mk
else ifeq ($(CIM_SERVER),pegasus)
include pegasus/sources.mk
include cimple/pegasus/sources.mk

PEGASUS_TOOLS_DEPS = libtools libcimplepeg 
include cimple/tools/regmod/sources.mk
include cimple/tools/regview/sources.mk
include cimple/tools/ciminvoke/sources.mk
include cimple/tools/cimlisten/sources.mk
else
$(error Unknown CIM_SERVER: $(CIM_SERVER))
endif
ifeq ($(CIM_INTERFACE),cmpi)
include cimple/cmpi/sources.mk
endif

include cimple/tools/file2c/sources.mk
include cimple/tools/libmof/sources.mk
include cimple/tools/libgencommon/sources.mk
include cimple/tools/genclass/sources.mk
include cimple/tools/genmod/sources.mk
include cimple/tools/genprov/sources.mk

.PHONY : bootstrap

CLASSLIST = $(libcimobjects_DIR)/classes

bootstrap : $(CLASSLIST) $(genmod_TARGET) $(genprov_TARGET)
	$(abspath $(genprov_TARGET))  -F$<

include libcimobjects/sources.mk

include libprovider/sources.mk
ifeq ($(CIM_SERVER),esxi)
include esxi_solarflare/sources.mk
endif

top_LDFLAGS += -Wl,-rpath=$(PROVIDER_LIBPATH)


.PHONY: register unregister install


ifneq ($(CIM_SERVER),esxi)
all : $(libprovider_TARGET)

else

all : $(esxi_archive_TARGET)

endif


install : all
	cp lib$(PROVIDER_LIBRARY).so $(PROVIDER_LIBPATH)

include mk/rules.mk