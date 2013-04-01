libprovider_SOURCES = SF_AffectedJobElement_Provider.cpp \
	SF_AvailableDiagnosticService_Provider.cpp \
	SF_BundleComponent_Provider.cpp \
	SF_ComputerSystem_Provider.cpp \
	SF_ConcreteJob_Provider.cpp \
	SF_ConnectorOnNIC_Provider.cpp \
	SF_ConnectorRealizesPort_Provider.cpp \
	SF_ControlledBy_Provider.cpp \
	SF_DiagnosticCompletionRecord_Provider.cpp \
	SF_DiagnosticLog_Provider.cpp \
	SF_DiagnosticServiceCapabilities_Provider.cpp \
	SF_DiagnosticSettingData_Provider.cpp \
	SF_DiagnosticTest_Provider.cpp \
	SF_ElementCapabilities_Provider.cpp \
	SF_ElementConformsToProfile_Provider.cpp \
	SF_ElementSettingData_Provider.cpp \
	SF_ElementSoftwareIdentity_Provider.cpp \
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
	SF_ReferencedProfile_Provider.cpp \
	SF_RegisteredProfile_Provider.cpp \
	SF_ServiceAffectsElement_Provider.cpp \
	SF_SoftwareIdentity_Provider.cpp \
	SF_SoftwareInstallationServiceCapabilities_Provider.cpp \
	SF_SoftwareInstallationService_Provider.cpp \
	SF_SystemDevice_Provider.cpp \
	SF_UseOfLog_Provider.cpp \
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

libprovider_DIR = libprovider
libprovider_TARGET = lib$(PROVIDER_LIBRARY).so
libprovider_INCLUDES = $(libprovider_DIR)
libprovider_CPPFLAGS = -Ilibprovider -DTARGET_CIM_SERVER_$(CIM_SERVER) -DCIM_SCHEMA_VERSION_MINOR=$(CIM_SCHEMA_VERSION_MINOR)

ifeq ($(CIM_SERVER),esxi)

CI_INCLUDES = libprovider/ci libprovider/ci/app libprovider/ci/app/platform \
		libprovider/ci/compat libprovider/ci/driver \
		libprovider/ci/driver/efab libprovider/ci/driver/efab/hardware \
		libprovider/ci/driver/efab/hardware/falcon \
		libprovider/ci/driver/platform libprovider/ci/driver/resource \
		libprovider/ci/efch libprovider/ci/efhw libprovider/ci/efrm \
		libprovider/ci/eftest libprovider/ci/internal libprovider/ci/iscsi \
		libprovider/ci/mgmt libprovider/ci/net libprovider/ci/tools \
		libprovider/ci/tools/platform libprovider/curl/

libprovider_INCLUDES += $(CI_INCLUDES)
endif

$(libprovider_DIR)/module.cpp : $(libcimobjects_DIR)/classes $(libcimobjects_DIR)/repository.mof $(CIM_SCHEMA_DIR) $(genmod_TARGET)
	cd $(dir $@); CIMPLE_MOF_PATH="$(CIM_SCHEMA_DIR)" $(abspath $(genmod_TARGET)) $(PROVIDER_LIBRARY) -F$(abspath $<) -M$(abspath $(libcimobjects_DIR)/repository.mof)


ifeq ($(CIM_INTERFACE),pegasus)
$(libprovider_DIR)/module.o $(libprovider_DIR)/module.d : CPPFLAGS += -DCIMPLE_PEGASUS_MODULE

endif
ifeq ($(CIM_INTERFACE),cmpi)
$(libprovider_DIR)/module.o $(libprovider_DIR)/module.d : CPPFLAGS += -DCIMPLE_CMPI_MODULE
endif

libprovider_DEPENDS = libcimobjects
libprovider_BUILD_DEPENDS = genmod

$(eval $(call component,libprovider,SHARED_LIBRARIES))

ifneq ($(CIM_SERVER),pegasus)

repository.reg : $(libcimobjects_DIR)/repository.mof mof2reg.awk
	$(AWK) -f mof2reg.awk -vPRODUCTNAME=$(PROVIDER_LIBRARY) -vNAMESPACE=$(IMP_NAMESPACE) $< >$@

interop.reg : $(libcimobjects_DIR)/interop.mof mof2reg.awk
	$(AWK) -f mof2reg.awk -vPRODUCTNAME=$(PROVIDER_LIBRARY) -vNAMESPACE=$(INTEROP_NAMESPACE) $< >$@

endif

ifeq ($(CIM_SERVER),pegasus)

register: install $(regmod_TARGET) $(PEGASUS_START_CONF)
	$(abspath $(regmod_TARGET)) -n $(IMP_NAMESPACE) -c $(PROVIDER_LIBRARY_SO)
	$(abspath $(regmod_TARGET)) -n $(INTEROP_NAMESPACE) -c $(PROVIDER_LIBRARY_SO) $(INTEROP_CLASSES)

unregister: $(regmod_TARGET) $(PEGASUS_START_CONF)
	$(abspath $(regmod_TARGET)) -n $(INTEROP_NAMESPACE) -u -c -i $(PROVIDER_LIBRARY_SO) $(INTEROP_CLASSES)
	$(abspath $(regmod_TARGET)) -n $(IMP_NAMESPACE) -u -c -i $(PROVIDER_LIBRARY_SO)
endif

ifeq ($(CIM_SERVER),sfcb)

register: repository.reg interop.reg install
	$(SFCBSTAGE) -n $(IMP_NAMESPACE) -r repository.reg repository.mof
	$(SFCBSTAGE) -n $(INTEROP_NAMESPACE) -r interop.reg repository.mof
	$(SFCBREPOS)

endif
