libcimobjects_TARGET = libcimobjects.a
libcimobjects_DIR = libcimobjects
libcimobjects_INCLUDES = $(libcimobjects_DIR)
libcimobjects_DEPENDS = $(CIMPLE_COMPONENTS)

libcimobjects_EXTRA_MOF = CIM_OperatingSystem IBMPSG_ComputerSystem IBMSD_ComputerSystem IBMSD_SPComputerSystem \
						 PG_ComputerSystem OMC_UnitaryComputerSystem

include $(libcimobjects_DIR)/classes.mk

libcimobjects_GENERATED = $(libcimobjects_SOURCES) $(patsubst %.cpp,%.h,$(libcimobjects_SOURCES)) classes.mk

$(eval $(call component,libcimobjects,STATIC_LIBRARIES))

genclass_cmd = cd $(libcimobjects_DIR); CIMPLE_MOF_PATH="$(CIM_SCHEMA_DIR)" $(abspath $(genclass_TARGET))

$(libcimobjects_DIR)/.genclass : $(libcimobjects_DIR)/classes $(libcimobjects_DIR)/repository.mof $(CIM_SCHEMA_DIR) $(genclass_TARGET)
	$(genclass_cmd) -S -r -e -F$(notdir $<) $(libcimobjects_EXTRA_MOF)

$(filter %.h,$(_libcimobjects_GENERATED)) : $(libcimobjects_DIR)/classes.mk

$(libcimobjects_DIR)/classes.mk : $(libcimobjects_DIR)/.genclass
	$(AWK) '{ print "libcimobjects_SOURCES += ", $$0 }' $< >$@