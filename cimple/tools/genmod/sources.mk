genmod_PURPOSE = host
genmod_DIR = cimple/tools/genmod
genmod_SOURCES = main.cpp
genmod_TARGET = genmod
genmod_DEPENDS = libgencommon
genmod_GENERATED = usage.h
genmod_SDK = 1

$(eval $(call component,genmod,BINARIES))



