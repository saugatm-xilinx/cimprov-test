regview_DIR = cimple/tools/regview
regview_SOURCES = main.cpp
regview_TARGET = regview

cimple/tools/regview/%.o : CPPFLAGS += $(TOOLS_CPPFLAGS)

$(eval $(call component,regview,BINARIES))

$(regview_TARGET) : $(libtools_TARGET) $(CIMPLE_LIBS) 

