regview_PURPOSE = target
regview_DIR = cimple/tools/regview
regview_SOURCES = main.cpp
regview_TARGET = regview
regview_DEPENDS = $(PEGASUS_TOOLS_DEPS)

$(eval $(call component,regview,BINARIES))


