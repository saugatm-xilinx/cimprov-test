#!/usr/bin/python

from test_lib import profile_check
from tester_vars import *
from tester_hlpr import *

# Specification
SPEC_CLASSES_LIST = [
    ['SF_HostedService',
        ['Antecedent', 'Dependent'],
        [],
        0
    ],
    ['SF_SoftwareInstallationService',
        ['SystemCreationClassName', 'SystemName',
         'CreationClassName', 'Name'],
        [],
        0,
    ],
    ['SF_ElementCapabilities',
        ['ManagedElement', 'Capabilities'],
        [],
        0
    ],
###    skipped
#    ['SF_SoftwareInstallationCapabilities',
#        ['InstanceID', 'SupportedInstallOptions'],
#        [],
#        0
#    ],
###    
    ['SF_ServiceAffectsSystem',
        ['AffectedElement', 'AffectingElement'],
        [],
        0
    ],
    ['SF_SoftwareIdentity',
        ['InstanceID', 'IsEntity', 'VersionString',
         'MajorVersion', 'MinorVersion', 'RevisionNumber',
###     skipped
#         'BuildNumber', 'TargetOSType', 'TargetOperatingSystem',
###
         'IdentityInfoType', 'IdentityInfoValue', 'Classifications'],
        [],
        0
    ],
    ['SF_InstalledSoftwareIdentity',
        ['System', 'InstalledSoftware'],
        [],
        0
    ],
    ['SF_ElementSoftwareIdentity',
        ['Antecedent', 'Dependent', 'ElementSoftwareStatus'],
        [],
        0
    ],
###    skipped
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
###    
    ['SF_BundleComponent',
        ['GroupComponent', 'PartComponent', 'AssignedSequence'],
        [],
        0
    ],
###    skipped
#    ['SF_OrderedDependency',
#        ['GroupComponent', 'PartComponent', 'AssignedSequence'],
#        [],
#        0
#    ],
###    
]

PROFILE_LIST = ['Software Inventory', 'Software Update']

def test_function(param = {}):
    """Software Inventory and Software Update profiles testing"""
    TEST_NAME = "sw_profile"

    test_start(TEST_NAME, test_function.__doc__)
    res = profile_check(PROFILE_LIST, SPEC_CLASSES_LIST, TESTER_NS)
    test_result(TEST_NAME, res)

