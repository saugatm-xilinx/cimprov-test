libtools_DIR = cimple/tools/lib
TOOLS_CPPFLAGS = -I$(libtools_DIR)
libtools_SOURCES = UUID.cpp MAC.cpp crc.cpp util.cpp
libtools_TARGET = libtools.a

$(libtools_DIR)/%.o $(libtools_DIR)/%.d : CPPFLAGS += $(TOOLS_CPPFLAGS)

$(eval $(call component,libtools,STATIC_LIBRARIES))
