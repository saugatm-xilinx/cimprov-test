esxi_archive_TARGET = esxi-solarflare.tar.gz
esxi_archive_DIR = esxi_solarflare

ESXI_PROJECT_NAME = solarflare
ESXI_SRC_PATH = $(esxi_archive_DIR)/$(ESXI_PROJECT_NAME)
ESXI_EXTRA_LIBDIR = $(libprovider_DIR)/esxi_libs/i386
ESXI_EXTRA_LIBS = libssh2.a libssl.so.0.9.8 librt.so.1 libcrypto.so.0.9.8

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

$(ESXI_SRC_PATH)/Makefile.am : $(MAKEFILE_LIST)
	echo "bin_PROGRAMS=lib$(PROVIDER_LIBRARY).so" >$@
	echo "lib$(PROVIDER_LIBRARY)_so_SOURCES=$(firstword $(ESXI_CONTENTS))" >>$@
	for src in $(wordlist 2,$(words $(ESXI_CONTENTS)),$(ESXI_CONTENTS)); do \
		echo "lib$(PROVIDER_LIBRARY)_so_SOURCES+=$${src}" >>$@; \
	done
	echo "lib$(PROVIDER_LIBRARY)_so_CPPFLAGS=$(CPPFLAGS) -I\$$(srcdir)" >>$@
	echo "lib$(PROVIDER_LIBRARY)_so_CPPFLAGS+=$(patsubst -I%,-I\$$(srcdir)/%,$(libprovider_CPPFLAGS) $(libtoolstgt_CPPFLAGS) $(_libprovider_DEP_CPPFLAGS))" >>$@
	echo "lib$(PROVIDER_LIBRARY)_so_CPPFLAGS+= -DSF_IMPLEMENTATION_NS=\\\"\$$(smash_namespace)\\\"" >>$@
	echo "lib$(PROVIDER_LIBRARY)_so_CPPFLAGS+= -DSF_INTEROP_NS=\\\"\$$(sfcb_interop_namespace)\\\"" >>$@
	echo "lib$(PROVIDER_LIBRARY)_so_CPPFLAGS+= -DCIMPLE_CMPI_MODULE" >>$@
	echo "lib$(PROVIDER_LIBRARY)_so_CXXFLAGS=\$$(CFLAGS) -Wno-unused -Wno-unused-parameter $(CXXFLAGS)" >>$@
	echo "lib$(PROVIDER_LIBRARY)_so_LDADD=\$$(srcdir)/libprovider/esxi_libs/i386/libsfupdate.a " >>$@
	echo "lib$(PROVIDER_LIBRARY)_so_LDADD+=\$$(srcdir)/libprovider/esxi_libs/i386/libutils.a " >>$@
	echo "lib$(PROVIDER_LIBRARY)_so_LDADD+=\$$(srcdir)/libprovider/esxi_libs/i386/libcurl.a " >>$@
	echo "lib$(PROVIDER_LIBRARY)_so_LDADD+=\$$(srcdir)/libprovider/esxi_libs/i386/libssh2.a " >>$@
	echo "lib$(PROVIDER_LIBRARY)_so_LDADD+=\$$(srcdir)/libprovider/esxi_libs/i386/libssl.so.0.9.8 " >>$@
	echo "lib$(PROVIDER_LIBRARY)_so_LDADD+=\$$(srcdir)/libprovider/esxi_libs/i386/librt.so.1 " >>$@
	echo "lib$(PROVIDER_LIBRARY)_so_LDADD+=\$$(srcdir)/libprovider/esxi_libs/i386/libcrypto.so.0.9.8 " >>$@
	echo "lib$(PROVIDER_LIBRARY)_so_LDFLAGS=-shared -L\$$(srcdir)" >>$@
	echo "NAMESPACES=\$$(smash_namespace) \$$(sfcb_interop_namespace) root/cimv2" >>$@
	echo "if ENABLE_SFCB" >>$@
	echo "dist_sfcb_interop_ns_DATA = libcimobjects/interop.mof" >>$@
	echo "dist_sfcb_ns_DATA = libcimobjects/namespace.mof" >>$@
	echo "dist_sfcb_root_ns_DATA = libcimobjects/root.mof" >>$@
	echo "dist_sfcb_reg_DATA = repository.reg" >>$@
	echo "endif" >>$@

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
