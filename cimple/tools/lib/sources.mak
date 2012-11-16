TOOLSLIB_SRC_PATH = cimple/tools/lib
TOOLS_CPPFLAGS = -I$(TOOLSLIB_SRC_PATH)
_tools_SOURCES = UUID.cpp MAC.cpp crc.cpp util.cpp
tools_SOURCES = $(addprefix $(TOOLSLIB_SRC_PATH)/,$(_tools_SOURCES))

$(TOOLSLIB_SRC_PATH)/%.o : CPPFLAGS += $(TOOLS_CPPFLAGS)

