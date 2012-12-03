#!/usr/bin/python

from test_lib import profile_check
from tester_vars import *
from tester_hlpr import *

NS = 'root/PG_InterOp'

PROFILE_LIST = ['Profile Registration']

# Specification
SPEC_CLASSES_LIST = [
    ['SF_RegisteredProfile',
        ['InstanceID', 'RegisteredName', 'RegisteredVersion',
         'RegisteredOrganization', 'AdvertiseTypes',
        ],
        [],
        0
    ],
    ['SF_ElementConformsToProfile',
        ['ConformantStandard', 'ManagedElement'],
        [],
        0
    ],
    ['SF_ReferencedProfile',
        ['Antecedent', 'Dependent'],
        [],
        0
    ]
]

def test_function(param = {}):
    """Profile Registration profile testing"""
    TEST_NAME = "reg_profile"

    test_start(TEST_NAME, test_function.__doc__)
    res = profile_check(PROFILE_LIST, SPEC_CLASSES_LIST, NS)
    test_result(TEST_NAME, res)
