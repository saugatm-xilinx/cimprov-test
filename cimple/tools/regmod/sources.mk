regmod_DIR = cimple/tools/regmod
regmod_SOURCES = main.cpp
regmod_TARGET = regmod
regmod_DEPENDS = $(PEGASUS_TOOLS_DEPS)

$(eval $(call component,regmod,BINARIES))

ifeq ($(INTREE_PEGASUS),1)
$(regmod_TARGET) : $(PEGASUS_IMP_REPOSITORY)
endif


