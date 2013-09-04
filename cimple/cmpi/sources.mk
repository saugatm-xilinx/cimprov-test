libcimplecmpi_PURPOSE = target
libcimplecmpi_SOURCES = CMPI_Containers.cpp \
    CMPI_Thread_Context.cpp \
    Converter.cpp \
    CMPI_Adapter.cpp

libcimplecmpi_TARGET = libcimplecmpi.a
libcimplecmpi_DIR = cimple/cmpi
libcimplecmpi_INCLUDES = cmpi cimple/cmpi
libcimplecmpi_DEPENDS = libcimple

$(eval $(call component,libcimplecmpi,STATIC_LIBRARIES))

CIMPLE_COMPONENT += libcimplecmpi 


