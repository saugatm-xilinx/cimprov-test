genclass_DIR = cimple/tools/genclass
genclass_SOURCES = main.cpp
genclass_TARGET = ./genclass

cimple/tools/genclass/%.o cimple/tools/genclass/%.d : CPPFLAGS += $(TOOLS_CPPFLAGS) -I$(libmof_DIR) -I$(libgencommon_DIR)

$(eval $(call component,genclass,BINARIES))

$(genclass_TARGET) : $(libgencommon_TARGET) $(libmof_TARGET) $(libtools_TARGET) $(CIMPLE_LIBS) 

$(_genclass_SOURCES) : $(genclass_DIR)/usage.h

.PHONY : clean-genclass-extra
clean-genclass : clean-genclass-extra

clean-genclass-extra:
	-rm -f $(genclass_DIR)/usage.h

