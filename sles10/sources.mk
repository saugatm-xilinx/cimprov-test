##########################################################################
#//#! \file ./sles10/sources.mk
## <L5_PRIVATE L5_SOURCE>
## \author  OktetLabs
##  \brief  CIM Provider
##   \date  2013/10/02
##    \cop  (c) Solarflare Communications Inc.
## </L5_PRIVATE>
##
##########################################################################

sles10_archive_TARGET = $(PROVIDER_TARBALL_DIR).tar.gz
sles10_archive_DIR = sles10

sles10_archive_EXTRA_DISTFILES += $(libcimobjects_DIR)/repository.mof \
								  $(libcimobjects_DIR)/interop.mof $(libcimobjects_DIR)/schema.mof \
				                  repository.reg \
								  $(CIM_SCHEMA_DIR) \
								  lib$(PROVIDER_LIBRARY).spec

sles10_archive_GENERATED += Makefile
sles10_archive_GENERATED += $(libcimobjects_DIR)/namespace.mof
sles10_archive_SOURCES = $(sles10_archive_GENERATED)

$(sles10_archive_TARGET) : TAR_TRANSFORM = !^Makefile$$!Makefile.orig! !^$(sles10_archive_DIR)/$(libcimobjects_DIR)/!$(libcimobjects_DIR)/! !^$(sles10_archive_DIR)/Makefile$$!Makefile! !^!$*/!

$(eval $(call archive_component,sles10_archive))

sles10_archive_CPPFLAGS = $(target_CPPFLAGS) $(foreach comp,$(sles10_archive_COMPONENTS),$($(comp)_CPPFLAGS) )

$(sles10_archive_DIR)/$(libcimobjects_DIR)/namespace.mof : $(libcimobjects_DIR)/namespace.mof \
	$(addprefix $(CIM_SCHEMA_PATCHDIR)/,$(CIM_SCHEMA_ADDON_MOFS))
	mkdir -p $(dir $@)
	cat $^ >$@

sles10_archive_CPP_DISTFILES = $(filter %.cpp,$(sles10_archive_DISTFILES))


$(sles10_archive_DIR)/Makefile : m4.defs $(sles10_archive_DIR)/Makefile.in
	$(M4) $^ >$@



