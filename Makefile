##
## Makefile generated by genmak version 2.0.24
##

TOP=/home/artem/src/pegasus/setup/share/cimple
ROOT=.
BINDIR=.
LIBDIR=.
include $(TOP)/mak/config.mak

MODULE=1
SHARED_LIBRARY=Solarflare

##
## Define source files for compile and link
##

SOURCES += CIM_Capabilities.cpp
SOURCES += CIM_Card.cpp
SOURCES += CIM_Collection.cpp
SOURCES += CIM_Component.cpp
SOURCES += CIM_ComputerSystem.cpp
SOURCES += CIM_ConcreteJob.cpp
SOURCES += CIM_ConnectorOnPackage.cpp
SOURCES += CIM_Container.cpp
SOURCES += CIM_ControlledBy.cpp
SOURCES += CIM_Controller.cpp
SOURCES += CIM_Dependency.cpp
SOURCES += CIM_DeviceConnection.cpp
SOURCES += CIM_DeviceSAPImplementation.cpp
SOURCES += CIM_DiagnosticLog.cpp
SOURCES += CIM_ElementCapabilities.cpp
SOURCES += CIM_ElementSoftwareIdentity.cpp
SOURCES += CIM_EnabledLogicalElement.cpp
SOURCES += CIM_EnabledLogicalElementCapabilities.cpp
SOURCES += CIM_Error.cpp
SOURCES += CIM_EthernetPort.cpp
SOURCES += CIM_HostedAccessPoint.cpp
SOURCES += CIM_HostedDependency.cpp
SOURCES += CIM_HostedService.cpp
SOURCES += CIM_InstalledSoftwareIdentity.cpp
SOURCES += CIM_Job.cpp
SOURCES += CIM_LANEndpoint.cpp
SOURCES += CIM_Log.cpp
SOURCES += CIM_LogEntry.cpp
SOURCES += CIM_LogManagesRecord.cpp
SOURCES += CIM_LogicalDevice.cpp
SOURCES += CIM_LogicalElement.cpp
SOURCES += CIM_LogicalPort.cpp
SOURCES += CIM_ManagedElement.cpp
SOURCES += CIM_ManagedSystemElement.cpp
SOURCES += CIM_NetworkPort.cpp
SOURCES += CIM_OperatingSystem.cpp
SOURCES += CIM_OrderedComponent.cpp
SOURCES += CIM_PhysicalConnector.cpp
SOURCES += CIM_PhysicalElement.cpp
SOURCES += CIM_PhysicalPackage.cpp
SOURCES += CIM_PortController.cpp
SOURCES += CIM_ProtocolEndpoint.cpp
SOURCES += CIM_Realizes.cpp
SOURCES += CIM_RecordForLog.cpp
SOURCES += CIM_RecordLog.cpp
SOURCES += CIM_Service.cpp
SOURCES += CIM_ServiceAccessPoint.cpp
SOURCES += CIM_ServiceAffectsElement.cpp
SOURCES += CIM_SoftwareIdentity.cpp
SOURCES += CIM_SoftwareInstallationService.cpp
SOURCES += CIM_SoftwareInstallationServiceCapabilities.cpp
SOURCES += CIM_System.cpp
SOURCES += CIM_SystemComponent.cpp
SOURCES += CIM_SystemDevice.cpp
SOURCES += CIM_UseOfLog.cpp
SOURCES += SF_BundleComponent.cpp
SOURCES += SF_BundleComponent_Provider.cpp
SOURCES += SF_ComputerSystem.cpp
SOURCES += SF_ComputerSystem_Provider.cpp
SOURCES += SF_ConcreteJob.cpp
SOURCES += SF_ConcreteJob_Provider.cpp
SOURCES += SF_ConnectorOnNIC.cpp
SOURCES += SF_ConnectorOnNIC_Provider.cpp
SOURCES += SF_ConnectorRealizesPort.cpp
SOURCES += SF_ConnectorRealizesPort_Provider.cpp
SOURCES += SF_ControlledBy.cpp
SOURCES += SF_ControlledBy_Provider.cpp
SOURCES += SF_DiagnosticLog.cpp
SOURCES += SF_DiagnosticLog_Provider.cpp
SOURCES += SF_ElementCapabilities.cpp
SOURCES += SF_ElementCapabilities_Provider.cpp
SOURCES += SF_ElementSoftwareIdentity.cpp
SOURCES += SF_ElementSoftwareIdentity_Provider.cpp
SOURCES += SF_EnabledLogicalElementCapabilities.cpp
SOURCES += SF_EnabledLogicalElementCapabilities_Provider.cpp
SOURCES += SF_EthernetPort.cpp
SOURCES += SF_EthernetPort_Provider.cpp
SOURCES += SF_HostedAccessPoint.cpp
SOURCES += SF_HostedAccessPoint_Provider.cpp
SOURCES += SF_HostedService.cpp
SOURCES += SF_HostedService_Provider.cpp
SOURCES += SF_InstalledSoftwareIdentity.cpp
SOURCES += SF_InstalledSoftwareIdentity_Provider.cpp
SOURCES += SF_LANEndpoint.cpp
SOURCES += SF_LANEndpoint_Provider.cpp
SOURCES += SF_LogEntry.cpp
SOURCES += SF_LogEntry_Provider.cpp
SOURCES += SF_LogManagesRecord.cpp
SOURCES += SF_LogManagesRecord_Provider.cpp
SOURCES += SF_NICCard.cpp
SOURCES += SF_NICCard_Provider.cpp
SOURCES += SF_NICSAPImplementation.cpp
SOURCES += SF_NICSAPImplementation_Provider.cpp
SOURCES += SF_PhysicalConnector.cpp
SOURCES += SF_PhysicalConnector_Provider.cpp
SOURCES += SF_PortController.cpp
SOURCES += SF_PortController_Provider.cpp
SOURCES += SF_RecordLog.cpp
SOURCES += SF_RecordLog_Provider.cpp
SOURCES += SF_ServiceAffectsElement.cpp
SOURCES += SF_ServiceAffectsElement_Provider.cpp
SOURCES += SF_SoftwareIdentity.cpp
SOURCES += SF_SoftwareIdentity_Provider.cpp
SOURCES += SF_SoftwareInstallationService.cpp
SOURCES += SF_SoftwareInstallationServiceCapabilities.cpp
SOURCES += SF_SoftwareInstallationServiceCapabilities_Provider.cpp
SOURCES += SF_SoftwareInstallationService_Provider.cpp
SOURCES += SF_SystemDevice.cpp
SOURCES += SF_SystemDevice_Provider.cpp
SOURCES += SF_UseOfLog.cpp
SOURCES += SF_UseOfLog_Provider.cpp
SOURCES += module.cpp
SOURCES += repository.cpp
SOURCES += sf_core.cpp
SOURCES += sf_logging.cpp
SOURCES += sf_platform.cpp
SOURCES += sf_sample.cpp
SOURCES += sf_threads.cpp
SOURCES += sf_utils.cpp

##
## Module defined as Pegasus C++ interface
##
CIMPLE_PEGASUS_MODULE=1
DEFINES += -DCIMPLE_PEGASUS_MODULE

LIBRARIES += cimplepegadap
LIBRARIES += cimple

include $(TOP)/mak/rules.mak
