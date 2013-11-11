#!/usr/bin/python

from defs import *
from profile import *

profileList = ['Software Inventory', 'Software Update']

# Specification
classDescList = [
    CIMClassDescr(
        name='SF_HostedService',
        properties=['Antecedent', 'Dependent'],
        associations=[],
        count=CountDescr(CountDescr.POSITIVE)),

    CIMClassDescr(
        name='SF_SoftwareInstallationService',
        properties=['SystemCreationClassName',
                    'SystemName',
                    'CreationClassName',
                    'Name'],
        associations=[],
        count=CountDescr(CountDescr.POSITIVE)),

    CIMClassDescr(
        name='SF_ElementCapabilities',
        properties=['ManagedElement', 'Capabilities'],
        associations=[],
        count=CountDescr(CountDescr.POSITIVE)),

    CIMClassDescr(
        name='SF_ServiceAffectsSystem',
        properties=['AffectedElement', 'AffectingElement'],
        associations=[],
        count=CountDescr(CountDescr.POSITIVE)),

    CIMClassDescr(
        name='SF_SoftwareIdentity',
        properties=['InstanceID',
                    'IsEntity',
                    'VersionString',
                    'MajorVersion',
                    'MinorVersion',
                    'RevisionNumber',
                    'IdentityInfoType',
                    'IdentityInfoValue',
                    'Classifications'],
###     skipped
#         'BuildNumber', 'TargetOSType', 'TargetOperatingSystem',
###
        associations=[],
        count=CountDescr(CountDescr.POSITIVE)),

    CIMClassDescr(
        name='SF_InstalledSoftwareIdentity',
        properties=['System', 'InstalledSoftware'],
        associations=[],
        count=CountDescr(CountDescr.POSITIVE)),

    CIMClassDescr(
        name='SF_ElementSoftwareIdentity',
        properties=['Antecedent', 'Dependent', 'ElementSoftwareStatus'],
        associations=[],
        count=CountDescr(CountDescr.POSITIVE)),

    CIMClassDescr(
        name='SF_BundleComponent',
        properties=['GroupComponent', 'PartComponent', 'AssignedSequence'],
        associations=[],
        count=CountDescr(CountDescr.POSITIVE)),

###    skipped
#    ['SF_SoftwareInstallationCapabilities',
#        ['InstanceID', 'SupportedInstallOptions'],
#        [],
#        0
#    ],
#    ['SF_SystemSpecificCollection',
#        [],
#        [],
#        0
#    ],
#    ['SF_HostedCollection',
#        [],
#        [],
#        0
#    ],
#    ['SF_MemberOfCollection',
#        [],
#        [],
#        0
#    ],
#    ['SF_SoftwareIdentityResource',
#        [],
#        [],
#        0
#    ],
#    ['SF_SAPAvailableForElement',
#        [],
#        [],
#        0
#    ],
#    ['SF_HostedAccessPoint',
#        ['Antecedent', 'Dependent'],
#        [],
#        0
#    ],
#    ['SF_OrderedDependency',
#        ['GroupComponent', 'PartComponent', 'AssignedSequence'],
#        [],
#        0
#    ],
###    
]

ProfileTest(
    name='sw_profile',
    desc="Software Inventory and Software Update",
    params=ProfileParams(profileList, classDescList))

