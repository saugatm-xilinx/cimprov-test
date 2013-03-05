ifeq ($(INTREE_PEGASUS),1)
$(info Using in-tree Pegasus build)
override PEGASUS_ROOT=$(CURDIR)/pegasus
override PEGASUS_HOME=$(PEGASUS_ROOT)/setup
ifeq ($(wildcard $(PEGASUS_HOME)),)
$(error In-tree Pegasus build is not complete; try running 'make pegasus-build')
endif
else
ifeq ($(and $(PEGASUS_ROOT),$(PEGASUS_HOME),ok),ok)
else
$(error OpenPegasus is not installed or cannot be found!)
endif
endif

CPPFLAGS += -DCIMPLE_PEGASUS_MODULE

include cimple/pegasus/sources.mk
include cimple/tools/regmod/sources.mk

LIBRARIES += pegprm \
		    pegprovidermanager \
	    	pegprovider \
			pegconfig \
	 		pegcommon

LDFLAGS += -L$(PEGASUS_LIBDIR) -Wl,-rpath=$(PEGASUS_LIBDIR)
CPPFLAGS += -I$(PEGASUS_INCLUDES)
CPPFLAGS += -DPEGASUS_PLATFORM_$(CIMPLE_PLATFORM)
