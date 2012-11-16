#!/usr/bin/python

from test_classes import profile_check

NS = 'root/solarflare'

# Specification
SPEC_CLASSES_LIST = [
    ['CIM_HostedService',
        ['Antecedent', 'Dependent'],
        [],
        0
    ],
    ['CIM_SoftwareInstallationService',
        ['SystemCreationClassName', 'SystemName',
         'CreationClassName', 'Name'],
        [],
        0,
    ],
    ['CIM_ElementCapabilities',
        ['ManagedElement', 'Capabilities'],
        [],
        0
    ],
    ['CIM_SoftwareInstallationCapabilities',
        ['InstanceID', 'SupportedInstallOptions'],
        [],
        0
    ],
    ['CIM_ServiceAffectsElement',
        ['AffectedElement', 'AffectingElement'],
        [],
        0
    ],
    ['CIM_SoftwareIdentity',
        ['InstanceID', 'IsEntity', 'VersionString',
         'MajorVersion', 'MinorVersion', 'RevisionNumber', 'BuildNumber',
         'TargetOSType', 'TargetOperatingSystem',
         'IdentityInfoType', 'IdentityInfoValue', 'Classifications'],
        [],
        0
    ],
    ['CIM_InstalledSoftwareIdentity',
        ['System', 'InstalledSoftware'],
        [],
        0
    ],
    ['CIM_ElementSoftwareIdentity',
        ['Antecedent', 'Dependent', 'ElementSoftwareStatus'],
        [],
        0
    ],
    ['CIM_SystemSpecificCollection',
        [],
        [],
        0
    ],
    ['CIM_HostedCollection',
        [],
        [],
        0
    ],
    ['CIM_MemberOfCollection',
        [],
        [],
        0
    ],
    ['CIM_SoftwareIdentityResource',
        [],
        [],
        0
    ],
    ['CIM_SAPAvailableForElement',
        [],
        [],
        0
    ],
    ['CIM_HostedAccessPoint',
        ['Antecedent', 'Dependent'],
        [],
        0
    ],
    ['CIM_OrderedComponent',
        ['GroupComponent', 'PartComponent', 'AssignedSequence'],
        [],
        0
    ],
    ['CIM_OrderedDependency',
        ['GroupComponent', 'PartComponent', 'AssignedSequence'],
        [],
        0
    ],
]

PROFILE_LIST = ['Software Inventory', 'Software Update']

profile_check(PROFILE_LIST, SPEC_CLASSES_LIST, NS)
