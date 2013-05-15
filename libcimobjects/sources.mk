libcimobjects_PURPOSE = target
libcimobjects_TARGET = libcimobjects.a
libcimobjects_DIR = libcimobjects
libcimobjects_INCLUDES = $(libcimobjects_DIR)
libcimobjects_DEPENDS = $(CIMPLE_COMPONENT)

libcimobjects_EXTRA_MOF = CIM_OperatingSystem 
ifeq ($(CIM_SERVER),pegasus)
libcimobjects_EXTRA_MOF += IBMPSG_ComputerSystem IBMSD_ComputerSystem IBMSD_SPComputerSystem PG_ComputerSystem 
endif
ifeq ($(CIM_SERVER),esxi)
libcimobjects_EXTRA_MOF += OMC_UnitaryComputerSystem
endif

include $(libcimobjects_DIR)/classes.mk

libcimobjects_GENERATED = $(libcimobjects_SOURCES) $(patsubst %.cpp,%.h,$(libcimobjects_SOURCES)) classes.mk repository.mof 

$(eval $(call component,libcimobjects,STATIC_LIBRARIES))

genclass_cmd = cd $(libcimobjects_DIR); CIMPLE_MOF_PATH="$(CIM_SCHEMA_DIR)" $(abspath $(genclass_TARGET))

$(libcimobjects_DIR)/.genclass : $(libcimobjects_DIR)/classes $(libcimobjects_DIR)/repository.mof \
								 $(CIM_SCHEMA_ROOTFILE) $(genclass_TARGET)
	$(genclass_cmd) -S -r -e -F$(notdir $<) $(libcimobjects_EXTRA_MOF)

$(filter %.h,$(_libcimobjects_GENERATED)) : $(libcimobjects_DIR)/classes.mk

$(libcimobjects_DIR)/classes.mk : $(libcimobjects_DIR)/.genclass
	$(AWK) '{ print "libcimobjects_SOURCES += ", $$0 }' $< >$@

$(libcimobjects_DIR)/repository.mof : $(libcimobjects_DIR)/repository.mof.cpp
	$(target_CXX) -E -P -DTARGET_CIM_SERVER_$(CIM_SERVER) $(libcimobjects_CPPFLAGS) $(target_CPPFLAGS) $< >$@

