

class SF_SoftwareIdentity : CIM_SoftwareIdentity {
};

class SF_ConcreteJob : CIM_ConcreteJob {
};

[Association]
class SF_InstalledSoftwareIdentity : CIM_InstalledSoftwareIdentity {
};

[Association]
class SF_ElementSoftwareIdentity : CIM_ElementSoftwareIdentity {
};


[Association]
class SF_BundleComponent : CIM_OrderedComponent {
};

class SF_ComputerSystem : CIM_ComputerSystem {
};

class SF_NICCard : CIM_Card {
};

class SF_PhysicalConnector : CIM_PhysicalConnector {
};

[Association]
class SF_ConnectorOnNIC : CIM_ConnectorOnPackage {
};

class SF_EthernetPort : CIM_EthernetPort {
};

class SF_PortController : CIM_PortController {
};

[Association]
class SF_SystemDevice : CIM_SystemDevice {
};

[Association]
class SF_ConnectorRealizesPort : CIM_Realizes {
};

[Association]
class SF_CardRealizesController : CIM_Realizes {
};

class SF_LANEndpoint : CIM_LANEndpoint {
};

[Association]
class SF_NICSAPImplementation : CIM_DeviceSAPImplementation {
};

class SF_SoftwareInstallationService : CIM_SoftwareInstallationService {
};

class SF_SoftwareInstallationServiceCapabilities : CIM_SoftwareInstallationServiceCapabilities {
};

[Association]
class SF_ElementCapabilities : CIM_ElementCapabilities {
};

[Association]
class SF_ServiceAffectsElement : CIM_ServiceAffectsElement {
};

[Association]
class SF_HostedService : CIM_HostedService {
};

[Association]
class SF_ControlledBy : CIM_ControlledBy {
};

[Association]
class SF_HostedAccessPoint : CIM_HostedAccessPoint {
};

class SF_EnabledLogicalElementCapabilities : CIM_EnabledLogicalElementCapabilities {
};

class SF_RecordLog : CIM_RecordLog {
};

class SF_RecordLogCapabilities : CIM_RecordLogCapabilities {
};

class SF_DiagnosticLog : CIM_DiagnosticLog {
};

class SF_DiagnosticLogCapabilities : CIM_RecordLogCapabilities {
};

class SF_DiagnosticCompletionRecord : CIM_DiagnosticCompletionRecord {
};

class SF_LogEntry : CIM_LogEntry {
};

[Association]
class SF_LogManagesRecord : CIM_LogManagesRecord {
};

[Association]
class SF_UseOfLog : CIM_UseOfLog {
};

class SF_DiagnosticTest : CIM_DiagnosticTest {
};

class SF_DiagnosticServiceCapabilities : CIM_DiagnosticServiceCapabilities {
};

[Association]
class SF_RecordAppliesToElement : CIM_RecordAppliesToElement {
};

[Association]
class SF_OwningJobElement : CIM_OwningJobElement {
};

[Association]
class SF_AffectedJobElement : CIM_AffectedJobElement {
};

[Association]
class SF_AvailableDiagnosticService : CIM_AvailableDiagnosticService {
};

class SF_DiagnosticSettingData : CIM_DiagnosticSettingData {
};

[Association]
class SF_ElementSettingData : CIM_ElementSettingData {
};

class SF_RegisteredProfile : CIM_RegisteredProfile {
};

[Association]
class SF_ElementConformsToProfile : CIM_ElementConformsToProfile {
};

[Association]
class SF_ReferencedProfile : CIM_ReferencedProfile {
};

[Association]
class SF_ComputerSystemPackage : CIM_ComputerSystemPackage {
};

#ifdef TARGET_CIM_SERVER_pegasus
class IBMPSG_ComputerSystem : CIM_UnitaryComputerSystem
{
string Model;
string LPARID;
};

class IBMSD_ComputerSystem : CIM_ComputerSystem
{
string UUID;
};

class IBMSD_SPComputerSystem : CIM_ComputerSystem
{
};

class PG_ComputerSystem : CIM_UnitaryComputerSystem
{  
    [
        Description ("The pager number of the primary system owner"
                     "available if DMI is installed, else blank."),
        MappingStrings {"MIF.DMTF|General Information|001.9"}
    ]  
    string PrimaryOwnerPager;

    [
        Description ("The name of the secondary owner of the system"
                     "available if DMI is installed, else blank."),
        MappingStrings {"MIF.DMTF|General Information|001.7"}
    ] 
    string SecondaryOwnerName;

    [
        Description ("The phone number of the secondary system owner,"
                     "available if DMI is installed, else blank."),
        MappingStrings {"MIF.DMTF|General Information|001.8"}
    ]  
    string SecondaryOwnerContact;

    [Description ("The pager number of the secondary system owner,"
                  "available if DMI is installed, else blank."),
     MappingStrings {"MIF.DMTF|General Information|001.10"}
    ]  
    string SecondaryOwnerPager;

    [Description ("The serial number of the system, fetched by the"
                  "uname() system service."),
     MappingStrings {"MIF.DMTF|General Information|001.13"}
    ]  
    string SerialNumber;

    [Description ("The identification number of the system,"
                  "currently blank."),
     MappingStrings {"MIF.DMTF|General Information|001.13"}
    ]  
    string IdentificationNumber;
};
#endif

#ifdef TARGET_CIM_SERVER_esxi
class OMC_UnitaryComputerSystem : CIM_UnitaryComputerSystem
{
      [Override("Caption")]
   string Caption;
      [Override("CreationClassName")]
   string CreationClassName;
      [Override("Dedicated")]
   uint16 Dedicated[];
      [Override("ElementName")]
   string ElementName;
      [Override("EnabledDefault")]
   uint16 EnabledDefault = 2;
      [Override("EnabledState")]
   uint16 EnabledState = 5;
      [Override("Name")]
   string Name;
      [Override("NameFormat")]
   string NameFormat;
      [Override("OtherDedicatedDescriptions")]
   string OtherDedicatedDescriptions[];
      [Override("RequestedState")]
   uint16 RequestedState = 12;
      [Override("ResetCapability")]
   uint16 ResetCapability;
};
#endif

[Indication]
class SF_JobCreated : CIM_InstCreation
{
};

[Indication]
class SF_JobStarted : CIM_InstModification
{
};

[Indication]
class SF_JobError : CIM_InstModification
{
};

[Indication]
class SF_JobSuccess : CIM_InstModification
{
};

[Indication]
class SF_Alert : CIM_AlertIndication
{
};