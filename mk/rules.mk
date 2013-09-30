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

.PHONY : distname

distname :
	@echo $(PROVIDER_TARBALL_DIR)

.PHONY : dist

dist : $(PROVIDER_TARBALL)

ifeq ($(MAKECMDGOALS), dist)
ifeq ($(DIST_IS_SPECIAL),)
$(PROVIDER_TARBALL) : $(PROVIDER_DIST_FILES)
endif
endif

define M4_DEFINE
	@echo 'm4_define([__$(1)__], [[$(subst ],,$(subst [,,$(subst #,,$(subst ','\'',$(strip $($(1)))))))]])m4_dnl' >>$@

endef

m4.defs : $(MAKEFILE_LIST)
	@echo Producing M4 defs from make var
	@echo "m4_changequote([,])m4_dnl" >$@
	$(foreach var,$(filter-out M4_DEFINE,$(.VARIABLES)), $(call M4_DEFINE,$(var)))

lib$(PROVIDER_LIBRARY).spec : m4.defs lib$(PROVIDER_LIBRARY).spec.in
	$(M4) $^ >$@

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

TAR_TRANSFORM = !^!$*/!
%.tar.gz :
	tar $(patsubst %,--transform='s%',$(TAR_TRANSFORM)) -czf $@ $(filter-out $(TOP)/%,$^) -C $(TOP) $(patsubst $(TOP)/%,%,$(filter $(TOP)/%,$^))


