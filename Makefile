AWK=awk
SED=sed
CURL=curl --negotiate -u :
BISON=bison
FLEX=flex

define component
_$(1)_SOURCES = $$(addprefix $$($(1)_DIR)/,$$($(1)_SOURCES))
$(1)_OBJS = $$(patsubst %.cpp,%.o,$$(_$(1)_SOURCES))
$(2) += $$($(1)_TARGET)

$$($(1)_TARGET) : $$($(1)_OBJS)

COMPONENTS += $(1)
endef

PRESET ?= default
include presets/$(PRESET).mk

CIM_SCHEMA_DIR=CIMPrelim226

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

include mk/server-$(CIM_SERVER).mk

.PHONY: all

ifneq ($(CIM_SERVER),esxi)
all : lib$(PROVIDER_LIBRARY).so

else

all : esxi-solarflare.tar.gz

endif


include cimple/lib/sources.mk
include cimple/tools/lib/sources.mk

include mk/interface-$(CIM_INTERFACE).mk
ifeq ($(DEVELOPMENT),1)
include cimple/tools/file2c/sources.mk
include cimple/tools/libmof/sources.mk
include cimple/tools/libgencommon/sources.mk
include cimple/tools/genclass/sources.mk
include cimple/tools/genmod/sources.mk
include cimple/tools/genprov/sources.mk
endif

include sources.mk

LDFLAGS += -Wl,-rpath=$(PROVIDER_LIBPATH)


.PHONY: register unregister install

include mk/rules-$(CIM_SERVER).mk

install : all
	cp lib$(PROVIDER_LIBRARY).so $(PROVIDER_LIBPATH)

%.d: %.cpp
	@set -e; rm -f $@; \
	$(CXX) -MM $(CPPFLAGS) $< > $@.$$$$; \
	$(SED) 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

include $(patsubst %.cpp,%.d,$(foreach comp,$(COMPONENTS),$(_$(comp)_SOURCES)))

%_Yacc.cpp : %.y
	$(BISON) --defines=$(patsubst %.cpp,%.h,$@) -p$(notdir $*)_ -o$@ $<

%_Lex.cpp : %.l
	$(FLEX) -P$(notdir $*)_ -o$@ $<

$(STATIC_LIBRARIES) : %.a:
	$(AR) crsu $@ $^

link_static_to_shared_start = -Wl,-whole-archive
link_static_to_shared_end = -Wl,-no-whole-archive

$(SHARED_LIBRARIES) : %.so:
	$(CXX) -shared -o $@ $(LDFLAGS) $(CXXFLAGS) $(filter %.o,$^) \
	-L. $(link_static_to_shared_start) $(patsubst lib%.a,-l%,$(filter %.a,$+)) $(link_static_to_shared_end)	$(addprefix -l,$(LIBRARIES))

$(BINARIES) : %:
	$(CXX) -o $@ $(LDFLAGS) $(CXXFLAGS) $(filter %.o,$^) \
	-L. $(patsubst lib%.a,-l%,$(filter %.a,$+))	$(addprefix -l,$(LIBRARIES))

CLEAN_TARGETS = $(addprefix clean-,$(COMPONENTS))

.PHONY: clean $(CLEAN_TARGETS)

clean : $(CLEAN_TARGETS)

$(CLEAN_TARGETS) : clean-% :
	-rm $($*_OBJS)
	-rm $(patsubst %.cpp,%.d,$(_$*_SOURCES))
	-rm $($*_TARGET)

