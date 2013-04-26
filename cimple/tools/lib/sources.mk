libtools_PURPOSE = host
libtools_DIR = cimple/tools/lib
libtools_SOURCES = UUID.cpp MAC.cpp crc.cpp util.cpp
libtools_TARGET = libtools.a
libtools_DEPENDS  = libcimplehost

libtools_INCLUDES = $(libtools_DIR)

libtools_SDK = 1

$(eval $(call component,libtools,STATIC_LIBRARIES))

libtoolstgt_PURPOSE = target
libtoolstgt_DIR = cimple/tools/libtgt
libtoolstgt_SOURCES = $(libtools_SOURCES)
libtoolstgt_GENERATED = $(libtoolstgt_SOURCES)
libtoolstgt_TARGET = libtoolstgt.a
libtoolstgt_DEPENDS = libcimple

libtoolstgt_INCLUDES = $(libtools_INCLUDES)

$(eval $(call component,libtoolstgt,STATIC_LIBRARIES))

$(libtoolstgt_DIR)/%.cpp : $(libtools_DIR)/%.cpp
	mkdir -p $(dir $@)
	cp $< $@

