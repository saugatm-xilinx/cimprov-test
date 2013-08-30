genprov_PURPOSE = host
genprov_DIR = cimple/tools/genprov
genprov_INCLUDES = $(genprov_DIR)
genprov_SOURCES = main.cpp
genprov_TARGET = genprov
genprov_SDK = 1

genprov_GENERATED = header_suffix.h \
	header_prefix.h \
	source_suffix.h \
	source_prefix.h \
	indication_header_prefix.h \
	indication_header_suffix.h \
	indication_source_prefix.h \
	association_header.h \
	association_source.h \
	usage.h

genprov_DEPENDS = libgencommon

$(genprov_DIR)/%.h : $(genprov_DIR)/%.h.in $(file2c_TARGET)
	$(abspath $(file2c_TARGET)) $< -n $* >$@

$(eval $(call component,genprov,BINARIES))

