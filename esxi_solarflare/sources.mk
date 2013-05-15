esxi_archive_TARGET = esxi-solarflare.tar.gz
esxi_archive_DIR = esxi_solarflare

esxi_archive_COMPONENTS = $(foreach comp,$(COMPONENTS),$(if $(findstring target,$($(comp)_PURPOSE)),$(comp) ,))
ESXI_PROJECT_NAME = solarflare
ESXI_SRC_PATH = $(esxi_archive_DIR)/$(ESXI_PROJECT_NAME)
ESXI_GENERATED = $(foreach comp,$(esxi_archive_COMPONENTS),$(_$(comp)_SOURCES) $(_$(comp)_HEADERS) )
ESXI_GENERATED += $(libcimobjects_DIR)/repository.mof $(libcimobjects_DIR)/interop.mof
ESXI_GENERATED += repository.reg.in interop.reg.in
ESXI_GENERATED += Makefile.am
ESXI_GENERATED += libprovider/esxi_libs/i386/libsfupdate.a libprovider/esxi_libs/i386/libutils.a
ESXI_GENERATED += libprovider/esxi_libs/i386/libcurl.a
ESXI_GENERATED += libprovider/esxi_libs/i386/libssh2.a
ESXI_GENERATED += libprovider/esxi_libs/i386/libssl.so.0.9.8
ESXI_GENERATED += libprovider/esxi_libs/i386/librt.so.1
ESXI_GENERATED += libprovider/esxi_libs/i386/libcrypto.so.0.9.8

esxi_archive_GENERATED = $(addprefix $(ESXI_PROJECT_NAME)/,$(ESXI_GENERATED))
_esxi_archive_GENERATED = $(addprefix $(esxi_archive_DIR)/,$(esxi_archive_GENERATED))

ifeq ($(MAKECMDGOALS),clean)
_esxi_archive_GENERATED += $(foreach comp,$(esxi_archive_COMPONENTS), \
								$(wildcard $(patsubst %,$(esxi_archive_DIR)/$(ESXI_PROJECT_NAME)/%/*.h,$($(comp)_INCLUDES))))
endif


esxi_archive_SOURCES = $(esxi_archive_GENERATED) \
					   esxi_bootstrap.sh \
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
					   oem/solarflare.make

_esxi_archive_SOURCES = $(addprefix $(esxi_archive_DIR)/,$(esxi_archive_SOURCES))

$(esxi_archive_TARGET) : $(_esxi_archive_SOURCES)
	cd $(esxi_archive_DIR); tar -czf ../$@ *

$(ESXI_SRC_PATH)/$(libcimobjects_DIR)/repository.mof : $(libcimobjects_DIR)/repository.mof \
													   $(addprefix $(CIM_SCHEMA_PATCHDIR)/,$(CIM_SCHEMA_ADDON_MOFS))
	mkdir -p $(dir $@)
	cat $^ >$@

$(ESXI_SRC_PATH)/repository.reg.in : repository.reg
	mkdir -p $(dir $@)	
	$(SED) 's!$(IMP_NAMESPACE)!@smash_namespace@!g' <$< >$@

$(ESXI_SRC_PATH)/interop.reg.in : interop.reg
	mkdir -p $(dir $@)	
	$(SED) 's!$(INTEROP_NAMESPACE)!@sfcb_interop_namespace@!g' <$< >$@

$(ESXI_SRC_PATH)/% : %
	mkdir -p $(dir $@)
	cp $< $@

$(ESXI_SRC_PATH)/Makefile.am : $(MAKEFILE_LIST)
	echo "bin_PROGRAMS=lib$(PROVIDER_LIBRARY).so" >$@
	echo "lib$(PROVIDER_LIBRARY)_so_SOURCES=$(firstword $(ESXI_GENERATED))" >>$@
	for src in $(wordlist 2,$(words $(ESXI_GENERATED)),$(ESXI_GENERATED)); do \
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
	echo "NAMESPACES=\$$(smash_namespace) \$$(sfcb_interop_namespace)" >>$@
	echo "if ENABLE_SFCB" >>$@
	echo "dist_sfcb_interop_ns_DATA = libcimobjects/interop.mof" >>$@
	echo "dist_sfcb_ns_DATA = libcimobjects/repository.mof" >>$@
	echo "dist_sfcb_reg_DATA = repository.reg interop.reg" >>$@
	echo "endif" >>$@

COMPONENTS += esxi_archive

ifneq ($(ESXI_BUILD_HOST),)

ifneq ($(ESXI_GATEWAY),)
ESXI_CONNECT = ssh $(ESXI_GATEWAY)
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
	$(ESXI_BUILD_RSH) "'set -e; cd $(ESXI_BUILD_DIR); tar xzvf ~/$<; ./esxi_bootstrap.sh'"
	$(ESXI_BUILD_RSH) "'set -e; cd $(ESXI_BUILD_DIR)/common; make CIMDE=1 solarflare && make CIMDE=1 visor-oem'"
	$(ESXI_BUILD_RCP) $(ESXI_COPYTO):$(ESXI_BUILD_DIR)/$(ESXI_BUILD_VIB_PATH)/$(notdir $@) $@

endif
