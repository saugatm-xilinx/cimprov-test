ciminvoke_PURPOSE = target
ciminvoke_DIR = cimple/tools/ciminvoke
ciminvoke_SOURCES = main.cpp
ciminvoke_TARGET = ciminvoke
ciminvoke_DEPENDS = $(PEGASUS_TOOLS_DEPS)

$(eval $(call component,ciminvoke,BINARIES))


