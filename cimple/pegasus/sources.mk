libcimplepeg_PURPOSE = target
libcimplepeg_SOURCES = \
    Pegasus_Adapter.cpp \
    Pegasus_Thread_Context.cpp \
    pegasus_unsupported.cpp \
	Containers.cpp \
	Converter.cpp \
	print.cpp

libcimplepeg_DIR = cimple/pegasus

libcimplepeg_INCLUDES = $(libcimplepeg_DIR)

libcimplepeg_TARGET = libcimplepeg.a

libcimplepeg_DEPENDS = pegasus libcimple

$(eval $(call component,libcimplepeg,STATIC_LIBRARIES))

ifeq ($(CIM_INTERFACE),pegasus)
CIMPLE_COMPONENT := libcimplepeg 
endif

