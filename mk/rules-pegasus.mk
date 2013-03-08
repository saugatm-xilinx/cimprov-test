PROVIDER_LIBPATH=$(PEGASUS_LIBDIR)
PROVIDER_LIBRARY_SO=$(PROVIDER_LIBPATH)/lib$(PROVIDER_LIBRARY).so

register: install $(regmod_TARGET)
	$(regmod_TARGET) -n $(IMP_NAMESPACE) -c $(PROVIDER_LIBRARY_SO)
	$(regmod_TARGET) -n $(INTEROP_NAMESPACE) -c $(PROVIDER_LIBRARY_SO) $(INTEROP_CLASSES)

unregister: $(regmod_TARGET)
	$(regmod_TARGET) -n $(INTEROP_NAMESPACE) -u -c -i $(PROVIDER_LIBRARY_SO) $(INTEROP_CLASSES)
	$(regmod_TARGET) -n $(IMP_NAMESPACE) -u -c -i $(PROVIDER_LIBRARY_SO)

PEGASUS_UPSTREAM_VERSION=2.11.1
PEGASUS_UPSTREAM_TARBALL=pegasus-$(PEGASUS_UPSTREAM_VERSION).tar.gz
PEGASUS_UPSTREAM_REPOSITORY=https://oktetlabs.ru/purgatorium/prj/level5/cim

CIM_SCHEMA_ZIP=cim_schema_2.26.0Experimental-MOFs.zip
CIM_SCHEMA_REPOSITORY=$(PEGASUS_UPSTREAM_REPOSITORY)

.PRECIOUS : $(PEGASUS_UPSTREAM_TARBALL) $(CIM_SCHEMA_ZIP)
$(PEGASUS_UPSTREAM_TARBALL):
	$(CURL) $(PEGASUS_UPSTREAM_REPOSITORY)/$(PEGASUS_UPSTREAM_TARBALL) -o $@

$(CIM_SCHEMA_ZIP):
	$(CURL) $(CIM_SCHEMA_REPOSITORY)/$(CIM_SCHEMA_ZIP) -o $@

.PHONY: pegasus-build

export PEGASUS_HOME
export PEGASUS_ROOT
export PEGASUS_PLATFORM

pegasus-build : override PEGASUS_ROOT=$(CURDIR)/pegasus
pegasus-build : override PEGASUS_HOME=$(PEGASUS_ROOT)/setup
pegasus-build : override PEGASUS_PLATFORM=$(CIMPLE_PLATFORM)
pegasus-build : export PEGASUS_DEBUG=true
pegasus-build : export PEGASUS_CIM_SCHEMA=$(CIM_SCHEMA_DIR)

pegasus-build: $(PEGASUS_UPSTREAM_TARBALL) $(CIM_SCHEMA_ZIP)
	tar xzf $<
	test -d $(PEGASUS_ROOT)
	mkdir -p $(PEGASUS_HOME)
	mkdir -p $(PEGASUS_ROOT)/Schemas/$(CIM_SCHEMA_DIR)/DMTF
	unzip -o $(CIM_SCHEMA_ZIP) -d $(PEGASUS_ROOT)/Schemas/$(CIM_SCHEMA_DIR)/DMTF
	cp schemas/$(CIM_SCHEMA_DIR)/*.mof $(PEGASUS_ROOT)/Schemas/$(CIM_SCHEMA_DIR)
	make -C $(PEGASUS_ROOT) build
	PATH=$(PEGASUS_HOME)/bin:$$PATH make -C $(PEGASUS_ROOT) repository
	$(PEGASUS_HOME)/bin/cimmofl -aE -R$(PEGASUS_HOME) -Nrepository -n$(IMP_NAMESPACE) \
					$(PEGASUS_ROOT)/Schemas/$(CIM_SCHEMA_DIR)/Core_Qualifiers.mof

.PHONY : pegasus-start
pegasus-start:
	$(PEGASUS_HOME)/bin/cimserver $(PEGASUS_RUN_OPTS)

.PHONY : patch-pegasus-schema
patch-pegasus-schema: $(CIM_SCHEMA_ZIP)
	mkdir -p $(PEGASUS_ROOT)/Schemas/$(CIM_SCHEMA_DIR)/DMTF
	unzip -o $(CIM_SCHEMA_ZIP) -d $(PEGASUS_ROOT)/Schemas/$(CIM_SCHEMA_DIR)/DMTF
	cp schemas/$(CIM_SCHEMA_DIR)/*.mof $(PEGASUS_ROOT)/Schemas/$(CIM_SCHEMA_DIR)

