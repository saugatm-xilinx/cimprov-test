##########################################################################
##! \file 
## <L5_PRIVATE L5_SCRIPT>
## \author  OktetLabs
##  \brief  CIM Provider build system
##   \date  2013/10/02
##    \cop  (c) Solarflare Communications Inc.
## </L5_PRIVATE>
## \ref build-faq
##########################################################################

##! Path to the top of the source tree
TOP ?= .
include $(TOP)/mk/def.mk

.DEFAULT_GOAL := all

##! Configuration preset (user-specified)
## \deprecated Use appropriate CONFIG()
ifneq ($(PRESET),)
include $(TOP)/presets/$(PRESET).mk
$(warning Presets are obsolete, use 'make CONFIG=$(CONFIG)' instead)
endif

ifeq ($(MAKECMDGOALS), doc)
# nothing
else ifeq  ($(MAKECMDGOALS), userdoc)
# nothing
else ifeq  ($(MAKECMDGOALS), clean)
# nothing
else
##! Configuration name (user-specified)
## \sa config/config.dox
ifeq ($(CONFIG),)
$(error No CONFIG is known)
endif
include $(TOP)/config/$(CONFIG).mk
endif

##! Location name (user-specified or provided by CONFIG())
## Locations asemble variables that may vary from site to site
## (such as the URL for schema downloading)
ifneq ($(LOCATION),)
include $(TOP)/config/locations/$(LOCATION).mk
endif

include $(TOP)/mk/vars.mk

##! CIMOM type
## Valid values are:
## - pegasus for OpenPegasus or IBM DPA 
## - sfcb for SFCB (except ESXi) 
## - esxi for ESXi CIMOM 
## - wmi for WMI
ifeq ($(CIM_SERVER),wmi)
include $(TOP)/mk/wmi.mk
endif
##! Provider target platform
## Defined values are
## - windows
## - linux
## - vmware
ifeq ($(PROVIDER_PLATFORM),windows)
include $(TOP)/mk/windows$(PROVIDER_BITNESS).mk
include $(TOP)/cimple/posix/sources.mk
endif

ifeq ($(PROVIDER_PLATFORM),linux)
include $(TOP)/libpciaccess/sources.mk
endif

.PHONY: all

include $(TOP)/cimple/lib/sources.mk
include $(TOP)/cimple/tools/lib/sources.mk
ifeq ($(CIM_SERVER),sfcb)
include $(TOP)/mk/sfcb.mk
else ifeq ($(CIM_SERVER),esxi_native)
include $(TOP)/mk/esxi.mk
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
ifeq ($(MAKECMDGOALS),doc)
# nothing
else ifeq ($(MAKECMDGOALS),userdoc)
# nothing
else ifeq  ($(MAKECMDGOALS), clean)
# nothing
else
$(error Unknown CIM_SERVER: $(CIM_SERVER))
endif
endif

##! CIMOM interface
## Supported values are:
## - pegasus (OpenPegasus C++ API, deprecated)
## - cmpi (default for Linux and VMWare)
## - wmi
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

ifeq ($(PROVIDER_PLATFORM), $(filter $(PROVIDER_PLATFORM),linux vmware))
include $(TOP)/libprovider/v5_import/endianness/sources.mk
include $(TOP)/libprovider/v5_import/tlv/sources.mk
include $(TOP)/libprovider/v5_import/common/sources.mk
endif

include $(TOP)/libprovider/sources.mk

ifeq ($(IS_ESXI), 1)
include $(TOP)/esxi_solarflare/sources.mk
endif

ifneq ($(SLES10_BUILD_HOST),)
include $(TOP)/sles10/sources.mk
endif

top_LDFLAGS += -Wl,-rpath=$(PROVIDER_LIBPATH)


.PHONY: register unregister install


ifneq ($(IS_ESXI), 1)
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

ifneq ($(IS_ESXI), 1)

##! Installs CIM provider where the CIMOM expects to find it
install : all
	$(RUNASROOT) mkdir -p $(DESTDIR)$(PROVIDER_LIBPATH)
	$(RUNASROOT) cp lib$(PROVIDER_LIBRARY).so $(DESTDIR)$(PROVIDER_LIBPATH)
endif

ifneq ($(PROVIDER_ROOT),)
##! Installs CIM provider's supplementary files under PROVIDER_ROOT()
install-aux : repository.reg $(libcimobjects_DIR)/namespace.mof $(libcimobjects_DIR)/schema.mof \
			  $(libcimobjects_DIR)/interop.mof \
			  $(if $(NEED_ASSOC_IN_ROOT_CIMV2),$(libcimobjects_DIR)/root.mof)
	mkdir -p $(DESTDIR)$(PROVIDER_ROOT)/mof
	cp $^ $(DESTDIR)$(PROVIDER_ROOT)/mof
endif

.PHONY : platform

##! The path to the platform-specific build directory
PLATFORM_BUILD = build/$(PROVIDER_PLATFORM)$(PROVIDER_BITNESS)/$(PROVIDER_PLATFORM_VARIANT)-$(CIM_INTERFACE)-$(CIM_SCHEMA_VERSION_MAJOR).$(CIM_SCHEMA_VERSION_MINOR)

##! Adding Repo information to get management Interface file for esxi_native driver
##! Branch argument to provided with make command in following format
##! make CONFIG=native-vmware/default LOCATION=solarflare BRANCH=dev2 platform
ifeq ($(CIM_SERVER),esxi_native)

USRMGMTINTF_BRANCH       = default/native_drv
USRMGMTINTF_URL          = http://source.uk.solarflarecom.com/hg/incoming/esxi_sfc/rawfile
USRMGMT_INCLFILE         = sfvmk_mgmt_interface.h
USRMGMT_SRCFILE          = sfvmk_mgmt_interface.c
USERMGMT_DIR		 = libprovider/userMgmtSrc

ifneq ($(v5_TAG),)
V5IMPORT_DIR             = libprovider/v5_import/common
V5IMPORT_URL             = http://source.uk.solarflarecom.com/hg/incoming/v5/rawfile/$(v5_TAG)/src/driver/common/
V5IMPORT_IMGFILE	 = ef10_image.c
V5IMPORT_CRCFILE	 = efx_crc32.c
V5IMPORT_EFXTYPESFILE	 = efx_types.h
V5IMPORT_EF10_SIMGFILE	 = ef10_signed_image_layout.h
V5IMPORT_EFXANNOTEFILE	 = efx_annote.h
V5IMPORT_EFXCHECKFILE	 = efx_check.h
V5IMPORT_EFXPHYIDFILE	 = efx_phy_ids.h
V5IMPORT_EF10_FIDFILE	 = ef10_firmware_ids.h
V5IMPORT_EF10_IMPLFILE	 = ef10_impl.h
else
$(error  No v5 TAG is known)
endif
endif
##! Creates a platform-specific build directory
platform : $(PLATFORM_BUILD)/Makefile

$(PLATFORM_BUILD)/Makefile : $(TOP)/mk/platform-tpl.mk $(MAKEFILE_LIST)
ifeq ($(CIM_SERVER),esxi_native)
		@wget -q $(USRMGMTINTF_URL)/$(USRMGMTINTF_BRANCH)/$(USRMGMT_INCLFILE)
		@wget -q $(USRMGMTINTF_URL)/$(USRMGMTINTF_BRANCH)/$(USRMGMT_SRCFILE)
		@mkdir -p $(TOP)/$(USERMGMT_DIR)
		@mv $(USRMGMT_INCLFILE) $(TOP)/$(USERMGMT_DIR)/
		@mv $(USRMGMT_SRCFILE) $(TOP)/$(USERMGMT_DIR)/

		@rm -f $(TOP)/$(V5IMPORT_DIR)/$(V5IMPORT_IMGFILE)
		@rm -f $(TOP)/$(V5IMPORT_DIR)/$(V5IMPORT_CRCFILE)
		@rm -f $(TOP)/$(V5IMPORT_DIR)/$(V5IMPORT_EFXTYPESFILE)
		@rm -f $(TOP)/$(V5IMPORT_DIR)/$(V5IMPORT_EF10_SIMGFILE)
		@rm -f $(TOP)/$(V5IMPORT_DIR)/$(V5IMPORT_EF10_FIDFILE)
		@rm -f $(TOP)/$(V5IMPORT_DIR)/$(V5IMPORT_EF10_IMPLFILE)
		@rm -f $(TOP)/$(V5IMPORT_DIR)/$(V5IMPORT_EFXANNOTEFILE)
		@rm -f $(TOP)/$(V5IMPORT_DIR)/$(V5IMPORT_EFXCHECKFILE)
		@rm -f $(TOP)/$(V5IMPORT_DIR)/$(V5IMPORT_EFXPHYIDFILE)

		@wget -q -P $(TOP)/$(V5IMPORT_DIR)  $(V5IMPORT_URL)/$(V5IMPORT_IMGFILE)
		@wget -q -P $(TOP)/$(V5IMPORT_DIR)  $(V5IMPORT_URL)/$(V5IMPORT_CRCFILE)
		@wget -q -P $(TOP)/$(V5IMPORT_DIR)  $(V5IMPORT_URL)/$(V5IMPORT_EFXTYPESFILE)
		@wget -q -P $(TOP)/$(V5IMPORT_DIR)  $(V5IMPORT_URL)/$(V5IMPORT_EF10_SIMGFILE)
		@wget -q -P $(TOP)/$(V5IMPORT_DIR)  $(V5IMPORT_URL)/$(V5IMPORT_EF10_FIDFILE)
		@wget -q -P $(TOP)/$(V5IMPORT_DIR)  $(V5IMPORT_URL)/$(V5IMPORT_EF10_IMPLFILE)
		@wget -q -P $(TOP)/$(V5IMPORT_DIR)  $(V5IMPORT_URL)/$(V5IMPORT_EFXANNOTEFILE)
		@wget -q -P $(TOP)/$(V5IMPORT_DIR)  $(V5IMPORT_URL)/$(V5IMPORT_EFXCHECKFILE)
		@wget -q -P $(TOP)/$(V5IMPORT_DIR)  $(V5IMPORT_URL)/$(V5IMPORT_EFXPHYIDFILE)
endif
		mkdir -p $(PLATFORM_BUILD)
		cd $(PLATFORM_BUILD); $(HGLISTALL) | xargs -n1 dirname | sort -u | xargs -n1 mkdir -p
		echo "CONFIG:=$(CONFIG)" >$@
		echo "LOCATION:=$(LOCATION)" >>$@
		echo "TOP:=$(CURDIR)" >>$@
		cat $< >>$@

.PHONY : doc userdoc

MKDOC = cd $(TOP); mkdir -p doc/$(1); PATH=$(abspath $(TOP))/scripts:$$PATH $(DOXYGEN) $(2)

##! Generates documentation
## \warning This target always operate on TOP directory,
## even when called in a build subdirectory
## \note The target depends only on Doxyfile, not on the whole source tree,
## for technical reasons

doc : doc/Doxyfile $(TOP)/doc/Makefile.dox
	$(call MKDOC,devel,$<)

##! Generates users' documentation
## \warning This target always operate on TOP directory,
## even when called in a build subdirectory
## \note The target depends only on Doxyfile.user, not on the whole source tree,
## for technical reasons

userdoc : doc/Doxyfile.user $(TOP)/doc/Makefile.dox
	$(call MKDOC,user,$<)

$(TOP)/doc/Makefile.dox : Makefile
	$(AWK) -f ./scripts/mk2dox.awk $< >$@

##! removes the build directory.
.PHONY : clean
clean :
	rm -rf build/
	rm -rf $(TOP)/libprovider/userMgmtSrc

ifeq ($(MAKECMDGOALS), clean)
# nothing
else
include $(TOP)/mk/rules.mk
endif
