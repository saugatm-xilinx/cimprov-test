
$(CIM_SCHEMA_ZIP):
	$(CURL) $(CIM_SCHEMA_REPOSITORY)/$(CIM_SCHEMA_ZIP) -o $@

$(CIM_SCHEMA_DIR) : $(CIM_SCHEMA_ZIP)
	mkdir -p $(CIM_SCHEMA_DIR)/DMTF
	unzip -o $(CIM_SCHEMA_ZIP) -d $(CIM_SCHEMA_DIR)/DMTF
ifneq ($(realpath $(CIM_SCHEMA_PATCHDIR)),$(realpath $(CIM_SCHEMA_DIR)))
	cp $(CIM_SCHEMA_PATCHDIR)/*.mof $(CIM_SCHEMA_DIR)
endif


%.d: %.cpp
	@echo Producing $@
	@set -e; rm -f $@; \
	$(CXX) -MM $(CPPFLAGS) $< > $@.$$$$; \
	$(SED) 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

ifneq ($(MAKECMDGOALS),clean)
include $(patsubst %.cpp,%.d,$(ALL_SOURCES))
endif

%_Yacc.cpp : %.y
	$(BISON) --defines=$(patsubst %.cpp,%.h,$@) -p$(notdir $*)_ -o$@ $<

%_Lex.cpp : %.l
	$(FLEX) -P$(notdir $*)_ -o$@ $<

$(STATIC_LIBRARIES) : %.a:
	$(AR) crsu $@ $(filter %o,$^)

link_static_to_shared_start = -Wl,-whole-archive
link_static_to_shared_end = -Wl,-no-whole-archive

$(SHARED_LIBRARIES) : %.so:
	$(CXX) -shared -o $@ $(LDFLAGS) $(CXXFLAGS) $(filter %.o,$^) \
	-L. $(link_static_to_shared_start) $(addprefix -l,$(LIBRARIES)) $(link_static_to_shared_end)

$(BINARIES) : %:
	$(CXX) -o $@ $(LDFLAGS) $(CXXFLAGS) $(filter %.o,$^) -L. $(addprefix -l,$(LIBRARIES))

CLEAN_TARGETS = $(addprefix clean-,$(COMPONENTS))

.PHONY: clean $(CLEAN_TARGETS)

clean : $(CLEAN_TARGETS)

$(CLEAN_TARGETS) : clean-% :
	-test -n "$($*_OBJS)" && rm $($*_OBJS)
	-test -n "$($*_OBJS)" && rm $(patsubst %.o,%.d,$($*_OBJS))
	-test -z "$($*_PERSISTENT_TARGET)" && rm $($*_TARGET)
	@echo "$(filter esxi_solarflare/solarflare/libcimobjects/%,$(_$*_GENERATED))"
	-$(foreach f,$(_$*_GENERATED),rm $(f))
	-$($*_EXTRA_CLEAN)

