PROVIDER_PACKAGE = solarflare_provider_ibm
PROVIDER_TARBALL_DIR = $(PROVIDER_PACKAGE)-$(PROVIDER_VERSION).$(PROVIDER_REVISION)

sles10_archive_TARGET = $(PROVIDER_TARBALL_DIR).tar.gz
sles10_archive_DIR = sles10

sles10_libprov_COMPONENTS = libcimple libcimplepeg libcimobjects libprovider
sles10_regmod_COMPONENTS = regmod libcimple $(PEGASUS_TOOLS_DEPS)
sles10_archive_COMPONENTS = $(sles10_libprov_COMPONENTS) $(sles10_regmod_COMPONENTS)

sles10_libprov_INCLUDES = $(foreach comp,$(sles10_libprov_COMPONENTS),$($(comp)_INCLUDES) )
sles10_libprov_CPPFLAGS = $(foreach comp,$(sles10_libprov_COMPONENTS),$($(comp)_CPPFLAGS) )
sles10_libprov_SOURCES = $(foreach comp,$(sles10_libprov_COMPONENTS),$(_$(comp)_SOURCES) $(_$(comp)_HEADERS) )

sles10_regmod_SOURCES = $(foreach comp,$(sles10_regmod_COMPONENTS),$(_$(comp)_SOURCES) $(_$(comp)_HEADERS) )
sles10_regmod_INCLUDES = $(foreach comp,$(sles10_regmod_COMPONENTS),$($(comp)_INCLUDES) )
sles10_regmod_CPPFLAGS = $(foreach comp,$(sles10_regmod_COMPONENTS),$($(comp)_CPPFLAGS) )

sles10_archive_SOURCES = $(foreach comp,$(sles10_archive_COMPONENTS),$(_$(comp)_SOURCES) $(_$(comp)_HEADERS) )
sles10_archive_SOURCES += $(libcimobjects_DIR)/repository.mof $(libcimobjects_DIR)/interop.mof $(libcimobjects_DIR)/root.mof
sles10_archive_SOURCES += Makefile
sles10_archive_SOURCES += lib$(PROVIDER_LIBRARY).spec

$(warning Components: $(sles10_archive_COMPONENTS))

_sles10_archive_SOURCES = $(addprefix $(sles10_archive_DIR)/,$(sles10_archive_SOURCES))

ifeq ($(MAKECMDGOALS),clean)
_sles10_archive_SOURCES += $(foreach comp,$(sles10_archive_COMPONENTS), \
			$(wildcard $(patsubst %,$(sles10_archive_DIR)/%/*.h,$($(comp)_INCLUDES))))
endif

_sles10_archive_GENERATED = $(_sles10_archive_SOURCES)

$(sles10_archive_TARGET) : $(_sles10_archive_SOURCES)
	mkdir -p $(PROVIDER_TARBALL_DIR)
	cp -r $(sles10_archive_DIR)/* $(PROVIDER_TARBALL_DIR)
	tar -czf $@ $(PROVIDER_TARBALL_DIR)

$(sles10_archive_DIR)/$(libcimobjects_DIR)/repository.mof : $(libcimobjects_DIR)/repository.mof \
	$(addprefix $(CIM_SCHEMA_PATCHDIR)/,$(CIM_SCHEMA_ADDON_MOFS))
	mkdir -p $(dir $@)
	cat $^ >$@

$(sles10_archive_DIR)/% : %
	mkdir -p $(dir $@)
	cp $< $@

define subst_spec
$(SED) 's!%{PROVIDER_LIBRARY}!$(PROVIDER_LIBRARY)!g; \
                s!%{PROVIDER_VERSION}!$(PROVIDER_VERSION)!g; \
		s!%{PROVIDER_REVISION}!$(PROVIDER_REVISION)!g; \
                s!%{PROVIDER_LIBPATH}!$(PROVIDER_LIBPATH)!g; \
		s!%{PROVIDER_PACKAGE}!$(PROVIDER_PACKAGE)!g; \
                s!%{PROVIDER_SO}!$(libprovider_TARGET)!g; \
                s!%{PROVIDER_ROOT}!$(PROVIDER_ROOT)!g; \
                s!%{IMP_NAMESPACE}!$(IMP_NAMESPACE)!g; \
                s!%{INTEROP_NAMESPACE}!$(INTEROP_NAMESPACE)!g; \
                s!%{INTEROP_CLASSES}!$(INTEROP_CLASSES)!g; \
                s!%{PEGASUS_HOME}!$(PEGASUS_HOME)!g' $< >$@
endef

$(sles10_archive_DIR)/lib$(PROVIDER_LIBRARY).spec : $(sles10_archive_DIR)/lib$(PROVIDER_LIBRARY).spec.in $(MAKEFILE_LIST)
	$(subst_spec)



$(sles10_archive_DIR)/Makefile : $(MAKEFILE_LIST)
	echo "PEGASUS_ROOT=$(PEGASUS_ROOT)" >$@
	echo "PEGASUS_HOME=$(PEGASUS_HOME)" >>$@
	echo "PROVIDER_ROOT=$(PROVIDER_ROOT)" >>$@
	echo "PEGASUS_PLATFORM=$(CIMPLE_PLATFORM)" >>$@
	echo "PROVIDER_LIBPATH?=$(PEGASUS_HOME)/lib" >>$@
	echo "TARGET=lib$(PROVIDER_LIBRARY).so" >>$@
	echo "SOURCES=$(sles10_libprov_SOURCES)" >>$@
	echo "INCLUDES=$(sles10_libprov_INCLUDES) $(sles10_regmod_INCLUDES) . \$$(PEGASUS_ROOT)/src " >>$@
	echo "CPPFLAGS=$(sles10_libprov_CPPFLAGS) $(sles10_regmod_CPPFLAGS)" >>$@
	echo "CPPFLAGS += -I. -I\$$(PEGASUS_ROOT)/src" >>$@
	echo "CPPFLAGS += -DPEGASUS_PLATFORM_\$$(PEGASUS_PLATFORM)" >>$@
	echo "CPPFLAGS += -DSF_IMPLEMENTATION_NS=\\\"$(IMP_NAMESPACE)\\\"" >>$@
	echo "CPPFLAGS += -DSF_INTEROP_NS=\\\"$(INTEROP_NAMESPACE)\\\"" >>$@
	echo "CPPFLAGS += -DCIMPLE_PEGASUS_MODULE" >>$@
	echo "CPPFLAGS += -DSLES10_BUILD_HOST" >>$@
	echo "CXXFLAGS = $(host_CXXFLAGS)" >>$@
	echo "SYSLIBRARIES = $(host_SYSLIBRARIES)" >>$@
	echo "lib$(PROVIDER_LIBRARY).so : \$$(patsubst %.cpp,%.o,\$$(SOURCES))" >>$@
	echo "lib$(PROVIDER_LIBRARY).so : %.so:" >>$@
	echo "		\$$(CXX) -shared -o \$$@ \$$(LDFLAGS) \$$(CXXFLAGS) \$$(filter %.o,\$$^) \\" >>$@
	echo "		\$$(addprefix -l,\$$(SYSLIBRARIES))" >>$@
	echo "regmod_INCLUDES=$(sles10_regmod_INCLUDES) \$$(PEGASUS_ROOT)/src" >>$@
	echo "regmod_LIBDIR=\$$(PEGASUS_HOME)/lib" >>$@
	echo "regmod_LIBRARIES=$(pegasus_PROVIDE_LIBRARIES)" >>$@
	echo "regmod_SOURCES=$(sles10_regmod_SOURCES)" >>$@
	echo "regmod_CPPFLAGS=\$$(addprefix -I,\$$(regmod_INCLUDES))" >>$@
	echo "regmod: \$$(patsubst %.cpp,%.o,\$$(regmod_SOURCES))" >>$@
	echo "regmod: %:" >>$@
	echo "		 \$$(CXX) -o \$$@ \$$(regmod_CPPFLAGS) \$$(filter %.o,\$$^) \\" >>$@
	echo "		 \$$(addprefix -l,\$$(regmod_LIBRARIES)) \\" >>$@
	echo "		-L\$$(regmod_LIBDIR) -Wl,-rpath=\$$(regmod_LIBDIR)" >>$@
	echo "install:" >>$@
	echo "		strip lib$(PROVIDER_LIBRARY).so" >>$@
	echo "		strip regmod" >>$@
	echo "		mkdir -p \$$(DESTDIR)\$$(PROVIDER_LIBPATH)" >>$@
	echo "		cp lib$(PROVIDER_LIBRARY).so \$$(DESTDIR)\$$(PROVIDER_LIBPATH)" >>$@
	echo "register: install regmod" >>$@
	echo "		$(RUNASROOT) ./regmod -n$(IMP_NAMESPACE) -c \$$(DESTDIR)\$$(PROVIDER_LIBPATH)/lib$(PROVIDER_LIBRARY).so" >>$@
	echo "		$(RUNASROOT) ./regmod -n$(INTEROP_NAMESPACE) -c \$$(DESTDIR)\$$(PROVIDER_LIBPATH)/lib$(PROVIDER_LIBRARY).so $(INTEROP_CLASSES)" >>$@
	echo "unregister: regmod" >>$@
	echo "		$(RUNASROOT) ./regmod -n$(INTEROP_NAMESPACE) -u -c -i \$$(DESTDIR)\$$(PROVIDER_LIBPATH)/lib$(PROVIDER_LIBRARY).so $(INTEROP_CLASSES)" >>$@
	echo "		$(RUNASROOT) ./regmod -n$(IMP_NAMESPACE) -u -c -i \$$(DESTDIR)\$$(PROVIDER_LIBPATH)/lib$(PROVIDER_LIBRARY).so" >>$@
	echo "ifneq (\$$(PROVIDER_ROOT),)" >>$@
	echo "install-aux : regmod" >>$@
	echo "		mkdir -p \$$(DESTDIR)\$$(PROVIDER_ROOT)/bin" >>$@
	echo "		cp regmod \$$(DESTDIR)\$$(PROVIDER_ROOT)/bin" >>$@
	echo "		mkdir -p \$$(DESTDIR)\$$(PROVIDER_ROOT)/mof" >>$@
	echo "		cp libcimobjects/repository.mof \$$(DESTDIR)\$$(PROVIDER_ROOT)/mof" >>$@
	echo "endif" >>$@

COMPONENTS += sles10_archive
