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

SOURCES += CIM_Card.cpp
SOURCES += CIM_Component.cpp
SOURCES += CIM_ComputerSystem.cpp
SOURCES += CIM_ConcreteJob.cpp
SOURCES += CIM_ConnectorOnPackage.cpp
SOURCES += CIM_Container.cpp
SOURCES += CIM_Controller.cpp
SOURCES += CIM_Dependency.cpp
SOURCES += CIM_EnabledLogicalElement.cpp
SOURCES += CIM_Error.cpp
SOURCES += CIM_EthernetPort.cpp
SOURCES += CIM_InstalledSoftwareIdentity.cpp
SOURCES += CIM_Job.cpp
SOURCES += CIM_LANEndpoint.cpp
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
SOURCES += CIM_ServiceAccessPoint.cpp
SOURCES += CIM_SoftwareIdentity.cpp
SOURCES += CIM_System.cpp
SOURCES += CIM_SystemComponent.cpp
SOURCES += CIM_SystemDevice.cpp
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
SOURCES += SF_EthernetPort.cpp
SOURCES += SF_EthernetPort_Provider.cpp
SOURCES += SF_InstalledSoftwareIdentity.cpp
SOURCES += SF_InstalledSoftwareIdentity_Provider.cpp
SOURCES += SF_LANEndpoint.cpp
SOURCES += SF_LANEndpoint_Provider.cpp
SOURCES += SF_NICCard.cpp
SOURCES += SF_NICCard_Provider.cpp
SOURCES += SF_PhysicalConnector.cpp
SOURCES += SF_PhysicalConnector_Provider.cpp
SOURCES += SF_PortController.cpp
SOURCES += SF_PortController_Provider.cpp
SOURCES += SF_SoftwareIdentity.cpp
SOURCES += SF_SoftwareIdentity_Provider.cpp
SOURCES += SF_SystemDevice.cpp
SOURCES += SF_SystemDevice_Provider.cpp
SOURCES += module.cpp
SOURCES += repository.cpp
SOURCES += sf_core.cpp
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
