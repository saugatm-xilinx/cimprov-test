ciminvoke_DIR = cimple/tools/ciminvoke
ciminvoke_SOURCES = main.cpp
ciminvoke_TARGET = ciminvoke

$(eval $(call component,ciminvoke,BINARIES))

$(ciminvoke_TARGET) : $(CIMPLE_LIBS) 

