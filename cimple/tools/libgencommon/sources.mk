libgencommon_DIR = cimple/tools/libgencommon
libgencommon_SOURCES = gencommon.cpp
libgencommon_TARGET = libgencommon.a

libgencommon_DEPENDS = libtools libmof
libgencommon_INCLUDES = $(libgencommon_DIR)
libgencommon_CPPFLAGS = $(libgencommon_PROVIDE_CPPFLAGS) -DGENCOMMON_INTERNAL
libgencommon_CPPFLAGS += -DCIMPLE_DEFAULT_SCHEMA=\"$(CIM_SCHEMA_DIR)\" 

$(eval $(call component,libgencommon,STATIC_LIBRARIES))
