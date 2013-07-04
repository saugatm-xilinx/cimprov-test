#if defined(IMPNS)
INSTANCE
class SF_SoftwareIdentity : CIM_SoftwareIdentity {
};

INSTANCE
class SF_ConcreteJob : CIM_ConcreteJob {
};
#endif

#if defined(ROOTNS) || defined(IMPNS)
ASSOCIATION
class SF_InstalledSoftwareIdentity : CIM_InstalledSoftwareIdentity {
};
#endif

#if defined(IMPNS)
ASSOCIATION
class SF_ElementSoftwareIdentity : CIM_ElementSoftwareIdentity {
      [Override("Dependent")]
   CIM_PortController REF Dependent;
};

ASSOCIATION
class SF_DiagElementSoftwareIdentity : CIM_ElementSoftwareIdentity {
      [Override("Dependent")]
   CIM_DiagnosticTest REF Dependent;
};

ASSOCIATION
class SF_BundleComponent : CIM_OrderedComponent {
      [Override("GroupComponent")]
   CIM_SoftwareIdentity REF GroupComponent;
      [Override("PartComponent")]
   CIM_SoftwareIdentity REF PartComponent;
};

INSTANCE
class SF_ComputerSystem : CIM_ComputerSystem {
};

INSTANCE
class SF_NICCard : CIM_Card {
};

INSTANCE
class SF_PhysicalConnector : CIM_PhysicalConnector {
};

ASSOCIATION
class SF_ConnectorOnNIC : CIM_ConnectorOnPackage {
};

INSTANCE
class SF_EthernetPort : CIM_EthernetPort {
};

INSTANCE
class SF_PortController : CIM_PortController {
};
#endif

#if defined(ROOTNS) || defined(IMPNS)
ASSOCIATION
class SF_SystemDevice : CIM_SystemDevice {
};
#endif

#if defined(IMPNS)
ASSOCIATION
class SF_ConnectorRealizesPort : CIM_Realizes {
};

ASSOCIATION
class SF_CardRealizesController : CIM_Realizes {
};

INSTANCE
class SF_LANEndpoint : CIM_LANEndpoint {
};

ASSOCIATION
class SF_NICSAPImplementation : CIM_DeviceSAPImplementation {
};

INSTANCE
class SF_SoftwareInstallationService : CIM_SoftwareInstallationService {
};

INSTANCE
class SF_SoftwareInstallationServiceCapabilities : CIM_SoftwareInstallationServiceCapabilities {
};

ASSOCIATION
class SF_ElementCapabilities : CIM_ElementCapabilities {
      [Override("ManagedElement")]
   CIM_LogicalElement REF ManagedElement;
};

ASSOCIATION
class SF_ServiceAffectsCard : CIM_ServiceAffectsElement {
      [Override("AffectedElement")]
   CIM_Card REF AffectedElement;
};

ASSOCIATION
class SF_ServiceAffectsController : CIM_ServiceAffectsElement {
      [Override("AffectedElement")]
   CIM_PortController REF AffectedElement;
};
#endif

#if defined(ROOTNS) || defined(IMPNS)
ASSOCIATION
class SF_ServiceAffectsSystem : CIM_ServiceAffectsElement {
      [Override("AffectedElement")]
   CIM_ComputerSystem REF AffectedElement;
};
#endif

#if defined(IMPNS)
ASSOCIATION
class SF_ServiceAffectsSoftware : CIM_ServiceAffectsElement {
      [Override("AffectedElement")]
   CIM_SoftwareIdentity REF AffectedElement;
};
#endif

#if defined(IMPNS) || defined(ROOTNS)
ASSOCIATION
class SF_HostedService : CIM_HostedService {
};
#endif

#if defined(IMPNS)
ASSOCIATION
class SF_ControlledBy : CIM_ControlledBy {
      [Override("Dependent")]
   CIM_EthernetPort REF Dependent;
      [Override("Antecedent")]
   CIM_PortController REF Antecedent;
};
#endif

#if defined(IMPNS) || defined(ROOTNS)
ASSOCIATION
class SF_HostedAccessPoint : CIM_HostedAccessPoint {
};
#endif

#if defined(IMPNS)
INSTANCE
class SF_EnabledLogicalElementCapabilities : CIM_EnabledLogicalElementCapabilities {
};

INSTANCE
class SF_RecordLog : CIM_RecordLog {
};

INSTANCE
class SF_RecordLogCapabilities : CIM_RecordLogCapabilities {
};

INSTANCE
class SF_DiagnosticLog : CIM_DiagnosticLog {
};

INSTANCE
class SF_DiagnosticLogCapabilities : CIM_RecordLogCapabilities {
};

INSTANCE
class SF_DiagnosticCompletionRecord : CIM_DiagnosticCompletionRecord {
};

INSTANCE
class SF_LogEntry : CIM_LogEntry {
};

ASSOCIATION
class SF_LogManagesRecord : CIM_LogManagesRecord {
};
#endif

#if defined(ROOTNS) || defined(IMPNS)
ASSOCIATION
class SF_SystemUseOfLog : CIM_UseOfLog {
      [Override("Dependent")]
   CIM_ComputerSystem REF Dependent;
};
#endif

#if defined(IMPNS)
ASSOCIATION
class SF_DiagnosticUseOfLog : CIM_UseOfLog {
      [Override("Dependent")]
   CIM_DiagnosticTest REF Dependent;
};

INSTANCE
class SF_DiagnosticTest : CIM_DiagnosticTest {
#if defined(TARGET_CIM_SERVER_wmi)
// On Windows [implemented] qualifier should be specified for
// actually implemented method, so we have to copy this method
// definition from DMTF schema here.
   [implemented]
   uint32 RunDiagnosticService(
         [IN, Description ( 
             "Specifies the element upon which the "
             "DiagnosticService SHOULD be run." )]
      CIM_ManagedSystemElement REF ManagedElement, 
         [IN, Description ( 
             "A string containing an encoding of the "
             "DiagnosticSettingData instance to be applied to "
             "the diagnostic. If null, the diagnostic\'s "
             "defaults are used." ), 
          EmbeddedInstance ( "CIM_DiagnosticSettingData" )]
      string DiagnosticSettings, 
         [IN, Description ( 
             "A string containing an encoding of the "
             "JobSettingData instance to be applied to the "
             "resulting job. If null, the job\'s defaults are "
             "used." ), 
          EmbeddedInstance ( "CIM_JobSettingData" )]
      string JobSettings, 
         [IN ( false ), OUT, Description ( 
             "Returns a reference to the resulting Job." )]
      CIM_ConcreteJob REF Job);
#endif
};

INSTANCE
class SF_DiagnosticServiceCapabilities : CIM_DiagnosticServiceCapabilities {
};

ASSOCIATION
class SF_RecordAppliesToElement : CIM_RecordAppliesToElement {
};

ASSOCIATION
class SF_OwningJobElement : CIM_OwningJobElement {
      [Override("OwningElement")]
   CIM_DiagnosticTest REF OwningElement;
};

ASSOCIATION
class SF_AffectedJobElement : CIM_AffectedJobElement {
      [Override("AffectedElement")]
   CIM_PortController REF AffectedElement;
};

ASSOCIATION
class SF_AvailableDiagnosticService : CIM_AvailableDiagnosticService {
      [Override("UserOfService")]
   CIM_Card REF UserOfService;
};

INSTANCE
class SF_DiagnosticSettingData : CIM_DiagnosticSettingData {
};

ASSOCIATION
class SF_ElementSettingData : CIM_ElementSettingData {
};
#endif

#if defined(IMPNS) || defined(INTEROPNS)
INSTANCE
class SF_RegisteredProfile : CIM_RegisteredProfile {
};

ASSOCIATION
class SF_ElementConformsToProfile : CIM_ElementConformsToProfile {
      [Override("ManagedElement")]
    CIM_ManagedSystemElement REF ManagedElement;
};

ASSOCIATION
class SF_ReferencedProfile : CIM_ReferencedProfile {
};
#endif

#if defined(IMPNS) || defined(ROOTNS)
ASSOCIATION
class SF_ComputerSystemPackage : CIM_ComputerSystemPackage {
};
#endif

#if defined(TARGET_CIM_SERVER_pegasus) && defined(IMPNS)
INSTANCE
class IBMPSG_ComputerSystem : CIM_UnitaryComputerSystem
{
string Model;
string LPARID;
};

INSTANCE
class IBMSD_ComputerSystem : CIM_ComputerSystem
{
string UUID;
};

INSTANCE
class IBMSD_SPComputerSystem : CIM_ComputerSystem
{
};

INSTANCE
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

INSTANCE
class PG_RegisteredProfile : CIM_RegisteredProfile
{
};
#endif

#if defined(TARGET_CIM_SERVER_esxi) && defined(IMPNS)
INSTANCE
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

#if defined(IMPNS)
INDICATION
class SF_JobCreated : CIM_InstCreation
{
};

INDICATION
class SF_JobStarted : CIM_InstModification
{
};

INDICATION
class SF_JobError : CIM_InstModification
{
};

INDICATION
class SF_JobSuccess : CIM_InstModification
{
};

INDICATION
class SF_Alert : CIM_AlertIndication
{
};
#endif
