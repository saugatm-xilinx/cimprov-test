#!/usr/bin/python

from test_classes import profile_check

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

profile_check(PROFILE_LIST, SPEC_CLASSES_LIST, NS)
