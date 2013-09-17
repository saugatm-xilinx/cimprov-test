libprovider_PURPOSE = target
libprovider_SOURCES = SF_AffectedJobElement_Provider.cpp \
	SF_AvailableDiagnosticService_Provider.cpp \
	SF_BundleComponent_Provider.cpp \
	SF_ComputerSystem_Provider.cpp \
	SF_ConcreteJob_Provider.cpp \
	SF_ConnectorOnNIC_Provider.cpp \
	SF_ConnectorRealizesPort_Provider.cpp \
	SF_ConnectorRealizesController_Provider.cpp \
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
	SF_ControllerSoftwareIdentity_Provider.cpp \
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
	SF_ProviderLog_Provider.cpp \
	SF_ProviderLogCapabilities_Provider.cpp \
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
	SF_JobProgress_Provider.cpp \
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
	sf_sensors.cpp \
	sf_alerts.cpp \
	$(libprovider_GENERATED)

libprovider_GENERATED = module.cpp

ifeq ($(PROVIDER_PLATFORM), $(filter $(PROVIDER_PLATFORM),linux vmware))
libprovider_SOURCES += sf_mcdi_sensors.cpp
endif

ifeq ($(PROVIDER_PLATFORM),windows)
libprovider_SOURCES += sf_wmi.cpp
endif
ifeq ($(CIM_INTERFACE),wmi)
libprovider_EXTRA_CLEAN = rm $(libprovider_DIR)/guid.h $(libprovider_DIR)/register.mof $(libprovider_DIR)/unregister.mof $(MSI_NAME)
endif

libprovider_DIR = libprovider
ifeq ($(PROVIDER_PLATFORM),windows)
libprovider_TARGET = $(PROVIDER_LIBRARY).dll
else
libprovider_TARGET = lib$(PROVIDER_LIBRARY).so
endif

libprovider_INCLUDES = $(libprovider_DIR)
libprovider_CPPFLAGS = $(libprovider_PROVIDE_CPPFLAGS) -Ilibprovider/v5_import -DTARGET_CIM_SERVER_$(CIM_SERVER) \
	-DCIM_SCHEMA_VERSION_MINOR=$(CIM_SCHEMA_VERSION_MINOR) \
	-DPROVIDER_VERSION_MAJOR=$(PROVIDER_VERSION_MAJOR) -DPROVIDER_VERSION_MINOR=$(PROVIDER_VERSION_MINOR) \
	-DPROVIDER_REVISION=$(PROVIDER_REVISION)

ifeq ($(CIM_SERVER),esxi)
NEED_ASSOC_IN_ROOT_CIMV2=1
libprovider_CPPFLAGS += -DNEED_ASSOC_IN_ROOT_CIMV2=$(NEED_ASSOC_IN_ROOT_CIMV2)
endif

ifeq ($(PROVIDER_PLATFORM), $(filter $(PROVIDER_PLATFORM),linux vmware))
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
		libprovider/v5_import/ci/tools/platform libprovider/curl

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

repository.reg : $(libcimobjects_DIR)/repository.mof.cpp $(libcimobjects_DIR)/classes $(TOP)/mof2reg.awk
	$(AWK) -f $(TOP)/mof2reg.awk -vPRODUCTNAME=$(PROVIDER_LIBRARY) -vNAMESPACE=$(IMP_NAMESPACE) \
                -vINTEROP_NAMESPACE=$(INTEROP_NAMESPACE) \
                -vROOT_NAMESPACE="$(if $(NEED_ASSOC_IN_ROOT_CIMV2),root/cimv2)" \
				-vINTERFACE="$(CIM_INTERFACE)" \
                -vCLASSLIST="`cat $(word 2,$^)`" -vTARGET="$(CIM_SERVER)" $< >$@

.PHONY : registration 

registration : repository.reg $(libcimobjects_DIR)/namespace.mof $(libcimobjects_DIR)/schema.mof $(libcimobjects_DIR)/interop.mof

ifeq ($(CIM_SERVER),pegasus)

register: repository.reg $(libcimobjects_DIR)/namespace.mof $(libcimobjects_DIR)/schema.mof $(libcimobjects_DIR)/interop.mof install $(PEGASUS_START_CONF)
	$(RUNASROOT) $(PEGASUS_BINPATH)/cimmof -uc -aE -n $(IMP_NAMESPACE) $(libcimobjects_DIR)/schema.mof
	$(RUNASROOT) $(PEGASUS_BINPATH)/cimmof -uc -n $(IMP_NAMESPACE) $(libcimobjects_DIR)/namespace.mof
	$(RUNASROOT) $(PEGASUS_BINPATH)/cimmof -uc -n $(INTEROP_NAMESPACE) $(libcimobjects_DIR)/interop.mof
	$(RUNASROOT) $(PEGASUS_BINPATH)/cimmof -n $(INTEROP_NAMESPACE) repository.reg

unregister: $(regmod_TARGET) $(PEGASUS_START_CONF)
	$(RUNASROOT) $(PEGASUS_BINPATH)/cimprovider -r -m $(PROVIDER_LIBRARY)_Module

endif

ifeq ($(CIM_SERVER),sfcb)

register: repository.reg install
	$(RUNASROOT) $(SFCBSTAGE) -n $(IMP_NAMESPACE) -r repository.reg namespace.mof
	$(RUNASROOT) $(SFCBSTAGE) -n $(IMP_NAMESPACE) -r repository.reg interop.mof
	$(RUNASROOT) $(SFCBREPOS)

endif

ifeq ($(PROVIDER_PLATFORM),windows)

$(libprovider_TARGET) : $(libprovider_DIR)/resource.o

$(libprovider_DIR)/resource.o : WINDRES_CPPFLAGS = -DPROVIDER_LIBRARY=\\\"$(PROVIDER_LIBRARY)\\0\\\" \
												   -DPROVIDER_VERSION=\\\"$(PROVIDER_VERSION).$(PROVIDER_REVISION)\\0\\\" \
												   -DPROVIDER_VERSION_MAJOR=$(PROVIDER_VERSION_MAJOR) \
												   -DPROVIDER_VERSION_MINOR=$(PROVIDER_VERSION_MINOR) \
												   -DPROVIDER_REVISION_NO=0 -DPROVIDER_BUILD_NO=0

ifeq ($(CIM_INTERFACE),wmi)
$(libprovider_DIR)/unregister.mof : $(libcimobjects_DIR)/namespace.mof $(MAKEFILE_LIST)
	tac $< | $(AWK) '$$1 == "class" { print "#pragma deleteclass(\"" $$2 "\", fail)" }' >$@
endif

.PHONY : msi InstallShieldProject

PROVIDER_MSI_PACKAGE=sf_cim_provider
MSI_NAME=$(PROVIDER_MSI_PACKAGE)_$(PROVIDER_VERSION).$(PROVIDER_REVISION)_windows_$(PROVIDER_BITNESS).exe

msi : $(MSI_NAME)

ifeq ($(CIM_INTERFACE),wmi)
NSIS_DEPENDENCIES=$(libcimobjects_DIR)/schema.mof $(libprovider_DIR)/unregister.mof
NSIS_OPTIONS=-DNAMESPACE='\\.\root\cimv2'
else
NSIS_DEPENDENCIES=repository.reg $(libcimobjects_DIR)/namespace.mof $(libcimobjects_DIR)/interop.mof $(libcimobjects_DIR)/schema.mof
NSIS_OPTIONS=-DNAMESPACE='$(IMP_NAMESPACE)' -DINTEROP_NAMESPACE='$(INTEROP_NAMESPACE)'
endif

$(MSI_NAME) : $(PROVIDER_LIBRARY).nsi $(libprovider_TARGET) sf-license.txt $(NSIS_DEPENDENCIES)
	$(target_STRIP) $(libprovider_TARGET)
	makensis -DPROVIDERNAME=$(PROVIDER_LIBRARY) -DCIM_INTERFACE=$(CIM_INTERFACE) -DINSTALLERNAME=$@ $(NSIS_OPTIONS) -DTOP=$(TOP) -NOCD $<

SolarflareCIM.ism.cab : SolarflareCIM.ism $(libprovider_TARGET) \
					$(libcimobjects_DIR)/namespace.mof $(libprovider_DIR)/register.mof \
					$(libcimobjects_DIR)/schema.mof $(libprovider_DIR)/unregister.mof
	$(target_STRIP) $(libprovider_TARGET)
	lcab -n $^ $@

endif
