AWK=awk
SED=sed
CURL=curl --negotiate -u :
BISON=bison
FLEX=flex
HG=hg
M4=m4 -P

override host_CXX := $(or $(CXX),$(CC),c++)
override host_AR := $(or $(AR),ar)
override host_CPPFLAGS := $(CPPFLAGS)
override host_CXXFLAGS := $(or $(CXXFLAGS),$(CFLAGS))
override host_LDFLAGS := $(LDFLAGS)
override host_LIBRARIES := 
override host_SYSLIBRARIES := 

target_CXX = $(host_CXX)
target_AR = $(host_AR)
target_CPPFLAGS = $(host_CPPFLAGS)
target_CXXFLAGS = $(host_CXXFLAGS)
target_LDFLAGS = $(host_LDFLAGS)
target_LIBRARIES = $(host_LIBRARIES)
target_SYSLIBRARIES = $(host_SYSLIBRARIES)

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

$(1)_PURPOSE ?= target

ifeq ($$($(1)_PURPOSE),host)
ifneq ($$(findstring target,$$(foreach d,$$($(1)_DEPENDS),$$($$(d)_PURPOSE))),)
$$(error Host component $(1) depends on some target components)
endif
else ifeq ($$($(1)_PURPOSE),target)
ifneq ($$(findstring host,$$(foreach d,$$($(1)_DEPENDS),$$($$(d)_PURPOSE))),)
$$(error Target component $(1) depends on some host components)
endif
else
$$(error Unknown purpose for $(1): $$($(1)_PURPOSE))
endif

__$(1)_DEPENDS = $$(foreach d,$$($(1)_DEPENDS),$$(_$$(d)_DEPENDS) )
_$(1)_DEPENDS = $$(filter-out $$(__$(1)_DEPENDS),$$($(1)_DEPENDS)) $$(__$(1)_DEPENDS)

ifeq ($(2),STATIC_LIBRARIES)
$(1)_PROVIDE_LIBRARIES += $$(patsubst lib%.a,%,$$($(1)_TARGET))
endif

$(1)_PROVIDE_CPPFLAGS += $$(addprefix -I,$$($(1)_INCLUDES))
$(1)_PROVIDE_CPPFLAGS += $$(addprefix -I$$(TOP)/,$$($(1)_INCLUDES))
$(call _augment_vars,$(1),CPPFLAGS)
$(call _augment_vars,$(1),LDFLAGS)
$(call _augment_vars,$(1),LIBRARIES)
$(call _augment_vars,$(1),SYSLIBRARIES)

_$(1)_SOURCES = $$(addprefix $$($(1)_DIR)/,$$($(1)_SOURCES))
_$(1)_GENERATED = $$(addprefix $$($(1)_DIR)/,$$($(1)_GENERATED)) $$(foreach d,$$(_$(1)_DEPENDS),$$(_$$(d)_GENERATED) )
ifneq ($(2),)
ALL_SOURCES += $$(_$(1)_SOURCES)
_$(1)_HEADERS = $$(foreach incdir,$$($(1)_INCLUDES),$$(wildcard $$(incdir)/*.h) $$(wildcard $$(TOP)/$$(incdir)/*.h))
ALL_HEADERS += $$(_$(1)_HEADERS)
$(1)_OBJS = $$(patsubst %.cpp,%.o,$$(_$(1)_SOURCES))
$(2) += $$($(1)_TARGET)

$$($(1)_OBJS) $$(patsubst %.o,%.d,$$($(1)_OBJS)) : $$(_$(1)_GENERATED)

$$($(1)_TARGET) : override CXX = $$($$($(1)_PURPOSE)_CXX)
$$($(1)_TARGET) : override AR = $$($$($(1)_PURPOSE)_AR)

$$($(1)_TARGET) : $$(foreach d,$$($(1)_DEPENDS) $$($(1)_BUILD_DEPENDS),$$($$(d)_TARGET)) $$($(1)_OBJS) 
$$($(1)_TARGET) : override CXXFLAGS = $$($$($(1)_PURPOSE)_CXXFLAGS) $($(1)_CXXFLAGS)

ifneq ($(2),STATIC_LIBRARIES)
$$($(1)_TARGET) : override LDFLAGS = $$($$($(1)_PURPOSE)_LDFLAGS) $$($(1)_LDFLAGS) $$(_$(1)_DEP_LDFLAGS)
$$($(1)_TARGET) : override LIBRARIES = $$(filter-out $$(_$(1)_DEP_LIBRARIES),$$($(1)_LIBRARIES)) $$(_$(1)_DEP_LIBRARIES) $$($$($(1)_PURPOSE)_LIBRARIES)
$$($(1)_TARGET) : override SYSLIBRARIES = $$(filter-out $$(_$(1)_DEP_SYSLIBRARIES),$$($(1)_SYSLIBRARIES)) $$(_$(1)_DEP_SYSLIBRARIES) $$($$($(1)_PURPOSE)_SYSLIBRARIES)
endif

$$($(1)_DIR)/%.o $$($(1)_DIR)/%.d : CPPFLAGS = $$($$($(1)_PURPOSE)_CPPFLAGS) $$($(1)_CPPFLAGS) $$(_$(1)_DEP_CPPFLAGS)
$$($(1)_DIR)/%.o : override CXXFLAGS = $$($$($(1)_PURPOSE)_CXXFLAGS) $($(1)_CXXFLAGS) $$($(1)_$$(notdir $$*)_CXXFLAGS)
$$($(1)_DIR)/%.o $$($(1)_DIR)/%.d : override CXX = $$($$($(1)_PURPOSE)_CXX) 
endif

COMPONENTS += $(1)
endef

define archive_component
ifeq ($$($(1)_TARGET),)
$$(error Target not specified for $(1))
endif

$(1)_COMPONENTS = $$(foreach comp,$$(COMPONENTS),$$(if $$(findstring target,$$($$(comp)_PURPOSE)),$$(comp) ,))

$(1)_DISTFILES = $$(foreach comp,$$($(1)_COMPONENTS),$$(_$$(comp)_SOURCES) $$(_$$(comp)_HEADERS) )
$(1)_DISTFILES += $$($(1)_EXTRA_DISTFILES)

_$(1)_SOURCES = $$(addprefix $$($(1)_DIR)/,$$($(1)_SOURCES))
_$(1)_GENERATED = $$(addprefix $$($(1)_DIR)/,$$($(1)_GENERATED))

$(1)_DISTFILES += $$(_$(1)_SOURCES)

$$($(1)_TARGET) : $$($(1)_DISTFILES)

COMPONENTS += $(1)

endef

ifeq ($(MAKECMDGOALS),distname)
_DO_NOT_GENERATE := 1
endif
ifeq ($(MAKECMDGOALS),clean)
_DO_NOT_GENERATE := 1
endif
ifeq ($(MAKECMDGOALS),platform)
_DO_NOT_GENERATE := 1
endif
