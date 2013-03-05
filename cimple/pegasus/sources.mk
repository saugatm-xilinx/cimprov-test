libcimplepeg_SOURCES = \
    Pegasus_Adapter.cpp \
    Pegasus_Thread_Context.cpp \
    pegasus_unsupported.cpp \
	Containers.cpp \
	Converter.cpp \
	print.cpp

libcimplepeg_DIR = cimple/pegasus

libcimplepeg_TARGET = libcimplepeg.a

$(eval $(call component,libcimplepeg,STATIC_LIBRARIES))


CIMPLE_LIBS := $(libcimplepeg_TARGET) $(CIMPLE_LIBS)

