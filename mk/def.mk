##########################################################################
##! \file ./mk/def.mk
## <L5_PRIVATE L5_SOURCE>
## \author  OktetLabs
##  \brief  CIM Provider
##   \date  2013/10/02
##    \cop  (c) Solarflare Communications Inc.
## </L5_PRIVATE>
##
##########################################################################

AWK=awk
SED=sed
CURL=curl --negotiate -u :
BISON=bison
FLEX=flex
HG=hg
M4=m4 -P
DOXYGEN=doxygen

override host_CXX := $(or $(CXX),$(CC),c++)
override host_CC := $(or $(CC),$(CXX),c++)
override host_AR := $(or $(AR),ar)
override host_CPPFLAGS := $(CPPFLAGS)
override host_CXXFLAGS := $(or $(CXXFLAGS),$(CFLAGS))
override host_CFLAGS := $(CFLAGS)
override host_LDFLAGS := $(LDFLAGS)
override host_LIBRARIES := 
override host_SYSLIBRARIES := 

##! Target C++ compiler
target_CXX = $(host_CXX)

##! Target C compiler
target_CC = $(host_CC)

##! Target static archiver
target_AR = $(host_AR)

##! Target C++ preprocessor flags
target_CPPFLAGS = $(host_CPPFLAGS)

##! Target C++ compiler flags
target_CXXFLAGS = $(host_CXXFLAGS)

##! Target C compiler flags
target_CFLAGS = $(host_CFLAGS)

##! Target linker flags
target_LDFLAGS = $(host_LDFLAGS)

##! Target user libraries to link
target_LIBRARIES = $(host_LIBRARIES)

##! Target system libraries to link
target_SYSLIBRARIES = $(host_SYSLIBRARIES)

define _augment_vars
_$(1)_DEP_$(2) = $$(foreach d,$$(_$(1)_DEPENDS),$$($$(d)_PROVIDE_$(2)) )
$(1)_$(2) ?= $$($(1)_PROVIDE_$(2))
endef

##! %List of static libraries
override STATIC_LIBRARIES =

##! %List of shared libraries
override SHARED_LIBRARIES =

##! %List of binaries
override BINARIES =

##! Declare a component inside a build system
##
## \param _1  component name
## \param _2  component type:
## - #STATIC_LIBRARIES()  component is a static library
## - #SHARED_LIBRARIES()  component is a shared library 
## - #BINARIES() component is an executable
## \sa mk/sample-sources.mk
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
ifneq ($$($(1)_PURPOSE)+$$(CROSS_BUILD),target+1)
ALL_SOURCES += $$(_$(1)_SOURCES)
endif
_$(1)_HEADERS = $$(foreach incdir,$$($(1)_INCLUDES),$$(wildcard $$(incdir)/*.h) $$(wildcard $$(TOP)/$$(incdir)/*.h))
ALL_HEADERS += $$(_$(1)_HEADERS)
$(1)_OBJS = $$(patsubst %.c,%.o,$$(patsubst %.cpp,%.o,$$(_$(1)_SOURCES)))
override $(2) += $$($(1)_TARGET)

$$($(1)_OBJS) $$(patsubst %.o,%.d,$$($(1)_OBJS)) : $$(_$(1)_GENERATED)

$$($(1)_TARGET) : override CXX = $$($$($(1)_PURPOSE)_CXX)
$$($(1)_TARGET) : override CC = $$($$($(1)_PURPOSE)_CC)
$$($(1)_TARGET) : override AR = $$($$($(1)_PURPOSE)_AR)

$$($(1)_TARGET) : $$(foreach d,$$($(1)_DEPENDS) $$($(1)_BUILD_DEPENDS),$$($$(d)_TARGET)) $$($(1)_OBJS) 
$$($(1)_TARGET) : override CXXFLAGS = $$($$($(1)_PURPOSE)_CXXFLAGS) $($(1)_CXXFLAGS)
$$($(1)_TARGET) : override CFLAGS = $$($$($(1)_PURPOSE)_CFLAGS) $($(1)_CFLAGS)

ifneq ($(2),STATIC_LIBRARIES)
$$($(1)_TARGET) : override LDFLAGS = $$($$($(1)_PURPOSE)_LDFLAGS) $$($(1)_LDFLAGS) $$(_$(1)_DEP_LDFLAGS)
$$($(1)_TARGET) : override LIBRARIES = $$(filter-out $$(_$(1)_DEP_LIBRARIES),$$($(1)_LIBRARIES)) $$(_$(1)_DEP_LIBRARIES) $$($$($(1)_PURPOSE)_LIBRARIES)
$$($(1)_TARGET) : override SYSLIBRARIES = $$(filter-out $$(_$(1)_DEP_SYSLIBRARIES),$$($(1)_SYSLIBRARIES)) $$(_$(1)_DEP_SYSLIBRARIES) $$($$($(1)_PURPOSE)_SYSLIBRARIES)
endif

$$($(1)_DIR)/%.o $$($(1)_DIR)/%.d : CPPFLAGS = $$($$($(1)_PURPOSE)_CPPFLAGS) $$($(1)_CPPFLAGS) $$(_$(1)_DEP_CPPFLAGS)
$$($(1)_DIR)/%.o : override CXXFLAGS = $$($$($(1)_PURPOSE)_CXXFLAGS) $($(1)_CXXFLAGS) $$($(1)_$$(notdir $$*)_CXXFLAGS)
$$($(1)_DIR)/%.o : override CFLAGS = $$($$($(1)_PURPOSE)_CFLAGS) $($(1)_CFLAGS) $$($(1)_$$(notdir $$*)_CFLAGS)
$$($(1)_DIR)/%.o $$($(1)_DIR)/%.d : override CXX = $$($$($(1)_PURPOSE)_CXX) 
$$($(1)_DIR)/%.o $$($(1)_DIR)/%.d : override CC = $$($$($(1)_PURPOSE)_CC) 
endif

COMPONENTS += $(1)
endef

##! declare an archive component inside a build system
##
## \param _1  component name

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
ifeq ($(MAKECMDGOALS),doc)
_DO_NOT_GENERATE := 1
endif
ifeq ($(MAKECMDGOALS),userdoc)
_DO_NOT_GENERATE := 1
endif

##! A command line to list all files known to Mercurial repository
## \note we cannot use a simpler hg manifest here because it would not list added but yet uncommitted files
HGLISTALL =  $(HG) st --all | grep -v "^[?IR]" | sed "s/^.\s//"
