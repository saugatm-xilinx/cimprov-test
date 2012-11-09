AWK=awk
SED=sed

CIM_SERVER=pegasus
CIMPLE_PLATFORM=LINUX_IX86_GNU

ifeq ($(CIM_SERVER),pegasus)
CIM_INTERFACE=pegasus
PEGASUS_LIBDIR=$(PEGASUS_HOME)/lib
PEGASUS_INCLUDES=$(PEGASUS_ROOT)/src
else ifeq ($(CIM_SERVER),sfcb)
CIM_INTERFACE=cmpi
SFCBSTAGE=sfcbstage
SFCBREPOS=sfcbrepos -f
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
CPPFLAGS += -Wall -W -Wno-unused -Werror
CPPFLAGS += -DCIMPLE_PLATFORM_$(CIMPLE_PLATFORM)

CXXFLAGS = -fPIC  -pthread
CXXFLAGS += -g -fvisibility=hidden
ifeq ($(CIMPLE_PLATFORM),LINUX_IX86_GNU)
CXXFLAGS += -m32
endif

CPPFLAGS += -DSF_IMPLEMENTATION_NS=\"$(IMP_NAMESPACE)\"
CPPFLAGS += -DSF_INTEROP_NS=\"$(INTEROP_NAMESPACE)\"

LIBRARIES = pthread

include sources.mak
include cimple/lib/sources.mak
include cimple/tools/lib/sources.mak

ifeq ($(CIM_INTERFACE),pegasus)
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
CPPFLAGS += -DCIMPLE_CMPI_MODULE -Icmpi

VPATH += cimple/cmpi
include cimple/cmpi/sources.mak

else
$(error Unknown CIM interface: $(CIM_INTERFACE))
endif

LDFLAGS += -Wl,-rpath=$(PROVIDER_LIBPATH)

.PHONY: all
all : lib$(PROVIDER_LIBRARY).so

lib$(PROVIDER_LIBRARY).so : $(patsubst %.cpp,%.o,$(lib$(PROVIDER_LIBRARY)_SOURCES))
	$(CXX) -shared -o $@ $(LDFLAGS) $^ $(addprefix -l,$(LIBRARIES))

ifeq ($(CIM_SERVER),pegasus)

regmod : $(patsubst %.cpp,%.o,$(regmod_SOURCES))
	$(CXX) -o $@ $(LDFLAGS) $^ -L. -l$(PROVIDER_LIBRARY) $(addprefix -l,$(LIBRARIES))

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

endif

ifeq ($(CIM_SERVER),sfcb)

repository.reg : repository.mof mof2reg.awk
	$(AWK) -f mof2reg.awk -vPRODUCTNAME=$(PROVIDER_LIBRARY) -vNAMESPACE=$(IMP_NAMESPACE) $< >$@

register: repository.reg interop.reg insmod
	$(SFCBSTAGE) -n $(IMP_NAMESPACE) -r repository.reg repository.mof
	$(SFCBSTAGE) -n $(INTEROP_NAMESPACE) -r interop.reg repository.mof
	$(SFCBREPOS)

ifeq ($(SFCB_PATH),)
$(error No path to SFCB installation)
endif

PROVIDER_LIBPATH=$(SFCB_PATH)/lib

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

