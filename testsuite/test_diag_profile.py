#!/usr/bin/python

from test_lib import profile_check
from tester_vars import *
from tester_hlpr import *

PROFILE_LIST = ['Diagnostics', 'Job Control', 'Record Log']

SPEC_CLASSES_LIST = [
    ['CIM_AvailableDiagnosticService',
        ['ServiceProvided',
         'UserOfService',
         'EstimatedDurationOfService'
        ],
        [],
        0
    ],
    ['CIM_ConcreteJob',
        ['InstanceID', 
         'Name',
         'JobState',
         'TimeBeforeRemoval',
         'StartTime',
         'ElapsedTime',
         'PercentComplete',
         #'ErrorDescription',
         'RequestedState'
        ],
        [],
        0
    ],
    ['CIM_DiagnosticCompletionRecord',
        ['InstanceID',
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
        [],
        0
    ],
    ['CIM_DiagnosticLog',
        ['InstanceID'
        ],
        [],
        0
    ],
    ['CIM_DiagnosticServiceRecord',
        ['InstanceID',
         'CreationTimeStamp',
         'RecordData',
         'RecordFormat',
         'LoopsPassed',
         'LoopsFailed',
         #'ErrorCode',
         #'ErrorCount',
         'ServiceName',
         'ManagedElementType',
         #'OtherRecordTypeDescription',
         'ExpirationDate'
        ],
        [],
        0
    ],
    ['CIM_DiagnosticTest',
        ['SystemCreationClassName',
         'SystemName',
         'CreationClassName',
         'Name',
         'ElementName',
         'Characteristics',
         #'OtherCharacteristicsDescriptions'
        ],
        [['CIM_ElementSoftwareIdentity', 'CIM_SoftwareIdentity', 1]],
        0
    ],
    ['CIM_ElementSoftwareIdentity',
        ['Antecedent',
         'Dependent'
        ],
        [],
        0
    ],
    ['CIM_HostedService',
        ['Antecedent',
         'Dependent'
        ],
        [],
        0
    ],
    ['CIM_LogManagesRecord',
        ['Log',
         'Record'
        ],
        [],
        0
    ],
    ['CIM_OwningJobElement',
        ['OwningElement',
         'OwnedElement'
        ],
        [],
        0
    ],
    ['CIM_ServiceAffectsElement',
        ['AffectedElement',
         'AffectingElement'
        ],
        [],
        0
    ],
    ['CIM_ServiceAvailableToElement',
        ['ServiceProvided',
         'UserOfService'
        ],
        [],
        0
    ],
    ['CIM_SoftwareIdentity',
        ['InstanceID',
         'MajorVersion',
         'MinorVersion',
         'RevisionNumber',
         'VersionString',
         'Manufacturer'
        ],
        [],
        0
    ],
    ['CIM_RecordLog',
        ['InstanceID',
         'MaxNumberOfRecords',
         'LogState',
         'OverwritePolicy',
         'RequestedState',
         'EnabledState',
         'OperationalStatus',
         'HealthState',
         'ElementName'],
        [],
        0
    ],
    ['CIM_UseOfLog',
        ['Antecedent',
         'Dependent'
        ],
        [],
        0
    ]
]

def test_function(param = {}):
    """Diagnostics, Job Control and Record Log profiles testing..."""
    TEST_NAME = "diag_profile"

    test_start(TEST_NAME, test_function.__doc__)
    res = profile_check(PROFILE_LIST, SPEC_CLASSES_LIST, TESTER_NS)
    test_result(TEST_NAME, res)
