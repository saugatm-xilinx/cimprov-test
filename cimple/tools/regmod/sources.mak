regmod_SOURCES = cimple/tools/regmod/main.cpp
regmod_SOURCES += $(tools_SOURCES)

cimple/tools/regmod/%.o : CPPFLAGS += $(TOOLS_CPPFLAGS)

