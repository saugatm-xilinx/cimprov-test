#!/usr/bin/python

from test_lib import profile_check
from tester_vars import *
from tester_hlpr import *

PROFILE_LIST = ['Diagnostics', 'Job Control', 'Record Log']

SPEC_CLASSES_LIST = [
    ['SF_AvailableDiagnosticService',
        ['ServiceProvided',
         'UserOfService',
         'EstimatedDurationOfService'
        ],
        [],
        0
    ],
    ['SF_ConcreteJob',
        ['InstanceID', 
         'Name',
         'JobState',
         'TimeBeforeRemoval',
         'StartTime',
         'ElapsedTime',
         'PercentComplete',
         #'ErrorDescription',
         #'RequestedState' # see OL 7619
        ],
        [],
        0
    ],
    ['SF_DiagnosticCompletionRecord',
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
    ['SF_DiagnosticLog',
        ['InstanceID'
        ],
        [],
        0
    ],
    ['SF_DiagnosticTest',
        ['SystemCreationClassName',
         'SystemName',
         'CreationClassName',
         'Name',
         'ElementName',
         'Characteristics',
         #'OtherCharacteristicsDescriptions'
        ],
        [],
        # [['SF_ElementSoftwareIdentity', 'SF_SoftwareIdentity', 1]],
        ## we don't have separate diagnostic tool, so no association
        0
    ],
    ['SF_ElementSoftwareIdentity',
        ['Antecedent',
         'Dependent'
        ],
        [],
        0
    ],
    ['SF_HostedService',
        ['Antecedent',
         'Dependent'
        ],
        [],
        0
    ],
    ['SF_LogManagesRecord',
        ['Log',
         'Record'
        ],
        [],
        0
    ],
    ['SF_OwningJobElement',
        ['OwningElement',
         'OwnedElement'
        ],
        [],
        0
    ],
    ['SF_ServiceAffectsSystem',
        ['AffectedElement',
         'AffectingElement'
        ],
        [],
        0
    ],
    ['SF_SoftwareIdentity',
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
    ['SF_RecordLog',
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
    ['SF_SystemUseOfLog',
        ['Antecedent',
         'Dependent'
        ],
        [],
        0
    ],
    ['SF_DiagnosticUseOfLog',
        ['Antecedent',
         'Dependent'
        ],
        [],
        0
    ]

]

WMI_UNSUPP_LIST = {
        'SF_ConcreteJob' : ['PercentComplete'],
        'SF_RecordLog' : ['OperationalStatus', 'HealthState', 'ElementName'],
        'SF_DiagnosticTest' : ['ElementName']
}                

def test_function(param = {}):
    """Diagnostics, Job Control and Record Log profiles testing..."""
    TEST_NAME = "diag_profile"

    test_start(TEST_NAME, test_function.__doc__)
    res = profile_check(PROFILE_LIST, SPEC_CLASSES_LIST, TESTER_NS,
            TESTER_WMI and WMI_UNSUPP_LIST or {})
    test_result(TEST_NAME, res)
