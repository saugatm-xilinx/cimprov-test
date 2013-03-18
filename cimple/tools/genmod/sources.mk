genmod_DIR = cimple/tools/genmod
genmod_SOURCES = main.cpp
genmod_TARGET = genmod
genmod_DEPENDS = libgencommon
genmod_GENERATED = usage.h
genmod_SDK = 1

$(eval $(call component,genmod,BINARIES))

$(info genmod_CPPFLAGS = $(top_CPPFLAGS) $(genmod_CPPFLAGS) $(_genmod_DEP_CPPFLAGS))


