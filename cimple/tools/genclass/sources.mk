genclass_DIR = cimple/tools/genclass
genclass_SOURCES = main.cpp
genclass_TARGET = genclass
genclass_DEPENDS = libgencommon
genclass_GENERATED = usage.h

$(eval $(call component,genclass,BINARIES))

$(genclass_TARGET) : $(libgencommon_TARGET) $(libmof_TARGET) $(libtools_TARGET) $(CIMPLE_LIBS) 

$(_genclass_SOURCES) : $(_genclass_GENERATED)



