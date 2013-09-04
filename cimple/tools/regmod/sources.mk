regmod_PURPOSE = target
regmod_DIR = cimple/tools/regmod
regmod_INCLUDES = $(regmod_DIR)
regmod_SOURCES = main.cpp
regmod_TARGET = regmod
regmod_DEPENDS = $(PEGASUS_TOOLS_DEPS)
regmod_LIBRARIES = pegclient

$(eval $(call component,regmod,BINARIES))

ifeq ($(INTREE_PEGASUS),1)
$(regmod_TARGET) : $(PEGASUS_IMP_REPOSITORY)
endif


