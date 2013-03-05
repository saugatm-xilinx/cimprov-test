file2c_DIR = cimple/tools/file2c
file2c_SOURCES = main.cpp
file2c_TARGET = file2c

cimple/tools/file2c/%.o : CPPFLAGS += $(TOOLS_CPPFLAGS)

$(eval $(call component,file2c,BINARIES))

$(file2c_TARGET) : $(libtools_TARGET) $(CIMPLE_LIBS) 

