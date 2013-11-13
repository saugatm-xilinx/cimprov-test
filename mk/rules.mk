##########################################################################
##! \file ./mk/rules.mk
## <L5_PRIVATE L5_SOURCE>
## \author  OktetLabs
##  \brief  CIM Provider
##   \date  2013/10/02
##    \cop  (c) Solarflare Communications Inc.
## </L5_PRIVATE>
##
##########################################################################

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

##! Output the base name of provider package name (w/o .tar.gz etc)
distname :
	@echo $(PROVIDER_TARBALL_DIR)

.PHONY : dist

##! Make a source distribution tarball
dist : $(PROVIDER_TARBALL)

ifeq ($(MAKECMDGOALS), dist)
ifeq ($(DIST_IS_SPECIAL),)
$(PROVIDER_TARBALL) : $(PROVIDER_DIST_FILES)
endif
endif

define M4_DEFINE
	@echo 'm4_define([__$(1)__], [[$(subst ],,$(subst [,,$(subst #,,$(subst ','\'',$(strip $($(1)))))))]])m4_dnl' >>$@

endef

##! Collect all Makefile variables and dump them as M4 macro definitions
m4.defs : $(MAKEFILE_LIST)
	@echo Producing M4 defs from make var
	@echo "m4_changequote([,])m4_dnl" >$@
	$(foreach var,$(filter-out M4_DEFINE,$(.VARIABLES)), $(call M4_DEFINE,$(var)))

##! Make an RPM spec file from a template and Makefile variables
## The template is processed with m4 using Makefile-derived M4 macros
lib$(PROVIDER_LIBRARY).spec : m4.defs lib$(PROVIDER_LIBRARY).spec.in
	$(M4) $^ >$@

##! Automatically generate include dependencies from C++ files
%.d : %.cpp
	@echo Producing $@
	@set -e; rm -f $@; \
	$(CXX) -MM $(CPPFLAGS) $< > $@.$$$$; \
	$(SED) 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

##! Compile Windows resource descriptions from sources
%.o : %.rc
	$(WINDRES) -o $@ -i $< $(WINDRES_CPPFLAGS)

ifneq ($(_DO_NOT_GENERATE),1)
include $(patsubst %.cpp,%.d,$(ALL_SOURCES))
endif

##! Generate Yacc/Bison parsers
%_Yacc.cpp : %.y
	$(BISON) --defines=$(patsubst %.cpp,%.h,$@) -p$(notdir $*)_ -o$@ $<

##! Generate (F)lex parsers
%_Lex.cpp : %.l
	$(FLEX) -P$(notdir $*)_ -o$@ $<

##! Assemble static libraries
$(STATIC_LIBRARIES) : %.a :
	$(AR) crsu $@ $(filter %.o,$^)

link_static_to_shared_start = -Wl,-whole-archive
link_static_to_shared_end = -Wl,-no-whole-archive

SOEXT ?= so

##! Link dynamic libraries
$(SHARED_LIBRARIES) : %.$(SOEXT) :
	$(CXX) -shared -o $@ $(LDFLAGS) $(CXXFLAGS) $(filter %.o,$^) \
	-L. $(link_static_to_shared_start) $(addprefix -l,$(LIBRARIES)) $(link_static_to_shared_end) \
	$(addprefix -l,$(SYSLIBRARIES))

##! Link executables
$(BINARIES) : %:
	$(CXX) -o $@ $(LDFLAGS) $(CXXFLAGS) $(filter %.o,$^) -L. $(addprefix -l,$(LIBRARIES)) $(addprefix -l,$(SYSLIBRARIES))

CLEAN_TARGETS = $(addprefix clean-,$(COMPONENTS))

.PHONY: clean $(CLEAN_TARGETS)

clean : $(CLEAN_TARGETS)

##! Clean components
$(CLEAN_TARGETS) $(EXTRA_CLEAN_TARGETS) : clean-% :
	-test -n "$($*_OBJS)" && rm $($*_OBJS)
	-test -n "$($*_OBJS)" && rm $(patsubst %.o,%.d,$($*_OBJS))
	-test -z "$($*_PERSISTENT_TARGET)" && rm $($*_TARGET)
	-$(foreach f,$(_$*_GENERATED),rm $(f);)
	-$($*_EXTRA_CLEAN)

TAR_TRANSFORM = !^!$*/!

##! Produces a gzipped tar archive of dependencies
%.tar.gz :
	tar $(patsubst %,--transform='s%',$(TAR_TRANSFORM)) -czf $@ $(filter-out $(TOP)/%,$^) -C $(TOP) $(patsubst $(TOP)/%,%,$(filter $(TOP)/%,$^))


