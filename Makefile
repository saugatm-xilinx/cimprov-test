AWK=awk
SED=sed
CURL=curl --negotiate -u :

PRESET ?= default
include presets/$(PRESET).mk

CIM_SCHEMA_DIR=CIM226Exp

ifeq ($(CIM_SERVER),pegasus)
CIM_INTERFACE=pegasus
PEGASUS_LIBDIR=$(PEGASUS_HOME)/lib
PEGASUS_INCLUDES=$(PEGASUS_ROOT)/src
else ifeq ($(CIM_SERVER),sfcb)
CIM_INTERFACE=cmpi
SFCBSTAGE=$(SFCB_PATH)/bin/sfcbstage
SFCBREPOS=$(SFCB_PATH)/bin/sfcbrepos -f
else ifeq ($(CIM_SERVER),esxi)
CIM_INTERFACE=cmpi
endif

IMP_NAMESPACE=root/solarflare
ifeq ($(CIM_SERVER),pegasus)
INTEROP_NAMESPACE=root/pg_interop
else
INTEROP_NAMESPACE=root/interop
endif
INTEROP_CLASSES=SF_RegisteredProfile SF_ReferencedProfile SF_ElementConformsToProfile

PROVIDER_LIBRARY=Solarflare

CPPFLAGS = -DCIMPLE_DEBUG -D_GNU_SOURCE 
CPPFLAGS += -I. -Icimple
CPPFLAGS += -DCIMPLE_PLATFORM_$(CIMPLE_PLATFORM)

ifneq ($(CIM_SERVER),esxi)
CXXFLAGS = -fPIC  -pthread
CXXFLAGS += -Wall -W -Wno-unused -Werror
CXXFLAGS += -g 

ifeq ($(CIMPLE_PLATFORM),LINUX_IX86_GNU)
CXXFLAGS += -m32
endif

endif
CXXFLAGS += -fvisibility=hidden

ifneq ($(CIM_SERVER),esxi)
CPPFLAGS += -DSF_IMPLEMENTATION_NS=\"$(IMP_NAMESPACE)\"
CPPFLAGS += -DSF_INTEROP_NS=\"$(INTEROP_NAMESPACE)\"
endif

LIBRARIES = pthread

include sources.mak
include cimple/lib/sources.mak
include cimple/tools/lib/sources.mak

ifeq ($(CIM_INTERFACE),pegasus)

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

include cimple/pegasus/sources.mak
include cimple/tools/regmod/sources.mak

LIBRARIES += pegprm \
		    pegprovidermanager \
	    	pegprovider \
			pegconfig \
	 		pegcommon

LDFLAGS += -L$(PEGASUS_LIBDIR) -Wl,-rpath=$(PEGASUS_LIBDIR)
CPPFLAGS += -I$(PEGASUS_INCLUDES)
CPPFLAGS += -DPEGASUS_PLATFORM_$(CIMPLE_PLATFORM)

else ifeq ($(CIM_INTERFACE),cmpi)
CPPFLAGS += -DCIMPLE_CMPI_MODULE
ifneq ($(CIM_SERVER),esxi)
CPPFLAGS += -Icmpi
endif

include cimple/cmpi/sources.mak

else
$(error Unknown CIM interface: $(CIM_INTERFACE))
endif

LDFLAGS += -Wl,-rpath=$(PROVIDER_LIBPATH)

.PHONY: all

ifneq ($(CIM_SERVER),esxi)
all : lib$(PROVIDER_LIBRARY).so

lib$(PROVIDER_LIBRARY).so : $(patsubst %.cpp,%.o,$(lib$(PROVIDER_LIBRARY)_SOURCES))
	$(CXX) -shared -o $@ $(LDFLAGS) $(CXXFLAGS) $^ $(addprefix -l,$(LIBRARIES)) 

else

all : esxi-solarflare.tar.gz

endif

ifeq ($(CIM_SERVER),pegasus)

regmod : $(patsubst %.cpp,%.o,$(regmod_SOURCES))
	$(CXX) -o $@ $(LDFLAGS) $(CXXFLAGS) $^ -L. -l$(PROVIDER_LIBRARY) $(addprefix -l,$(LIBRARIES))

endif

.PHONY: register unregister install

ifeq ($(CIM_SERVER),pegasus)

PROVIDER_LIBPATH=$(PEGASUS_LIBDIR)
PROVIDER_LIBRARY_SO=$(PROVIDER_LIBPATH)/lib$(PROVIDER_LIBRARY).so

register: install regmod
	./regmod -n $(IMP_NAMESPACE) -c $(PROVIDER_LIBRARY_SO)
	./regmod -n $(INTEROP_NAMESPACE) -c $(PROVIDER_LIBRARY_SO) $(INTEROP_CLASSES)

unregister: regmod
	./regmod -n $(INTEROP_NAMESPACE) -u -c $(PROVIDER_LIBRARY_SO) $(INTEROP_CLASSES)
	./regmod -n $(IMP_NAMESPACE) -u -c $(PROVIDER_LIBRARY_SO)

PEGASUS_UPSTREAM_VERSION=2.11.1
PEGASUS_UPSTREAM_TARBALL=pegasus-$(PEGASUS_UPSTREAM_VERSION).tar.gz
PEGASUS_UPSTREAM_REPOSITORY=https://oktetlabs.ru/purgatorium/prj/level5/cim

CIM_SCHEMA_ZIP=cim_schema_2.26.0Experimental-MOFs.zip
CIM_SCHEMA_REPOSITORY=$(PEGASUS_UPSTREAM_REPOSITORY)

.PRECIOUS : $(PEGASUS_UPSTREAM_TARBALL) $(CIM_SCHEMA_ZIP)
$(PEGASUS_UPSTREAM_TARBALL):
	$(CURL) $(PEGASUS_UPSTREAM_REPOSITORY)/$(PEGASUS_UPSTREAM_TARBALL) -o $@

$(CIM_SCHEMA_ZIP):
	$(CURL) $(CIM_SCHEMA_REPOSITORY)/$(CIM_SCHEMA_ZIP) -o $@

.PHONY: pegasus-build

export PEGASUS_HOME
export PEGASUS_ROOT
export PEGASUS_PLATFORM

pegasus-build : override PEGASUS_ROOT=$(CURDIR)/pegasus
pegasus-build : override PEGASUS_HOME=$(PEGASUS_ROOT)/setup
pegasus-build : override PEGASUS_PLATFORM=$(CIMPLE_PLATFORM)
pegasus-build : export PEGASUS_DEBUG=true

pegasus-build: $(PEGASUS_UPSTREAM_TARBALL) $(CIM_SCHEMA_ZIP)
	tar xzf $<
	test -d $(PEGASUS_ROOT)
	mkdir -p $(PEGASUS_HOME)
	make -C $(PEGASUS_ROOT) build
	unzip $(CIM_SCHEMA_ZIP) -d $(PEGASUS_ROOT)/Schemas/$(CIM_SCHEMA_DIR)

endif

ifneq ($(CIM_SERVER),pegasus)
repository.reg : repository.mof mof2reg.awk
	$(AWK) -f mof2reg.awk -vPRODUCTNAME=$(PROVIDER_LIBRARY) -vNAMESPACE=$(IMP_NAMESPACE) $< >$@

interop.reg : interop.mof mof2reg.awk
	$(AWK) -f mof2reg.awk -vPRODUCTNAME=$(PROVIDER_LIBRARY) -vNAMESPACE=$(INTEROP_NAMESPACE) $< >$@

endif

ifeq ($(CIM_SERVER),sfcb)

register: repository.reg interop.reg install
	$(SFCBSTAGE) -n $(IMP_NAMESPACE) -r repository.reg repository.mof
	$(SFCBSTAGE) -n $(INTEROP_NAMESPACE) -r interop.reg repository.mof
	$(SFCBREPOS)

ifeq ($(SFCB_PATH),)
$(error No path to SFCB installation)
endif

PROVIDER_LIBPATH=$(SFCB_PATH)/lib

endif

ifeq ($(CIM_SERVER),esxi)

ALL_HEADERS = $(wildcard *.h) $(wildcard cimple/*.h) $(wildcard cimple/cmpi/*.h)
ESXI_SUBDIR = esxi_solarflare
ESXI_PROJECT_NAME = solarflare
ESXI_SRC_PATH = $(ESXI_SUBDIR)/$(ESXI_PROJECT_NAME)
ESXI_GENERATED = $(lib$(PROVIDER_LIBRARY)_SOURCES) $(ALL_HEADERS)
ESXI_GENERATED += repository.mof interop.mof
ESXI_GENERATED += repository.reg.in interop.reg.in
ESXI_GENERATED += Makefile.am

esxi-solarflare.tar.gz : $(addprefix $(ESXI_SRC_PATH)/,$(ESXI_GENERATED)) $(ESXI_SUBDIR)/esxi_bootstrap.sh
	cd $(ESXI_SUBDIR); tar -czf ../$@ *

$(ESXI_SRC_PATH)/repository.reg.in : repository.reg
	mkdir -p $(dir $@)	
	$(SED) 's!$(IMP_NAMESPACE)!@smash_namespace@!g' <$< >$@

$(ESXI_SRC_PATH)/interop.reg.in : interop.reg
	mkdir -p $(dir $@)	
	$(SED) 's!$(INTEROP_NAMESPACE)!@sfcb_interop_namespace@!g' <$< >$@


$(ESXI_SRC_PATH)/% : %
	mkdir -p $(dir $@)
	cp $< $@

$(ESXI_SRC_PATH)/Makefile.am : Makefile
	echo "bin_PROGRAMS=lib$(PROVIDER_LIBRARY).so" >$@
	echo "lib$(PROVIDER_LIBRARY)_so_SOURCES=$(firstword $(lib$(PROVIDER_LIBRARY)_SOURCES))" >>$@
	for src in $(wordlist 2,$(words $(lib$(PROVIDER_LIBRARY)_SOURCES)),$(lib$(PROVIDER_LIBRARY)_SOURCES)); do \
		echo "lib$(PROVIDER_LIBRARY)_so_SOURCES+=$${src}" >>$@; \
	done
	echo "lib$(PROVIDER_LIBRARY)_so_CPPFLAGS=$(CPPFLAGS) -I\$$(srcdir) -I\$$(srcdir)/cimple" >>$@
	echo "lib$(PROVIDER_LIBRARY)_so_CPPFLAGS+= -DSF_IMPLEMENTATION_NS=\\\"\$$(smash_namespace)\\\"" >>$@
	echo "lib$(PROVIDER_LIBRARY)_so_CPPFLAGS+= -DSF_INTEROP_NS=\\\"\$$(sfcb_interop_namespace)\\\"" >>$@
	echo "lib$(PROVIDER_LIBRARY)_so_CXXFLAGS=\$$(CFLAGS) $(CXXFLAGS)" >>$@
	echo "lib$(PROVIDER_LIBRARY)_so_LDFLAGS=-shared -L\$$(srcdir)" >>$@
	echo "NAMESPACES=\$$(smash_namespace) \$$(sfcb_interop_namespace)" >>$@
	echo "if ENABLE_SFCB" >>$@
	echo "dist_sfcb_interop_ns_DATA = interop.mof" >>$@
	echo "dist_sfcb_ns_DATA = repository.mof" >>$@
	echo "dist_sfcb_reg_DATA = repository.reg interop.reg" >>$@
	echo "endif" >>$@
endif

install : all
	cp lib$(PROVIDER_LIBRARY).so $(PROVIDER_LIBPATH)

%.d: %.cpp
	@set -e; rm -f $@; \
	$(CXX) -MM $(TOOLS_CPPFLAGS) $(CPPFLAGS) $< > $@.$$$$; \
	$(SED) 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

include $(patsubst %.cpp,%.d,$(lib$(PROVIDER_LIBRARY)_SOURCES))
include $(patsubst %.cpp,%.d,$(regmod_SOURCES))

.PHONY: clean

clean :
	-rm -f *.[od]
	-rm -f *.so
	-rm -f regmod
	-rm -f cimple/lib/*.[od]
	-rm -f cimple/cmpi/*.[od]
	-rm -f cimple/pegasus/*.[od]
	-rm -f $(TOOLSLIB_SRC_PATH)/*.[od]
	-rm -f cimple/tools/regmod/*.[od]

