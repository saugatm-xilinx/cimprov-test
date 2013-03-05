genmod_DIR = cimple/tools/genmod
genmod_SOURCES = main.cpp
genmod_TARGET = ./genmod

cimple/tools/genmod/%.o cimple/tools/genmod/%.d : CPPFLAGS += $(TOOLS_CPPFLAGS) -I$(libmof_DIR) -I$(libgencommon_DIR)

$(eval $(call component,genmod,BINARIES))

$(genmod_TARGET) : $(libgencommon_TARGET) $(libmof_TARGET) $(libtools_TARGET) $(CIMPLE_LIBS) 

$(_genmod_SOURCES) : $(genmod_DIR)/usage.h

.PHONY : clean-genmod-extra
clean-genmod : clean-genmod-extra

clean-genmod-extra:
	-rm -f $(genmod_DIR)/usage.h

