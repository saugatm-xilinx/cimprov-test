sles10_archive_TARGET = sles10-solarflare.tar.gz
sles10_archive_DIR = sles10

sles10_archive_COMPONENTS = $(foreach comp,$(COMPONENTS),$(if $(findstring target,$($(comp)_PURPOSE)),$(comp) ,))
sles10_archive_INCLUDES = $(foreach comp,$(sles10_archive_COMPONENTS),$($(comp)_INCLUDES) )
sles10_archive_CPPFLAGS = $(foreach comp,$(sles10_archive_COMPONENTS),$($(comp)_CPPFLAGS) )
sles10_archive_SOURCES = $(foreach comp,$(sles10_archive_COMPONENTS),$(_$(comp)_SOURCES) $(_$(comp)_HEADERS) )
sles10_archive_SOURCES += $(libcimobjects_DIR)/repository.mof $(libcimobjects_DIR)/interop.mof $(libcimobjects_DIR)/root.mof
sles10_archive_SOURCES += Makefile.sles10

$(warning Components: $(sles10_archive_COMPONENTS))

_sles10_archive_SOURCES = $(addprefix $(sles10_archive_DIR)/,$(sles10_archive_SOURCES))

ifeq ($(MAKECMDGOALS),clean)
_sles10_archive_SOURCES += $(foreach comp,$(sles10_archive_COMPONENTS), \
			$(wildcard $(patsubst %,$(sles10_archive_DIR)/%/*.h,$($(comp)_INCLUDES))))
endif

_sles10_archive_GENERATED = $(_sles10_archive_SOURCES)

$(sles10_archive_TARGET) : $(_sles10_archive_SOURCES)
	cd $(sles10_archive_DIR); tar -czf ../$@ *

$(sles10_archive_DIR)/$(libcimobjects_DIR)/repository.mof : $(libcimobjects_DIR)/repository.mof \
	$(addprefix $(CIM_SCHEMA_PATCHDIR)/,$(CIM_SCHEMA_ADDON_MOFS))
	mkdir -p $(dir $@)
	cat $^ >$@

$(sles10_archive_DIR)/% : %
	mkdir -p $(dir $@)
	cp $< $@

$(sles10_archive_DIR)/Makefile.sles10 : $(MAKEFILE_LIST)
	echo "PEGASUS_ROOT=$(PEGASUS_ROOT)" >$@
	echo "PEGASUS_HOME=$(PEGASUS_HOME)" >>$@
	echo "PEGASUS_PLATFORM=$(CIMPLE_PLATFORM)" >>$@
	echo "PROVIDER_LIBPATH?=$(PEGASUS_HOME)/lib" >>$@
	echo "TARGET=lib$(PROVIDER_LIBRARY).so" >>$@
	echo "SOURCES=$(sles10_archive_SOURCES)" >>$@
	echo "INCLUDES=$(sles10_archive_INCLUDES) . \$$(PEGASUS_ROOT)/src " >>$@
	echo "CPPFLAGS=$(sles10_archive_CPPFLAGS)" >>$@
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
	echo "regmod_INCLUDES=$(regmod_INCLUDES) $(libtools_INCLUDES) \$$(PEGASUS_ROOT)/src" >>$@
	echo "regmod_LIBDIR=\$$(PEGASUS_HOME)/lib" >>$@
	echo "regmod_LIBRARIES=$(pegasus_PROVIDE_LIBRARIES)" >>$@
	echo "regmod_SOURCES=\$$(SOURCES)" >>$@
	echo "regmod_CPPFLAGS=\$$(addprefix -I,\$$(regmod_INCLUDES))" >>$@
	echo "regmod: \$$(patsubst %.cpp,%.o,\$$(regmod_SOURCES))" >>$@
	echo "regmod: %:" >>$@
	echo "		 \$$(CXX) -o \$$@ \$$(regmod_CPPFLAGS) \$$(filter %.o,\$$^) \\" >>$@
	echo "		 \$$(addprefix -l,\$$(regmod_LIBRARIES)) \\" >>$@
	echo "		-L\$$(regmod_LIBDIR) -Wl,-rpath=\$$(regmod_LIBDIR)" >>$@
	echo "install:" >>$@
	echo "		mkdir -p \$$(DESTDIR)\$$(PROVIDER_LIBPATH)" >>$@
	echo "		cp lib$(PROVIDER_LIBRARY).so \$$(DESTDIR)\$$(PROVIDER_LIBPATH)" >>$@
	echo "register: install regmod" >>$@
	echo "		$(RUNASROOT) ./regmod -n$(IMP_NAMESPACE) -c \$$(DESTDIR)\$$(PROVIDER_LIBPATH)/lib$(PROVIDER_LIBRARY).so" >>$@
	echo "		$(RUNASROOT) ./regmod -n$(INTEROP_NAMESPACE) -c \$$(DESTDIR)\$$(PROVIDER_LIBPATH)/lib$(PROVIDER_LIBRARY).so $(INTEROP_CLASSES)" >>$@
	echo "unregister: regmod" >>$@
	echo "		$(RUNASROOT) ./regmod -n$(INTEROP_NAMESPACE) -u -c -i \$$(DESTDIR)\$$(PROVIDER_LIBPATH)/lib$(PROVIDER_LIBRARY).so $(INTEROP_CLASSES)" >>$@
	echo "		$(RUNASROOT) ./regmod -n$(IMP_NAMESPACE) -u -c -i \$$(DESTDIR)\$$(PROVIDER_LIBPATH)/lib$(PROVIDER_LIBRARY).so" >>$@






COMPONENTS += sles10_archive
