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
	module.cpp \
	sf_core.cpp \
	sf_logging.cpp \
	sf_platform.cpp \
	sf_sample.cpp \
	sf_threads.cpp \
	sf_utils.cpp \
	sf_helpers.cpp \
	sf_nic_helpers.cpp \
	sf_port_helpers.cpp \
	sf_intf_helpers.cpp \
	sf_diag_helpers.cpp \
	sf_sw_helpers.cpp \
	sf_sys_helpers.cpp

libprovider_DIR = .
libprovider_TARGET = lib$(PROVIDER_LIBRARY).so
libprovider_INCLUDES = $(libprovider_DIR)

ifeq ($(CIM_SERVER),esxi)

CI_INCLUDES = ci ci/app ci/app/platform ci/compat \
			  ci/driver ci/driver/efab ci/driver/efab/hardware \
			  ci/driver/efab/hardware/falcon \
			  ci/driver/platform ci/driver/resource \
			  ci/efch ci/efhw ci/efrm ci/eftest \
			  ci/internal ci/iscsi ci/mgmt ci/net ci/tools ci/tools/platform

ALL_HEADERS += $(foreach inc,$(CI_INCLUDES),$(wildcard $(inc)/*.h) )

endif


ifeq ($(CIM_INTERFACE),pegasus)
module.o module.d : CPPFLAGS += -DCIMPLE_PEGASUS_MODULE

endif
ifeq ($(CIM_INTERFACE),cmpi)
module.o module.d : CPPFLAGS += -DCIMPLE_CMPI_MODULE
endif

libprovider_DEPENDS = libcimobjects

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
