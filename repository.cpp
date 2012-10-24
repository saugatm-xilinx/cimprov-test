//==============================================================================
//
// PLEASE DO NOT EDIT; THIS FILE WAS AUTOMATICALLY GENERATED BY GENCLASS 2.0.24
//
//==============================================================================

#include <cimple/cimple.h>
#include "repository.h"

CIMPLE_NAMESPACE_BEGIN

/*[2817]*/
static const Meta_Class* _meta_classes[] =
{
    &CIM_ManagedElement::static_meta_class,
    &CIM_ManagedSystemElement::static_meta_class,
    &CIM_LogicalElement::static_meta_class,
    &CIM_SoftwareIdentity::static_meta_class,
    &SF_SoftwareIdentity::static_meta_class,
    &CIM_Job::static_meta_class,
    &CIM_Error::static_meta_class,
    &CIM_ConcreteJob::static_meta_class,
    &SF_ConcreteJob::static_meta_class,
    &CIM_EnabledLogicalElement::static_meta_class,
    &CIM_System::static_meta_class,
    &CIM_InstalledSoftwareIdentity::static_meta_class,
    &SF_InstalledSoftwareIdentity::static_meta_class,
    &CIM_Dependency::static_meta_class,
    &CIM_ElementSoftwareIdentity::static_meta_class,
    &SF_ElementSoftwareIdentity::static_meta_class,
    &CIM_Component::static_meta_class,
    &CIM_OrderedComponent::static_meta_class,
    &SF_BundleComponent::static_meta_class,
    &CIM_ComputerSystem::static_meta_class,
    &SF_ComputerSystem::static_meta_class,
    &CIM_PhysicalElement::static_meta_class,
    &CIM_PhysicalPackage::static_meta_class,
    &CIM_PhysicalConnector::static_meta_class,
    &CIM_Card::static_meta_class,
    &SF_NICCard::static_meta_class,
    &SF_PhysicalConnector::static_meta_class,
    &CIM_Container::static_meta_class,
    &CIM_ConnectorOnPackage::static_meta_class,
    &SF_ConnectorOnNIC::static_meta_class,
    &CIM_LogicalDevice::static_meta_class,
    &CIM_LogicalPort::static_meta_class,
    &CIM_NetworkPort::static_meta_class,
    &CIM_EthernetPort::static_meta_class,
    &SF_EthernetPort::static_meta_class,
    &CIM_Controller::static_meta_class,
    &CIM_PortController::static_meta_class,
    &SF_PortController::static_meta_class,
    &CIM_Capabilities::static_meta_class,
    &CIM_EnabledLogicalElementCapabilities::static_meta_class,
    &SF_EnabledLogicalElementCapabilities::static_meta_class,
    &CIM_SystemComponent::static_meta_class,
    &CIM_SystemDevice::static_meta_class,
    &SF_SystemDevice::static_meta_class,
    &CIM_Realizes::static_meta_class,
    &SF_ConnectorRealizesPort::static_meta_class,
    &CIM_ServiceAccessPoint::static_meta_class,
    &CIM_ProtocolEndpoint::static_meta_class,
    &CIM_LANEndpoint::static_meta_class,
    &SF_LANEndpoint::static_meta_class,
    &CIM_DeviceSAPImplementation::static_meta_class,
    &SF_NICSAPImplementation::static_meta_class,
    &CIM_Service::static_meta_class,
    &CIM_Collection::static_meta_class,
    &CIM_SoftwareInstallationService::static_meta_class,
    &SF_SoftwareInstallationService::static_meta_class,
    &CIM_SoftwareInstallationServiceCapabilities::static_meta_class,
    &SF_SoftwareInstallationServiceCapabilities::static_meta_class,
    &CIM_ElementCapabilities::static_meta_class,
    &SF_ElementCapabilities::static_meta_class,
    &CIM_ServiceAffectsElement::static_meta_class,
    &SF_ServiceAffectsElement::static_meta_class,
    &CIM_HostedDependency::static_meta_class,
    &CIM_HostedService::static_meta_class,
    &SF_HostedService::static_meta_class,
    &CIM_DeviceConnection::static_meta_class,
    &CIM_ControlledBy::static_meta_class,
    &SF_ControlledBy::static_meta_class,
    &CIM_HostedAccessPoint::static_meta_class,
    &SF_HostedAccessPoint::static_meta_class,
    &CIM_Log::static_meta_class,
    &CIM_RecordLog::static_meta_class,
    &SF_RecordLog::static_meta_class,
    &CIM_RecordForLog::static_meta_class,
    &CIM_LogEntry::static_meta_class,
    &SF_LogEntry::static_meta_class,
    &CIM_LogManagesRecord::static_meta_class,
    &SF_LogManagesRecord::static_meta_class,
    &CIM_UseOfLog::static_meta_class,
    &SF_UseOfLog::static_meta_class,
    &CIM_DiagnosticLog::static_meta_class,
    &SF_DiagnosticLog::static_meta_class,
    &CIM_DiagnosticRecord::static_meta_class,
    &CIM_DiagnosticServiceRecord::static_meta_class,
    &CIM_DiagnosticCompletionRecord::static_meta_class,
    &SF_DiagnosticCompletionRecord::static_meta_class,
    &CIM_CollectionOfMSEs::static_meta_class,
    &CIM_Setting::static_meta_class,
    &CIM_DiagnosticSetting::static_meta_class,
    &CIM_SettingData::static_meta_class,
    &CIM_JobSettingData::static_meta_class,
    &CIM_DiagnosticSettingData::static_meta_class,
    &CIM_DiagnosticService::static_meta_class,
    &CIM_DiagnosticResult::static_meta_class,
    &CIM_DiagnosticTest::static_meta_class,
    &SF_DiagnosticTest::static_meta_class,
    &CIM_DiagnosticServiceCapabilities::static_meta_class,
    &SF_DiagnosticServiceCapabilities::static_meta_class,
    &CIM_RecordAppliesToElement::static_meta_class,
    &SF_RecordAppliesToElement::static_meta_class,
    &CIM_OwningJobElement::static_meta_class,
    &SF_OwningJobElement::static_meta_class,
    &CIM_AffectedJobElement::static_meta_class,
    &SF_AffectedJobElement::static_meta_class,
    &CIM_ServiceAvailableToElement::static_meta_class,
    &CIM_AvailableDiagnosticService::static_meta_class,
    &SF_AvailableDiagnosticService::static_meta_class,
    &SF_DiagnosticSettingData::static_meta_class,
    &CIM_ElementSettingData::static_meta_class,
    &SF_ElementSettingData::static_meta_class,
};

static const size_t _num_meta_classes = CIMPLE_ARRAY_SIZE(_meta_classes);

extern const Meta_Repository __meta_repository_480038F9449B17AC9E2210119CF38AF2;

/*[2844]*/
const Meta_Repository __meta_repository_480038F9449B17AC9E2210119CF38AF2 =
{
    _meta_classes,
    _num_meta_classes,
};

CIMPLE_NAMESPACE_END
