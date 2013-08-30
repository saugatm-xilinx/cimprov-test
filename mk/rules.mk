ifeq ($(USE_EXISITING_SCHEMA),)
$(CIM_SCHEMA_ZIP):
	$(CURL) $(CIM_SCHEMA_REPOSITORY)/$(CIM_SCHEMA_ZIP) -o $@

$(CIM_SCHEMA_ROOTFILE) : $(CIM_SCHEMA_ZIP)
	mkdir -p $(CIM_SCHEMA_DIR)/DMTF
	unzip -q -o $(CIM_SCHEMA_ZIP) -d $(CIM_SCHEMA_DIR)/DMTF
ifneq ($(realpath $(CIM_SCHEMA_PATCHDIR)),$(realpath $(CIM_SCHEMA_DIR)))
	cp $(CIM_SCHEMA_PATCHDIR)/*.mof $(CIM_SCHEMA_DIR)
endif
	touch $@

endif

ifeq ($(MAKECMDGOALS),dist)
PROVIDER_PACKAGE = solarflare_provider_ibm
PROVIDER_TARBALL_DIR = $(PROVIDER_PACKAGE)-$(PROVIDER_VERSION).$(PROVIDER_REVISION)
PROVIDER_DIST_FILES := $(shell $(HG) manifest)
PROVIDER_DIST_FILES += $(CIM_SCHEMA_ZIP)
PROVIDER_DIST_FILES += lib$(PROVIDER_LIBRARY).spec

.PHONY : dist

dist : $(PROVIDER_TARBALL_DIR).tar.gz

$(PROVIDER_TARBALL_DIR).tar.gz : $(addprefix $(PROVIDER_TARBALL_DIR)/,$(PROVIDER_DIST_FILES))
	tar czf $@ $^

$(PROVIDER_TARBALL_DIR)/% : %
	mkdir -p $(dir $@)
	cp $< $@
endif

define subst_spec
$(SED) 's!%{PROVIDER_LIBRARY}!$(PROVIDER_LIBRARY)!g; \
		s!%{PROVIDER_VERSION}!$(PROVIDER_VERSION)!g; \
		s!%{PROVIDER_REVISION}!$(PROVIDER_REVISION)!g; \
		s!%{PROVIDER_LIBPATH}!$(PROVIDER_LIBPATH)!g; \
		s!%{PROVIDER_PACKAGE}!$(PROVIDER_PACKAGE)!g; \
		s!%{PROVIDER_SO}!$(libprovider_TARGET)!g; \
		s!%{PROVIDER_ROOT}!$(PROVIDER_ROOT)!g; \
		s!%{IMP_NAMESPACE}!$(IMP_NAMESPACE)!g; \
		s!%{INTEROP_NAMESPACE}!$(INTEROP_NAMESPACE)!g; \
		s!%{INTEROP_CLASSES}!$(INTEROP_CLASSES)!g; \
		s!%{PRESET}!$(PRESET)!g; \
		s!%{PEGASUS_HOME}!$(PEGASUS_HOME)!g' $< >$@
endef

lib$(PROVIDER_LIBRARY).spec : lib$(PROVIDER_LIBRARY).spec.in $(MAKEFILE_LIST)
	$(subst_spec)

%.d: %.cpp
	@echo Producing $@
	@set -e; rm -f $@; \
	$(CXX) -MM $(CPPFLAGS) $< > $@.$$$$; \
	$(SED) 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

%.o: %.rc
	$(WINDRES) -o $@ -i $< $(WINDRES_CPPFLAGS)

ifneq ($(_DO_NOT_GENERATE),1)
include $(patsubst %.cpp,%.d,$(ALL_SOURCES))
endif

%_Yacc.cpp : %.y
	$(BISON) --defines=$(patsubst %.cpp,%.h,$@) -p$(notdir $*)_ -o$@ $<

%_Lex.cpp : %.l
	$(FLEX) -P$(notdir $*)_ -o$@ $<

$(STATIC_LIBRARIES) : %.a:
	$(AR) crsu $@ $(filter %.o,$^)

link_static_to_shared_start = -Wl,-whole-archive
link_static_to_shared_end = -Wl,-no-whole-archive

SOEXT ?= so

$(SHARED_LIBRARIES) : %.$(SOEXT):
	$(CXX) -shared -o $@ $(LDFLAGS) $(CXXFLAGS) $(filter %.o,$^) \
	-L. $(link_static_to_shared_start) $(addprefix -l,$(LIBRARIES)) $(link_static_to_shared_end) \
	$(addprefix -l,$(SYSLIBRARIES))


$(BINARIES) : %:
	$(CXX) -o $@ $(LDFLAGS) $(CXXFLAGS) $(filter %.o,$^) -L. $(addprefix -l,$(LIBRARIES)) $(addprefix -l,$(SYSLIBRARIES))

CLEAN_TARGETS = $(addprefix clean-,$(COMPONENTS))

.PHONY: clean $(CLEAN_TARGETS)

clean : $(CLEAN_TARGETS)

$(CLEAN_TARGETS) $(EXTRA_CLEAN_TARGETS) : clean-% :
	-test -n "$($*_OBJS)" && rm $($*_OBJS)
	-test -n "$($*_OBJS)" && rm $(patsubst %.o,%.d,$($*_OBJS))
	-test -z "$($*_PERSISTENT_TARGET)" && rm $($*_TARGET)
	-$(foreach f,$(_$*_GENERATED),rm $(f);)
	-$($*_EXTRA_CLEAN)

