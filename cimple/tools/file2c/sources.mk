file2c_PURPOSE = host
file2c_DIR = cimple/tools/file2c
file2c_SOURCES = main.cpp
file2c_TARGET = file2c
file2c_DEPENDS = libtools
file2c_SDK = 1

$(eval $(call component,file2c,BINARIES))

%.h : %.txt $(file2c_TARGET)
	$(abspath $(file2c_TARGET)) $< -n `echo $(notdir $*) | tr a-z A-Z` >$@

