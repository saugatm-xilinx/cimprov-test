cimlisten_DIR = cimple/tools/cimlisten
cimlisten_SOURCES = main.cpp
cimlisten_TARGET = cimlisten
cimlisten_DEPENDS = $(PEGASUS_TOOLS_DEPS)

$(eval $(call component,cimlisten,BINARIES))

$(cimlisten_TARGET) : LIBRARIES += peglistener


