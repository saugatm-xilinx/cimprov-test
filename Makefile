include mk/def.mk

.DEFAULT_GOAL := all

PRESET ?= default
include presets/$(PRESET).mk

include mk/vars.mk
ifeq ($(CIM_SERVER),wmi)
include mk/wmi.mk
include cimple/posix/sources.mk
endif

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

PEGASUS_TOOLS_DEPS = libtoolstgt libcimplepeg 
include cimple/tools/regmod/sources.mk
ifeq ($(SLES10_BUILD_HOST),)
include cimple/tools/regview/sources.mk
include cimple/tools/ciminvoke/sources.mk
include cimple/tools/cimlisten/sources.mk
endif
else ifeq ($(CIM_SERVER),wmi)
# do nothing
else
$(error Unknown CIM_SERVER: $(CIM_SERVER))
endif
ifeq ($(CIM_INTERFACE),cmpi)
include cimple/cmpi/sources.mk
endif
ifeq ($(CIM_INTERFACE),wmi)
include cimple/wmi/sources.mk
# include wmihelper/sources.mk
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

ifneq ($(SLES10_BUILD_HOST),)
include sles10/sources.mk
endif

top_LDFLAGS += -Wl,-rpath=$(PROVIDER_LIBPATH)


.PHONY: register unregister install


ifneq ($(CIM_SERVER),esxi)
ifneq ($(SLES10_BUILD_HOST),)
all : $(sles10_archive_TARGET)
else
all : $(libprovider_TARGET)
endif
else

ifeq ($(ESXI_BUILD_HOST),)

all : $(esxi_archive_TARGET)

else

all : $(esxi_vib_TARGET)

endif

endif

ifneq ($(CIM_SERVER),esxi)
install : all
	mkdir -p $(DESTDIR)$(PROVIDER_LIBPATH)
	cp lib$(PROVIDER_LIBRARY).so $(DESTDIR)$(PROVIDER_LIBPATH)
endif

ifneq ($(PROVIDER_ROOT),)
install-aux : $(libcimobjects_DIR)/repository.mof regmod
	mkdir -p $(DESTDIR)$(PROVIDER_ROOT)/bin
	cp regmod $(DESTDIR)$(PROVIDER_ROOT)/bin
	mkdir -p $(DESTDIR)$(PROVIDER_ROOT)/mof
	cp $(libcimobjects_DIR)/repository.mof $(DESTDIR)$(PROVIDER_ROOT)/mof
endif

include mk/rules.mk
