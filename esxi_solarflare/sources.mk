esxi_archive_TARGET = esxi-solarflare.tar.gz
esxi_archive_DIR = esxi_solarflare

esxi_archive_COMPONENTS = $(foreach comp,$(COMPONENTS),$(if $($(comp)_SDK),,$(comp) ))
ESXI_PROJECT_NAME = solarflare
ESXI_SRC_PATH = $(esxi_archive_DIR)/$(ESXI_PROJECT_NAME)
ESXI_GENERATED = $(foreach comp,$(esxi_archive_COMPONENTS),$(_$(comp)_SOURCES) $(_$(comp)_HEADERS) )
ESXI_GENERATED += $(libcimobjects_DIR)/repository.mof $(libcimobjects_DIR)/interop.mof
ESXI_GENERATED += repository.reg.in interop.reg.in
ESXI_GENERATED += Makefile.am

esxi_archive_GENERATED = $(addprefix $(ESXI_PROJECT_NAME)/,$(ESXI_GENERATED))
_esxi_archive_GENERATED = $(addprefix $(esxi_archive_DIR)/,$(esxi_archive_GENERATED))

esxi_archive_SOURCES = $(esxi_archive_GENERATED) \
					   esxi_bootstrap.sh \
					   $(ESXI_PROJECT_NAME)/configure.ac \
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
	echo "lib$(PROVIDER_LIBRARY)_so_CPPFLAGS=$(CPPFLAGS) -DCIMPLE_PLATFORM_$(CIMPLE_PLATFORM)" >>$@
	echo "lib$(PROVIDER_LIBRARY)_so_CPPFLAGS+= -DCIMPLE_CMPI_MODULE -I\$$(srcdir) -I\$$(srcdir)/cimple" >>$@
	echo "lib$(PROVIDER_LIBRARY)_so_CPPFLAGS+= -I\$$(srcdir)/libcimobjects" >>$@
	echo "lib$(PROVIDER_LIBRARY)_so_CPPFLAGS+= -DSF_IMPLEMENTATION_NS=\\\"\$$(smash_namespace)\\\"" >>$@
	echo "lib$(PROVIDER_LIBRARY)_so_CPPFLAGS+= -DSF_INTEROP_NS=\\\"\$$(sfcb_interop_namespace)\\\"" >>$@
	echo "lib$(PROVIDER_LIBRARY)_so_CXXFLAGS=\$$(CFLAGS) $(CXXFLAGS)" >>$@
	echo "lib$(PROVIDER_LIBRARY)_so_LDFLAGS=-shared -L\$$(srcdir)" >>$@
	echo "NAMESPACES=\$$(smash_namespace) \$$(sfcb_interop_namespace)" >>$@
	echo "if ENABLE_SFCB" >>$@
	echo "dist_sfcb_interop_ns_DATA = interop.mof" >>$@
	echo "dist_sfcb_ns_DATA = repository.mof" >>$@
	echo "dist_sfcb_reg_DATA = repository.reg interop.reg" >>$@
	echo "endif" >>$@
