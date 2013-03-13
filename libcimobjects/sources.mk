libcimobjects_TARGET = libcimobjects.a
libcimobjects_DIR = libcimobjects
libcimobjects_INCLUDES = $(libcimobjects_DIR)
libcimobjects_DEPENDS = $(CIMPLE_COMPONENTS)

include $(libcimobjects_DIR)/classes.mk

libcimobjects_GENERATED = $(libcimobjects_SOURCES) $(patsubst %.cpp,%.h,$(libcimobjects_SOURCES)) classes.mk

$(eval $(call component,libcimobjects,STATIC_LIBRARIES))

$(libcimobjects_DIR)/.genclass : $(libcimobjects_DIR)/classes $(libcimobjects_DIR)/repository.mof $(CIM_SCHEMA_DIR) $(genclass_TARGET)
	cd $(libcimobjects_DIR); CIMPLE_MOF_PATH="$(CIM_SCHEMA_DIR)" $(abspath $(genclass_TARGET)) -S -r -e -F$(notdir $<)

$(filter %.h,$(_libcimobjects_GENERATED)) : $(libcimobjects_DIR)/classes.mk

$(libcimobjects_DIR)/classes.mk : $(libcimobjects_DIR)/.genclass
	$(AWK) '{ print "libcimobjects_SOURCES += ", $$0 }' $< >$@