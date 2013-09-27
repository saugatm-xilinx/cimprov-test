esxi_archive_TARGET = esxi-solarflare.tar.gz
esxi_archive_DIR = esxi_solarflare

ESXI_PROJECT_NAME = solarflare
ESXI_SRC_PATH = $(esxi_archive_DIR)/$(ESXI_PROJECT_NAME)
ESXI_EXTRA_LIBDIR = $(libprovider_DIR)/esxi_libs/i386
ESXI_EXTRA_LIBS = libssh2.a librt.so.1
ESXI_EXTRA_LIBS += libcurl.a libutils.a libsfupdate.a
ifeq ($(ESXI_VERSION),5.5)
LIBSSL_FILE = libssl.so.1.0.1
LIBCRYPTO_FILE = libcrypto.so.1.0.1
else
LIBSSL_FILE = libssl.so.0.9.8
LIBCRYPTO_FILE = libcrypto.so.0.9.8
endif
ESXI_EXTRA_LIBS += $(LIBSSL_FILE) $(LIBCRYPTO_FILE)

esxi_archive_EXTRA_DISTFILES += $(libcimobjects_DIR)/repository.mof \
				                $(libcimobjects_DIR)/interop.mof $(libcimobjects_DIR)/root.mof
esxi_archive_EXTRA_DISTFILES += $(addprefix $(ESXI_EXTRA_LIBDIR)/,$(ESXI_EXTRA_LIBS))

esxi_archive_GENERATED = $(ESXI_PROJECT_NAME)/$(libcimobjects_DIR)/namespace.mof \
						 $(ESXI_PROJECT_NAME)/repository.reg.in \
						 $(ESXI_PROJECT_NAME)/Makefile.am

esxi_archive_SOURCES = esxi_bootstrap.sh \
					   $(ESXI_PROJECT_NAME)/configure.ac \
					   $(ESXI_PROJECT_NAME)/solarflare.wsman \
					   oem/descriptor-urls.xml	\
					   oem/descriptor.xml	\
					   oem/oem.mk \
					   oem/solarflare-build.sh \
					   oem/solarflare-pkg.sh \
					   oem/solarflare-prep.sh \
					   oem/solarflare-vib-depends.xml \
					   oem/solarflare.defs \
					   oem/solarflare.deps \
					   oem/solarflare.inc \
					   oem/solarflare.make \
					   $(esxi_archive_GENERATED)

$(esxi_archive_TARGET) : TAR_TRANSFORM=!^!$(ESXI_PROJECT_NAME)/! !^$(ESXI_PROJECT_NAME)/$(esxi_archive_DIR)/!!

$(eval $(call archive_component,esxi_archive))

$(ESXI_SRC_PATH)/$(libcimobjects_DIR)/namespace.mof : $(libcimobjects_DIR)/namespace.mof \
													   $(addprefix $(CIM_SCHEMA_PATCHDIR)/,$(CIM_SCHEMA_ADDON_MOFS))
	mkdir -p $(dir $@)
	cat $^ >$@

$(ESXI_SRC_PATH)/repository.reg.in : repository.reg
	mkdir -p $(dir $@)	
	$(SED) 's!$(IMP_NAMESPACE)!@smash_namespace@!g; s!$(INTEROP_NAMESPACE)!@sfcb_interop_namespace@!g' <$< >$@

ESXI_CONTENTS = $(patsubst $(TOP)/%,%,$(filter-out $(esxi_archive_DIR)/%,$(esxi_archive_DISTFILES)))

$(esxi_archive_DIR)/m4.defs : $(MAKEFILE_LIST)
	echo "m4_define(\`PROVIDER_LIBRARY', \`$(PROVIDER_LIBRARY)')m4_dnl" >$@
	echo "m4_define(\`SOURCES', \`$(ESXI_CONTENTS)')m4_dnl" >>$@
	echo "m4_define(\`CPPFLAGS', \`$(target_CPPFLAGS)')m4_dnl" >>$@
	echo "m4_define(\`CXXFLAGS', \`$(target_CXXFLAGS)')m4_dnl" >>$@
	echo "m4_define(\`INCLUDES', \`$(foreach comp,$(esxi_archive_COMPONENTS),$($(comp)_CPPFLAGS) )')m4_dnl" >>$@
	echo "m4_define(\`LDADD', \`$(addprefix $(ESXI_EXTRA_LIBDIR)/,$(ESXI_EXTRA_LIBS))')m4_dnl" >>$@

$(ESXI_SRC_PATH)/Makefile.am : $(esxi_archive_DIR)/m4.defs $(esxi_archive_DIR)/Makefile.am.in 
	$(M4) $^ >$@

ifneq ($(ESXI_BUILD_HOST),)

ifneq ($(ESXI_GATEWAY),)
ESXI_CONNECT = ssh $(ESXI_GATEWAY)
ESXI_Q = "
endif

ESXI_BUILD_CIMPDK_PATH ?= /opt/vmware/cimpdk
ESXI_BUILD_SUBDIR ?= $(PROVIDER_LIBRARY)
ESXI_BUILD_DIR = $(ESXI_BUILD_CIMPDK_PATH)/$(ESXI_BUILD_SUBDIR)
ESXI_BUILD_VIB_PATH = build/obj/esx/staging

ESXI_BUILD_RSH=$(ESXI_CONNECT) ssh $(ESXI_BUILD_HOST) #-oBatchMode=yes 
ESXI_BUILD_RCP=scp #-oBatchMode=yes 
ESXI_COPYTO=$(or $(ESXI_GATEWAY),$(ESXI_BUILD_HOST))

esxi_vib_TARGET = vmware-esx-provider-solarflare.vib
esxi_vib_DIR = esxi_solarflare

$(esxi_vib_TARGET) : $(esxi_archive_TARGET)
	$(ESXI_BUILD_RCP) $< $(ESXI_COPYTO):$(notdir $<)
	$(ESXI_BUILD_RSH) mkdir -p $(ESXI_BUILD_DIR)
	$(ESXI_BUILD_RSH) $(ESXI_Q)'set -e; cd $(ESXI_BUILD_DIR); tar xzvf ~/$<; ./esxi_bootstrap.sh'$(ESXI_Q)
	$(ESXI_BUILD_RSH) $(ESXI_Q)'set -e; cd $(ESXI_BUILD_DIR)/common; make CIMDE=1 solarflare && make CIMDE=1 visor-oem'$(ESXI_Q)
	$(ESXI_BUILD_RCP) $(ESXI_COPYTO):$(ESXI_BUILD_DIR)/$(ESXI_BUILD_VIB_PATH)/$(notdir $@) $@

endif
