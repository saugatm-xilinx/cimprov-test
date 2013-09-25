sles10_archive_TARGET = $(PROVIDER_TARBALL_DIR).tar.gz
sles10_archive_DIR = sles10

sles10_archive_EXTRA_DISTFILES += $(libcimobjects_DIR)/repository.mof \
								  $(libcimobjects_DIR)/interop.mof $(libcimobjects_DIR)/schema.mof \
				                  repository.reg \
								  $(CIM_SCHEMA_DIR)

sles10_archive_GENERATED += Makefile
sles10_archive_GENERATED += lib$(PROVIDER_LIBRARY).spec
sles10_archive_GENERATED += $(libcimobjects_DIR)/namespace.mof
sles10_archive_SOURCES = $(sles10_archive_GENERATED)

$(sles10_archive_TARGET) : TAR_TRANSFORM = !^$(sles10_archive_DIR)/!! !^!$*/!

$(eval $(call archive_component,sles10_archive))

$(sles10_archive_DIR)/$(libcimobjects_DIR)/namespace.mof : $(libcimobjects_DIR)/namespace.mof \
	$(addprefix $(CIM_SCHEMA_PATCHDIR)/,$(CIM_SCHEMA_ADDON_MOFS))
	mkdir -p $(dir $@)
	cat $^ >$@

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
	echo "INCLUDES=$(sles10_libprov_INCLUDES) . \$$(PEGASUS_ROOT)/src " >>$@
	echo "CPPFLAGS=$(sles10_libprov_CPPFLAGS)" >>$@
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
	echo "install:" >>$@
	echo "		strip lib$(PROVIDER_LIBRARY).so" >>$@
	echo "		mkdir -p \$$(DESTDIR)\$$(PROVIDER_LIBPATH)" >>$@
	echo "		cp lib$(PROVIDER_LIBRARY).so \$$(DESTDIR)\$$(PROVIDER_LIBPATH)" >>$@
	echo "register: install" >>$@
	echo "	$(RUNASROOT) $(PEGASUS_BINPATH)/cimmof -uc -aE -n $(IMP_NAMESPACE) $(libcimobjects_DIR)/schema.mof" >>$@
	echo "	$(RUNASROOT) $(PEGASUS_BINPATH)/cimmof -uc -n $(IMP_NAMESPACE) $(libcimobjects_DIR)/namespace.mof" >>$@
	echo "	$(RUNASROOT) $(PEGASUS_BINPATH)/cimmof -uc -n $(INTEROP_NAMESPACE) $(libcimobjects_DIR)/interop.mof" >>$@
	echo "	$(RUNASROOT) $(PEGASUS_BINPATH)/cimmof -n $(INTEROP_NAMESPACE) repository.reg" >>$@
	echo "" >>$@
	echo "unregister: " >>$@
	echo "	$(RUNASROOT) $(PEGASUS_BINPATH)/cimprovider -r -m $(PROVIDER_LIBRARY)_Module" >>$@
	echo "ifneq (\$$(PROVIDER_ROOT),)" >>$@
	echo "install-aux : " >>$@
	echo "		mkdir -p \$$(DESTDIR)\$$(PROVIDER_ROOT)/bin" >>$@
	echo "		mkdir -p \$$(DESTDIR)\$$(PROVIDER_ROOT)/mof" >>$@
	echo "		cp libcimobjects/namespace.mof \$$(DESTDIR)\$$(PROVIDER_ROOT)/mof" >>$@
	echo "		cp libcimobjects/interop.mof \$$(DESTDIR)\$$(PROVIDER_ROOT)/mof" >>$@
	echo "		cp libcimobjects/schema.mof \$$(DESTDIR)\$$(PROVIDER_ROOT)/mof" >>$@
	echo "		cp repository.reg \$$(DESTDIR)\$$(PROVIDER_ROOT)/mof" >>$@
	echo "endif" >>$@


