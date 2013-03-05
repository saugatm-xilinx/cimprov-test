libgencommon_DIR = cimple/tools/libgencommon
libgencommon_SOURCES = gencommon.cpp
libgencommon_TARGET = libgencommon.a

$(libgencommon_DIR)/%.o $(libgencommon_DIR)/%.d : CPPFLAGS += -DGENCOMMON_INTERNAL -I$(libgencommon_DIR) -I$(libmof_DIR) \
		-DCIMPLE_DEFAULT_SCHEMA=\"$(PEGASUS_ROOT)/Schemas/$(CIM_SCHEMA_DIR)\" $(TOOLS_CPPFLAGS)

$(eval $(call component,libgencommon,STATIC_LIBRARIES))
