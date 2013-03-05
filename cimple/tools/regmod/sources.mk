regmod_DIR = cimple/tools/regmod
regmod_SOURCES = main.cpp
regmod_TARGET = ./regmod

cimple/tools/regmod/%.o : CPPFLAGS += $(TOOLS_CPPFLAGS)

$(eval $(call component,regmod,BINARIES))

$(regmod_TARGET) : $(libtools_TARGET) $(CIMPLE_LIBS) 

