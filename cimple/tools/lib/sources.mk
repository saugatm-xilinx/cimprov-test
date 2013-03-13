libtools_DIR = cimple/tools/lib
libtools_SOURCES = UUID.cpp MAC.cpp crc.cpp util.cpp
libtools_TARGET = libtools.a
libtools_DEPENDS  = libcimple

libtools_INCLUDES = $(libtools_DIR)

$(eval $(call component,libtools,STATIC_LIBRARIES))
