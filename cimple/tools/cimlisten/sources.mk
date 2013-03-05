cimlisten_DIR = cimple/tools/cimlisten
cimlisten_SOURCES = main.cpp
cimlisten_TARGET = cimlisten

$(eval $(call component,cimlisten,BINARIES))

$(cimlisten_TARGET) : LIBRARIES += peglistener

$(cimlisten_TARGET) : $(CIMPLE_LIBS) 

