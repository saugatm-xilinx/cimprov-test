genprov_DIR = cimple/tools/genprov
genprov_SOURCES = main.cpp
genprov_TARGET = ./genprov

genprov_HEADERS = header_suffix.h \
	header_prefix.h \
	source_suffix.h \
	source_prefix.h \
	indication_header_prefix.h \
	indication_header_suffix.h \
	indication_source_prefix.h \
	association_header.h \
	association_source.h

cimple/tools/genprov/%.o cimple/tools/genprov/%.d : CPPFLAGS += $(TOOLS_CPPFLAGS) -I$(libmof_DIR) -I$(libgencommon_DIR)

$(genprov_DIR)/%.h : $(genprov_DIR)/%.h.in $(file2c_TARGET)
	$(file2c_TARGET) $< -n $* >$@

$(eval $(call component,genprov,BINARIES))

$(genprov_TARGET) : $(libgencommon_TARGET) $(libmof_TARGET) $(libtools_TARGET) $(CIMPLE_LIBS)

$(_genprov_SOURCES) : $(genprov_DIR)/usage.h $(addprefix $(genprov_DIR)/,$(genprov_HEADERS))

.PHONY : clean-genprov-extra
clean-genprov : clean-genprov-extra

clean-genprov-extra:
	-rm -f $(genprov_DIR)/usage.h

