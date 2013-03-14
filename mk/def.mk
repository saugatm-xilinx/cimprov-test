AWK=awk
SED=sed
CURL=curl --negotiate -u :
BISON=bison
FLEX=flex

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

_$(1)_DEPENDS = $$($(1)_DEPENDS) $$(foreach d,$$($(1)_DEPENDS),$$(_$$(d)_DEPENDS) )

ifeq ($(2),STATIC_LIBRARIES)
$(1)_PROVIDE_LIBRARIES += $$(patsubst lib%.a,%,$$($(1)_TARGET))
endif

$(1)_PROVIDE_CPPFLAGS += $$(addprefix -I,$$($(1)_INCLUDES))
$(call _augment_vars,$(1),CPPFLAGS)
$(call _augment_vars,$(1),LDFLAGS)
$(call _augment_vars,$(1),LIBRARIES)

_$(1)_SOURCES = $$(addprefix $$($(1)_DIR)/,$$($(1)_SOURCES))
_$(1)_GENERATED = $$(addprefix $$($(1)_DIR)/,$$($(1)_GENERATED))
_ALL_GENERATED += $$(_$(1)_GENERATED)
ifneq ($(2),)
ALL_SOURCES += $$(_$(1)_SOURCES)
_$(1)_HEADERS = $$(foreach incdir,$$($(1)_INCLUDES),$$(wildcard $$(incdir)/*.h) )
ALL_HEADERS += $$(_$(1)_HEADERS)
$(1)_OBJS = $$(patsubst %.cpp,%.o,$$(_$(1)_SOURCES))
$(2) += $$($(1)_TARGET)

$$(patsubst %.o,%.d,$$($(1)_OBJS)) : $$(_ALL_GENERATED)

$$($(1)_TARGET) : $$($(1)_OBJS) $$(foreach d,$$($(1)_DEPENDS),$$($$(d)_TARGET))

ifneq ($(2),STATIC_LIBRARIES)
$$($(1)_TARGET) : LDFLAGS += $$($(1)_LDFLAGS) $$(_$(1)_DEP_LDFLAGS)
$$($(1)_TARGET) : LIBRARIES += $$($(1)_LIBRARIES) $$(_$(1)_DEP_LIBRARIES)
endif

$$($(1)_DIR)/%.o $$($(1)_DIR)/%.d : CPPFLAGS += $$($(1)_CPPFLAGS) $$(_$(1)_DEP_CPPFLAGS)
endif

COMPONENTS += $(1)
endef
