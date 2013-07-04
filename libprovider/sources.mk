libprovider_PURPOSE = target
libprovider_SOURCES = SF_AffectedJobElement_Provider.cpp \
	SF_AvailableDiagnosticService_Provider.cpp \
	SF_BundleComponent_Provider.cpp \
	SF_ComputerSystem_Provider.cpp \
	SF_ConcreteJob_Provider.cpp \
	SF_ConnectorOnNIC_Provider.cpp \
	SF_ConnectorRealizesPort_Provider.cpp \
	SF_CardRealizesController_Provider.cpp \
	SF_ControlledBy_Provider.cpp \
	SF_DiagnosticCompletionRecord_Provider.cpp \
	SF_DiagnosticLog_Provider.cpp \
	SF_DiagnosticLogCapabilities_Provider.cpp \
	SF_DiagnosticServiceCapabilities_Provider.cpp \
	SF_DiagnosticSettingData_Provider.cpp \
	SF_DiagnosticTest_Provider.cpp \
	SF_ElementCapabilities_Provider.cpp \
	SF_ElementConformsToProfile_Provider.cpp \
	SF_ElementSettingData_Provider.cpp \
	SF_ElementSoftwareIdentity_Provider.cpp \
	SF_DiagElementSoftwareIdentity_Provider.cpp \
	SF_EnabledLogicalElementCapabilities_Provider.cpp \
	SF_EthernetPort_Provider.cpp \
	SF_HostedAccessPoint_Provider.cpp \
	SF_HostedService_Provider.cpp \
	SF_InstalledSoftwareIdentity_Provider.cpp \
	SF_LANEndpoint_Provider.cpp \
	SF_LogEntry_Provider.cpp \
	SF_LogManagesRecord_Provider.cpp \
	SF_NICCard_Provider.cpp \
	SF_NICSAPImplementation_Provider.cpp \
	SF_OwningJobElement_Provider.cpp \
	SF_PhysicalConnector_Provider.cpp \
	SF_PortController_Provider.cpp \
	SF_RecordAppliesToElement_Provider.cpp \
	SF_RecordLog_Provider.cpp \
	SF_RecordLogCapabilities_Provider.cpp \
	SF_ReferencedProfile_Provider.cpp \
	SF_RegisteredProfile_Provider.cpp \
	SF_ServiceAffectsCard_Provider.cpp \
	SF_ServiceAffectsController_Provider.cpp \
	SF_ServiceAffectsSystem_Provider.cpp \
	SF_ServiceAffectsSoftware_Provider.cpp \
	SF_SoftwareIdentity_Provider.cpp \
	SF_SoftwareInstallationServiceCapabilities_Provider.cpp \
	SF_SoftwareInstallationService_Provider.cpp \
	SF_SystemDevice_Provider.cpp \
	SF_SystemUseOfLog_Provider.cpp \
	SF_DiagnosticUseOfLog_Provider.cpp \
	SF_ComputerSystemPackage_Provider.cpp \
	SF_JobStarted_Provider.cpp \
	SF_JobCreated_Provider.cpp \
	SF_JobError_Provider.cpp \
	SF_JobSuccess_Provider.cpp \
	SF_Alert_Provider.cpp \
	sf_core.cpp \
	sf_logging.cpp \
	sf_platform.cpp \
	sf_$(PROVIDER_PLATFORM).cpp \
	sf_threads.cpp \
	sf_utils.cpp \
	sf_helpers.cpp \
	sf_nic_helpers.cpp \
	sf_port_helpers.cpp \
	sf_intf_helpers.cpp \
	sf_diag_helpers.cpp \
	sf_sw_helpers.cpp \
	sf_sys_helpers.cpp \
	$(libprovider_GENERATED)

libprovider_GENERATED = module.cpp

ifeq ($(CIM_INTERFACE),wmi)
libprovider_EXTRA_CLEAN = rm $(libprovider_DIR)/guid.h $(libprovider_DIR)/register.mof $(libprovider_DIR)/unregister.mof
endif

libprovider_DIR = libprovider
ifeq ($(CIM_INTERFACE),wmi)
libprovider_TARGET = $(PROVIDER_LIBRARY).dll
else
libprovider_TARGET = lib$(PROVIDER_LIBRARY).so
endif

libprovider_INCLUDES = $(libprovider_DIR)
libprovider_CPPFLAGS = -Ilibprovider -Ilibprovider/v5_import -DTARGET_CIM_SERVER_$(CIM_SERVER) -DCIM_SCHEMA_VERSION_MINOR=$(CIM_SCHEMA_VERSION_MINOR)

ifeq ($(CIM_SERVER),esxi)
NEED_ASSOC_IN_ROOT_CIMV2=1
libprovider_CPPFLAGS += -DNEED_ASSOC_IN_ROOT_CIMV2=$(NEED_ASSOC_IN_ROOT_CIMV2)

CI_INCLUDES = libprovider/v5_import libprovider/v5_import/ci libprovider/v5_import/ci/app \
	      libprovider/v5_import/ci/app/platform \
		libprovider/v5_import/ci/compat libprovider/v5_import/ci/driver \
		libprovider/v5_import/ci/driver/efab \
		libprovider/v5_import/ci/driver/efab/hardware \
		libprovider/v5_import/ci/driver/efab/hardware/falcon \
		libprovider/v5_import/ci/driver/platform \
		libprovider/v5_import/ci/driver/resource \
		libprovider/v5_import/ci/efch libprovider/v5_import/ci/efhw \
		libprovider/v5_import/ci/efrm libprovider/v5_import/ci/eftest \
		libprovider/v5_import/ci/internal libprovider/v5_import/ci/iscsi \
		libprovider/v5_import/ci/mgmt libprovider/v5_import/ci/net \
		libprovider/v5_import/ci/tools \
		libprovider/v5_import/ci/tools/platform libprovider/curl/

libprovider_INCLUDES += $(CI_INCLUDES)
endif

$(libprovider_DIR)/module.cpp : $(libcimobjects_DIR)/classes $(libcimobjects_DIR)/repository.mof $(CIM_SCHEMA_ROOTFILE) $(genmod_TARGET)
	cd $(dir $@); CIMPLE_MOF_PATH="$(CIM_SCHEMA_DIR)" $(abspath $(genmod_TARGET)) $(PROVIDER_LIBRARY) \
									-F$(abspath $<) -M$(abspath $(libcimobjects_DIR)/repository.mof) \
									$(if $(findstring wmi,$(CIM_INTERFACE)),-w)

ifeq ($(CIM_INTERFACE),cmpi)
CIM_INTERFACE_UPCASE=CMPI
endif
ifeq ($(CIM_INTERFACE),pegasus)
CIM_INTERFACE_UPCASE=PEGASUS
endif
ifeq ($(CIM_INTERFACE),wmi)
CIM_INTERFACE_UPCASE=WMI
endif

$(libprovider_DIR)/module.o $(libprovider_DIR)/module.d : CPPFLAGS += -DCIMPLE_$(CIM_INTERFACE_UPCASE)_MODULE

libprovider_DEPENDS = libcimobjects
libprovider_BUILD_DEPENDS = genmod

$(eval $(call component,libprovider,SHARED_LIBRARIES))

ifneq ($(CIM_SERVER),pegasus)

repository.reg : $(libcimobjects_DIR)/repository.mof.cpp $(libcimobjects_DIR)/classes mof2reg.awk
	$(AWK) -f mof2reg.awk -vPRODUCTNAME=$(PROVIDER_LIBRARY) -vNAMESPACE=$(IMP_NAMESPACE) \
                -vINTEROP_NAMESPACE=$(INTEROP_NAMESPACE) \
                -vROOT_NAMESPACE="$(if $(NEED_ASSOC_IN_ROOT_CIMV2),root/cimv2)" \
                -vCLASSLIST="`cat $(libcimobjects_DIR)/classes`" $< >$@
endif

ifeq ($(CIM_SERVER),pegasus)

register: install $(regmod_TARGET) $(PEGASUS_START_CONF)
	$(RUNASROOT) $(abspath $(regmod_TARGET)) -n $(IMP_NAMESPACE) -c $(PROVIDER_LIBRARY_SO)
	$(RUNASROOT) $(abspath $(regmod_TARGET)) -n $(INTEROP_NAMESPACE) -c $(PROVIDER_LIBRARY_SO) $(INTEROP_CLASSES)

unregister: $(regmod_TARGET) $(PEGASUS_START_CONF)
	$(RUNASROOT) $(abspath $(regmod_TARGET)) -n $(INTEROP_NAMESPACE) -u -c -i $(PROVIDER_LIBRARY_SO) $(INTEROP_CLASSES)
	$(RUNASROOT) $(abspath $(regmod_TARGET)) -n $(IMP_NAMESPACE) -u -c -i $(PROVIDER_LIBRARY_SO)
endif

ifeq ($(CIM_SERVER),sfcb)

register: repository.reg install
	$(RUNASROOT) $(SFCBSTAGE) -n $(IMP_NAMESPACE) -r repository.reg repository.mof
	$(RUNASROOT) $(SFCBSTAGE) -n $(IMP_NAMESPACE) -r repository.reg interop.mof
	$(RUNASROOT) $(SFCBREPOS)

endif

ifeq ($(CIM_SERVER),wmi)

$(libprovider_TARGET) : $(libprovider_DIR)/resource.o

$(libprovider_DIR)/resource.o : WINDRES_CPPFLAGS = -DPROVIDER_LIBRARY=\\\"$(PROVIDER_LIBRARY)\\0\\\" \
												   -DPROVIDER_VERSION=\\\"$(PROVIDER_VERSION).$(PROVIDER_REVISION)\\0\\\" \
												   -DPROVIDER_VERSION_MAJOR=$(PROVIDER_VERSION_MAJOR) \
												   -DPROVIDER_VERSION_MINOR=$(PROVIDER_VERSION_MINOR) \
												   -DPROVIDER_REVISION_NO=0 -DPROVIDER_BUILD_NO=0

$(libprovider_DIR)/unregister.mof : $(libcimobjects_DIR)/repository.mof $(MAKEFILE_LIST)
	tac $< | $(AWK) '$$1 == "class" { print "#pragma deleteclass(\"" $$2 "\", fail)" }' >$@


Install$(PROVIDER_LIBRARY).exe : $(PROVIDER_LIBRARY).nsi $(libprovider_TARGET) \
								 $(libcimobjects_DIR)/schema.mof $(libprovider_DIR)/unregister.mof
	makensis -DPROVIDERNAME=$(PROVIDER_LIBRARY) -DINSTALLERNAME=$@ -DNAMESPACE='\\.\root\cimv2' $<

endif
