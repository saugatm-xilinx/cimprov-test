#!/usr/bin/python

from defs import *
from profile import *

profileList = ['Diagnostics', 'Job Control', 'Record Log']

classDescList = [
    CIMClassDescr(
        name='SF_AvailableDiagnosticService',
        properties=['ServiceProvided',
                    'UserOfService',
                    'EstimatedDurationOfService'],
        associations=[],
        count=CountDescr(CountDescr.ANY)),

    CIMClassDescr(
        name='SF_ConcreteJob',
        properties=['InstanceID', 
                    'Name',
                    'JobState',
                    'TimeBeforeRemoval',
                    'StartTime',
                    'ElapsedTime',
                    'PercentComplete',
                    #'ErrorDescription',
                    #'RequestedState' # see OL 7619
                    ],
        associations=[],
        count=CountDescr(CountDescr.ANY)),

    CIMClassDescr(
        name='SF_DiagnosticCompletionRecord',
        properties=['InstanceID',
                    'CreationTimeStamp',
                    'RecordData',
                    'RecordFormat',
                    'ServiceName',
                    'ManagedElementName',
                    'RecordType',
                    'ExpirationDate',
                    'CompletionState',
                    'OtherCompletionStateDescription', # cond
                    'ErrorCode',
                    'ErrorCount'
                    ],
        associations=[],
        count=CountDescr(CountDescr.ANY)),

    CIMClassDescr(
        name='SF_DiagnosticLog',
        properties=['InstanceID'],
        associations=[],
        count=CountDescr(CountDescr.ANY)),

    CIMClassDescr(
        name='SF_DiagnosticTest',
        properties=['SystemCreationClassName',
                    'SystemName',
                    'CreationClassName',
                    'Name',
                    'ElementName',
                    'Characteristics',
                    #'OtherCharacteristicsDescriptions'
                    ],
        associations=[],
        # [['SF_ElementSoftwareIdentity', 'SF_SoftwareIdentity', 1]],
        ## we don't have separate diagnostic tool, so no association
        count=CountDescr(CountDescr.ANY)),
    
    CIMClassDescr(
        name='SF_ElementSoftwareIdentity',
        properties=['Antecedent', 'Dependent'],
        associations=[],
        count=CountDescr(CountDescr.ANY)),

    CIMClassDescr(
        name='SF_HostedService',
        properties=['Antecedent', 'Dependent'],
        associations=[],
        count=CountDescr(CountDescr.ANY)),

    CIMClassDescr(
        name='SF_LogManagesRecord',
        properties=['Log', 'Record'],
        associations=[],
        count=CountDescr(CountDescr.ANY)),

    CIMClassDescr(
        name='SF_OwningJobElement',
        properties=['OwningElement', 'OwnedElement'],
        associations=[],
        count=CountDescr(CountDescr.ANY)),

    CIMClassDescr(
        name='SF_ServiceAffectsSystem',
        properties=['AffectedElement', 'AffectingElement'],
        associations=[],
        count=CountDescr(CountDescr.ANY)),

    CIMClassDescr(
        name='SF_SoftwareIdentity',
        properties=['InstanceID',
                    'MajorVersion',
                    'MinorVersion',
                    'RevisionNumber',
                    'VersionString',
                    'Manufacturer'],
        associations=[],
        count=CountDescr(CountDescr.POSITIVE)),

    CIMClassDescr(
        name='SF_RecordLog',
        properties=['InstanceID',
                    'MaxNumberOfRecords',
                    'LogState',
                    'OverwritePolicy',
                    'RequestedState',
                    'EnabledState',
                    'OperationalStatus',
                    'HealthState',
                    'ElementName'],
        associations=[],
        count=CountDescr(CountDescr.POSITIVE)),

    CIMClassDescr(
        name='SF_SystemUseOfLog',
        properties=['Antecedent','Dependent'],
        associations=[],
        count=CountDescr(CountDescr.POSITIVE)),

    CIMClassDescr(
        name='SF_DiagnosticUseOfLog',
        properties=['Antecedent', 'Dependent'],
        associations=[],
        count=CountDescr(CountDescr.POSITIVE))
]

ProfileTest(name='diag_profile',
            desc='Diagnostics, Job Control and Record Log',
            params=ProfileParams(profileList,
                                 classDescList))

