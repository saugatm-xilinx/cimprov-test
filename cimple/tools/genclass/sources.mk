genclass_DIR = cimple/tools/genclass
genclass_SOURCES = main.cpp
genclass_TARGET = genclass
genclass_DEPENDS = libgencommon
genclass_GENERATED = usage.h
genclass_SDK = 1

$(eval $(call component,genclass,BINARIES))




