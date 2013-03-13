genmod_DIR = cimple/tools/genmod
genmod_SOURCES = main.cpp
genmod_TARGET = genmod
genmod_DEPENDS = file2c libtools libgencommon
genmod_GENERATED = usage.h

$(eval $(call component,genmod,BINARIES))

$(genmod_TARGET) : $(libgencommon_TARGET) $(libmof_TARGET) $(libtools_TARGET) $(CIMPLE_LIBS) 

$(_genmod_SOURCES) : $(genmod_DIR)/usage.h

.PHONY : clean-genmod-extra
clean-genmod : clean-genmod-extra

clean-genmod-extra:
	-rm -f $(genmod_DIR)/usage.h

