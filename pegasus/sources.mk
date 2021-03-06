##########################################################################
##! \file 
## <L5_PRIVATE L5_SCRIPT>
## \author  OktetLabs
##  \brief  Pegasus buid
##   \date  2013/10/02
##    \cop  (c) Solarflare Communications Inc.
## </L5_PRIVATE>
##
##########################################################################

CIM_INTERFACE ?= pegasus

##! If the variable is set, Pegasus will be build and installed inside the build tree,
## though not as the part of the regular build process
## \sa pegasus_build()
ifeq ($(INTREE_PEGASUS),1)
$(info Using in-tree Pegasus build)
PEGASUS_SETUP_SUBDIR ?= /setup

##! Root of the Pegasus source tree
override PEGASUS_ROOT = $(abspath pegasus)

##! The location where Pegasus is installed
override PEGASUS_HOME = $(PEGASUS_ROOT)$(PEGASUS_SETUP_SUBDIR)
else
ifeq ($(and $(PEGASUS_ROOT),$(PEGASUS_HOME),ok),ok)
else
$(error OpenPegasus is not installed or cannot be found!)
endif
endif

PEGASUS_LIBDIR ?= $(PEGASUS_HOME)/lib
PEGASUS_INCLUDES ?= $(PEGASUS_ROOT)/src
PEGASUS_BINPATH ?= $(PEGASUS_HOME)/bin
PEGASUS_SERVER ?= $(PEGASUS_BINPATH)/cimserver
PEGASUS_REPODIR ?= $(PEGASUS_HOME)/repository
PEGASUS_OWN_REPOSITORY = $(PEGASUS_REPODIR)/root#PG_Internal
PEGASUS_IMP_REPOSITORY = $(PEGASUS_REPODIR)/$(subst /,\#,$(IMP_NAMESPACE))
PEGASUS_START_CONF ?= /var/tmp/cimserver_start.conf
PEGASUS_RUN_OPTS ?= traceComponents=all traceLevel=4

PGCIMMOFL = cimmofl

PEGASUS_UPSTREAM_VERSION ?= 2.11.1
PEGASUS_UPSTREAM_TARBALL = pegasus-$(PEGASUS_UPSTREAM_VERSION).tar.gz
PEGASUS_UPSTREAM_REPOSITORY = $(DEFAULT_UPSTREAM_REPOSITORY)


PEGASUS_SCHEMA_DIR = CIM$(if $(CIM_SCHEMA_VERSION_EXPERIMENTAL),Prelim)$(CIM_SCHEMA_VERSION_MAJOR)$(CIM_SCHEMA_VERSION_MINOR)

CIM_SCHEMA_DIR ?= $(PEGASUS_ROOT)/Schemas/$(PEGASUS_SCHEMA_DIR)
CIM_SCHEMA_ROOTFILE = $(CIM_SCHEMA_DIR)/DMTF/cim_schema_$(CIM_SCHEMA_VERSION_MAJOR).$(CIM_SCHEMA_VERSION_MINOR).$(CIM_SCHEMA_VERSION_PATCHLEVEL).mof

PROVIDER_LIBPATH ?= $(PEGASUS_LIBDIR)
PROVIDER_LIBRARY_SO = $(PROVIDER_LIBPATH)/lib$(PROVIDER_LIBRARY).so

pegasus_PURPOSE = target

pegasus_PROVIDE_LIBRARIES = pegprm \
						    pegprovidermanager \
					    	pegprovider \
							pegconfig \
					 		pegcommon

pegasus_PROVIDE_LDFLAGS = -L$(PEGASUS_LIBDIR) -Wl,-rpath=$(PEGASUS_LIBDIR)
pegasus_INCLUDES = $(PEGASUS_INCLUDES)
pegasus_PROVIDE_CPPFLAGS = -I$(pegasus_INCLUDES) -DPEGASUS_PLATFORM_$(CIMPLE_PLATFORM)

pegasus_TARGET = $(PEGASUS_SERVER)
pegasus_DIR = $(PEGASUS_ROOT)

ifeq ($(INTREE_PEGASUS),1)

.SECONDARY : $(PEGASUS_UPSTREAM_TARBALL) $(CIM_SCHEMA_ZIP)
$(PEGASUS_UPSTREAM_TARBALL):
	$(CURL) $(PEGASUS_UPSTREAM_REPOSITORY)/$(PEGASUS_UPSTREAM_TARBALL) -o $@


.PHONY: pegasus-build

##! Build the OpenPegasus
pegasus-build : $(PEGASUS_SERVER)

PEGASUS_COMPONENTS = $(PEGASUS_SERVER) $(CIM_SCHEMA_ROOTFILE) \
					$(PEGASUS_OWN_REPOSITORY) \
					clean-pegasus

pegasus_EXTRA_CLEAN = make -C $(PEGASUS_ROOT) clean

EXTRA_CLEAN_TARGETS += clean-pegasus

export PEGASUS_ROOT
export PEGASUS_HOME
export PEGASUS_PLATFORM
export PEGASUS_DEBUG
export PEGASUS_CIM_SCHEMA
export PEGASUS_DISABLE_PROV_USERCTXT

PATH_WITH_PEGASUS := $(PEGASUS_BINPATH):$(PATH)

$(PEGASUS_COMPONENTS) : override PEGASUS_ROOT=$(abspath pegasus)
$(PEGASUS_COMPONENTS) : override PEGASUS_HOME=$(PEGASUS_ROOT)/setup
$(PEGASUS_COMPONENTS) : override PEGASUS_PLATFORM=$(CIMPLE_PLATFORM)
$(PEGASUS_COMPONENTS) : PEGASUS_DEBUG=true
$(PEGASUS_COMPONENTS) : PEGASUS_CIM_SCHEMA=$(PEGASUS_SCHEMA_DIR)
$(PEGASUS_COMPONENTS) : PEGASUS_DISABLE_PROV_USERCTXT=1

##! The actual target to build the OpenPegasus
## It shall not be invoked by the user, use pegasus_build() instead.
## \warning This target is not very robust: on the one hand, it does
## not track changes in the Pegasus source tree (as it actually delegates its
## functions to Pegasus build system), depending only on the schema root file
## and the Pegasus tarball. On the other hand, if invoked it will always unpack
## the tarball which may cause a total rebuild of Pegasus
$(PEGASUS_SERVER) : $(PEGASUS_UPSTREAM_TARBALL) $(CIM_SCHEMA_ROOTFILE)
	tar xzf $<
ifneq ($(PEGASUS_PREBUILT),1)
	PATH=$(PATH_WITH_PEGASUS) make -C $(PEGASUS_ROOT) build
endif

##! Initialize the OpenPegaus repository
$(PEGASUS_OWN_REPOSITORY) : $(PEGASUS_SERVER)
	PATH=$(PATH_WITH_PEGASUS) make -C $(PEGASUS_ROOT) repository

endif

.PHONY : pegasus-start
##! Start the OpenPegasus initializing its repository if needed
pegasus-start : $(PEGASUS_OWN_REPOSITORY)
	PATH=$(PATH_WITH_PEGASUS) $(PEGASUS_SERVER) $(PEGASUS_RUN_OPTS)

.PHONY : pegasus-stop
pegasus-stop : $(PEGASUS_START_CONF) 
	PATH=$(PATH_WITH_PEGASUS) $(PEGASUS_SERVER) -s

$(PEGASUS_START_CONF) : | $(PEGASUS_OWN_REPOSITORY)
	PATH=$(PATH_WITH_PEGASUS) $(PEGASUS_SERVER) $(PEGASUS_RUN_OPTS)

ifneq ($(INTREE_PEGASUS),1)

.PHONY : patch-pegasus-schema
patch-pegasus-schema: $(CIM_SCHEMA_ROOTFILE)

endif
