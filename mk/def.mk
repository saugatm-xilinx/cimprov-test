AWK=awk
SED=sed
CURL=curl --negotiate -u :
BISON=bison
FLEX=flex
HG=hg

override top_CPPFLAGS := $(CPPFLAGS)
override top_CXXFLAGS := $(or $(CXXFLAGS),$(CFLAGS))
override top_LDFLAGS := $(LDFLAGS)
override top_LIBRARIES := 

define _augment_vars
_$(1)_DEP_$(2) = $$(foreach d,$$(_$(1)_DEPENDS),$$($$(d)_PROVIDE_$(2)) )
$(1)_$(2) ?= $$($(1)_PROVIDE_$(2))
endef

define component
ifeq ($$($(1)_TARGET),)
$$(error Target not specified for $(1))
endif
ifeq ($$($(1)_DIR),)
$$(error Directory not specified for $(1))
endif

__$(1)_DEPENDS = $$(foreach d,$$($(1)_DEPENDS),$$(_$$(d)_DEPENDS) )
_$(1)_DEPENDS = $$(filter-out $$(__$(1)_DEPENDS),$$($(1)_DEPENDS)) $$(__$(1)_DEPENDS)

ifeq ($(2),STATIC_LIBRARIES)
$(1)_PROVIDE_LIBRARIES += $$(patsubst lib%.a,%,$$($(1)_TARGET))
endif

$(1)_PROVIDE_CPPFLAGS += $$(addprefix -I,$$($(1)_INCLUDES))
$(call _augment_vars,$(1),CPPFLAGS)
$(call _augment_vars,$(1),LDFLAGS)
$(call _augment_vars,$(1),LIBRARIES)

_$(1)_SOURCES = $$(addprefix $$($(1)_DIR)/,$$($(1)_SOURCES))
_$(1)_GENERATED = $$(addprefix $$($(1)_DIR)/,$$($(1)_GENERATED)) $$(foreach d,$$(_$(1)_DEPENDS),$$(_$$(d)_GENERATED) )
ifneq ($(2),)
ALL_SOURCES += $$(_$(1)_SOURCES)
_$(1)_HEADERS = $$(foreach incdir,$$($(1)_INCLUDES),$$(wildcard $$(incdir)/*.h) )
ALL_HEADERS += $$(_$(1)_HEADERS)
$(1)_OBJS = $$(patsubst %.cpp,%.o,$$(_$(1)_SOURCES))
$(2) += $$($(1)_TARGET)

$$($(1)_OBJS) $$(patsubst %.o,%.d,$$($(1)_OBJS)) : $$(_$(1)_GENERATED)

$$($(1)_TARGET) : $$(foreach d,$$($(1)_DEPENDS) $$($(1)_BUILD_DEPENDS),$$($$(d)_TARGET)) $$($(1)_OBJS) 
$$($(1)_TARGET) : override CXXFLAGS = $$(top_CXXFLAGS) $($(1)_CXXFLAGS)

ifneq ($(2),STATIC_LIBRARIES)
$$($(1)_TARGET) : override LDFLAGS = $$(top_LDFLAGS) $$($(1)_LDFLAGS) $$(_$(1)_DEP_LDFLAGS)
$$($(1)_TARGET) : override LIBRARIES = $$(filter-out $$(_$(1)_DEP_LIBRARIES),$$($(1)_LIBRARIES)) $$(_$(1)_DEP_LIBRARIES) $$(top_LIBRARIES)
endif

$$($(1)_DIR)/%.o $$($(1)_DIR)/%.d : CPPFLAGS = $$(top_CPPFLAGS) $$($(1)_CPPFLAGS) $$(_$(1)_DEP_CPPFLAGS)
$$($(1)_DIR)/%.o : override CXXFLAGS = $$(top_CXXFLAGS) $($(1)_CXXFLAGS)
endif

COMPONENTS += $(1)
endef
