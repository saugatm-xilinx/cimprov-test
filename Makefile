##########################################################################
#//#! \file Makefile
## <L5_PRIVATE L5_SOURCE>
## \author  OktetLabs
##  \brief  CIM Provider
##   \date  2013/10/02
##    \cop  (c) Solarflare Communications Inc.
## </L5_PRIVATE>
##
##########################################################################

TOP ?= .
include $(TOP)/mk/def.mk

.DEFAULT_GOAL := all

ifneq ($(PRESET),)
include $(TOP)/presets/$(PRESET).mk
$(warning Presets are obsolete, use 'make CONFIG=$(CONFIG)' instead)
endif

include $(TOP)/config/$(CONFIG).mk

include $(TOP)/mk/vars.mk
ifeq ($(CIM_SERVER),wmi)
include $(TOP)/mk/wmi.mk
endif
ifeq ($(PROVIDER_PLATFORM),windows)
include $(TOP)/mk/windows$(PROVIDER_BITNESS).mk
include $(TOP)/cimple/posix/sources.mk
endif

.PHONY: all

include $(TOP)/cimple/lib/sources.mk
include $(TOP)/cimple/tools/lib/sources.mk
ifeq ($(CIM_SERVER),sfcb)
include $(TOP)/mk/sfcb.mk
else ifeq ($(CIM_SERVER),esxi)
include $(TOP)/mk/esxi.mk
else ifeq ($(CIM_SERVER),pegasus)
include $(TOP)/pegasus/sources.mk
ifeq ($(CIM_INTERFACE),pegasus)
include $(TOP)/cimple/pegasus/sources.mk

PEGASUS_TOOLS_DEPS = libtoolstgt libcimplepeg 
include $(TOP)/cimple/tools/regmod/sources.mk
ifeq ($(SLES10_BUILD_HOST),)
include $(TOP)/cimple/tools/regview/sources.mk
include $(TOP)/cimple/tools/ciminvoke/sources.mk
include $(TOP)/cimple/tools/cimlisten/sources.mk
endif
endif
else ifeq ($(CIM_SERVER),wmi)
# do nothing
else
$(error Unknown CIM_SERVER: $(CIM_SERVER))
endif
ifeq ($(CIM_INTERFACE),cmpi)
include $(TOP)/cimple/cmpi/sources.mk
endif
ifeq ($(PROVIDER_PLATFORM),windows)
include $(TOP)/cimple/wmi/sources.mk
endif

include $(TOP)/cimple/tools/file2c/sources.mk
include $(TOP)/cimple/tools/libmof/sources.mk
include $(TOP)/cimple/tools/libgencommon/sources.mk
include $(TOP)/cimple/tools/genclass/sources.mk
include $(TOP)/cimple/tools/genmod/sources.mk
include $(TOP)/cimple/tools/genprov/sources.mk

.PHONY : bootstrap

CLASSLIST = $(libcimobjects_DIR)/classes

bootstrap : $(CLASSLIST) $(genmod_TARGET) $(genprov_TARGET)
	$(abspath $(genprov_TARGET))  -F$<

include $(TOP)/libcimobjects/sources.mk

include $(TOP)/libprovider/sources.mk
ifeq ($(CIM_SERVER),esxi)
include $(TOP)/esxi_solarflare/sources.mk
endif

ifneq ($(SLES10_BUILD_HOST),)
include $(TOP)/sles10/sources.mk
endif

top_LDFLAGS += -Wl,-rpath=$(PROVIDER_LIBPATH)


.PHONY: register unregister install


ifneq ($(CIM_SERVER),esxi)
ifneq ($(SLES10_BUILD_HOST),)
all : $(sles10_archive_TARGET)
	$(warning This target is deprecated, consider using "make dist")
else
all : $(libprovider_TARGET)
endif
else

ifeq ($(ESXI_BUILD_HOST),)

all : $(esxi_archive_TARGET)
	$(warning This target is deprecated, consider using "make dist")

else

all : $(esxi_vib_TARGET)

endif

endif

ifneq ($(CIM_SERVER),esxi)
install : all
	$(RUNASROOT) mkdir -p $(DESTDIR)$(PROVIDER_LIBPATH)
	$(RUNASROOT) cp lib$(PROVIDER_LIBRARY).so $(DESTDIR)$(PROVIDER_LIBPATH)
endif

ifneq ($(PROVIDER_ROOT),)
install-aux : repository.reg $(libcimobjects_DIR)/namespace.mof $(libcimobjects_DIR)/schema.mof \
			  $(libcimobjects_DIR)/interop.mof \
			  $(if $(NEED_ASSOC_IN_ROOT_CIMV2),$(libcimobjects_DIR)/root.mof)
	mkdir -p $(DESTDIR)$(PROVIDER_ROOT)/mof
	cp $^ $(DESTDIR)$(PROVIDER_ROOT)/mof
endif

.PHONY : platform
PLATFORM_BUILD = build/$(PROVIDER_PLATFORM)$(PROVIDER_BITNESS)/$(PROVIDER_PLATFORM_VARIANT)-$(CIM_INTERFACE)-$(CIM_SCHEMA_VERSION_MAJOR).$(CIM_SCHEMA_VERSION_MINOR)
platform : $(PLATFORM_BUILD)/Makefile

$(PLATFORM_BUILD)/Makefile : $(TOP)/mk/platform-tpl.mk $(MAKEFILE_LIST)
		mkdir -p $(PLATFORM_BUILD)
		cd $(PLATFORM_BUILD); $(HG) manifest | xargs -n1 dirname | sort -u | xargs -n1 mkdir -p
		echo "CONFIG:=$(CONFIG)" >$@
		echo "TOP:=$(CURDIR)" >>$@
		cat $< >>$@

include $(TOP)/mk/rules.mk
