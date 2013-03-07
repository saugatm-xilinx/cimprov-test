include mk/rules-reg.mk

ALL_HEADERS = $(wildcard *.h) $(wildcard cimple/*.h) $(wildcard cimple/cmpi/*.h)
ALL_HEADERS += $(wildcard ci/*.h) $(wildcard ci/tools/*.h) $(wildcard ci/tools/platform/*.h)
ALL_HEADERS += $(wildcard ci/compat/*.h) $(wildcard ci/mgmt/*.h)
ESXI_SUBDIR = esxi_solarflare
ESXI_PROJECT_NAME = solarflare
ESXI_SRC_PATH = $(ESXI_SUBDIR)/$(ESXI_PROJECT_NAME)
ESXI_GENERATED = $(libprovider_SOURCES) $(ALL_HEADERS)
ESXI_GENERATED += repository.mof interop.mof
ESXI_GENERATED += repository.reg.in interop.reg.in
ESXI_GENERATED += Makefile.am

esxi-solarflare.tar.gz : $(addprefix $(ESXI_SRC_PATH)/,$(ESXI_GENERATED)) $(ESXI_SUBDIR)/esxi_bootstrap.sh
	cd $(ESXI_SUBDIR); tar -czf ../$@ *

$(ESXI_SRC_PATH)/repository.reg.in : repository.reg
	mkdir -p $(dir $@)	
	$(SED) 's!$(IMP_NAMESPACE)!@smash_namespace@!g' <$< >$@

$(ESXI_SRC_PATH)/interop.reg.in : interop.reg
	mkdir -p $(dir $@)	
	$(SED) 's!$(INTEROP_NAMESPACE)!@sfcb_interop_namespace@!g' <$< >$@


$(ESXI_SRC_PATH)/% : %
	mkdir -p $(dir $@)
	cp $< $@

$(ESXI_SRC_PATH)/Makefile.am : mk/rules-esxi.mk
	echo "bin_PROGRAMS=lib$(PROVIDER_LIBRARY).so" >$@
	echo "lib$(PROVIDER_LIBRARY)_so_SOURCES=$(firstword $(libprovider_SOURCES))" >>$@
	for src in $(wordlist 2,$(words $(libprovider_SOURCES)),$(libprovider_SOURCES)); do \
		echo "lib$(PROVIDER_LIBRARY)_so_SOURCES+=$${src}" >>$@; \
	done
	echo "lib$(PROVIDER_LIBRARY)_so_CPPFLAGS=$(CPPFLAGS) -DCIMPLE_CMPI_MODULE -I\$$(srcdir) -I\$$(srcdir)/cimple" >>$@
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
