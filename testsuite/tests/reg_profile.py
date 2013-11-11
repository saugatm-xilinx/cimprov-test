#!/usr/bin/python

from defs import *
from profile import *

profileList = ['Profile Registration']
profileSuppCount = 9

# For each profile we have association 
# with Profile Registration (except itself) + 3 more:
# Host LAN Network Port - Ethernet Port
# Software Inventory - Software Update
# Physical Asset - Host LAN Network Port
profileRefsCount = profileSuppCount - 1 + 3

# Specification
classDescList = [
    CIMClassDescr(
        name='SF_RegisteredProfile',
        properties=['InstanceID',
                    'RegisteredName',
                    'RegisteredVersion',
                    'RegisteredOrganization',
                    'AdvertiseTypes',
        ],
        associations=[],
        count=profileSuppCount),

    CIMClassDescr(
        name='SF_ElementConformsToProfile',
        properties=['ConformantStandard', 'ManagedElement'],
        associations=[],
        count=CountDescr(CountDescr.POSITIVE)),

    CIMClassDescr(
        name='SF_ReferencedProfile',
        properties=['Antecedent', 'Dependent'],
        associations=[],
        count=profileRefsCount),
]

ProfileTest(
    name='reg_profile',
    desc="Profile Registration",
    params=ProfileParams(profileList, classDescList),
    ns=getTesterCfg().interopNS)
