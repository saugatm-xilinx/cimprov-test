#!/usr/bin/python

from test_lib import profile_check
from tester_vars import *
from tester_hlpr import *

CARD_NUM = 1

PROFILE_LIST = ['Physical Asset', 'Ethernet Port', 'Host LAN Network Port']

# Specification
SPEC_CLASSES_LIST = [
    ['SF_NICCard',
        ['Tag', 'CreationClassName', 'PackageType', 'Name', 'ElementName',
         'HostingBoard', 'Manufacturer', 'Model', 'SerialNumber',
         'PartNumber', 'SKU'],
        [['SF_CardRealizesController', 'SF_PortController', 1],
         ['SF_ConnectorOnNIC', 'SF_PhysicalConnector', 2]
        ],
        CARD_NUM,
    ],
    ['SF_ComputerSystemPackage',
        ['Antecedent', 'Dependent', 'PlatformGUID'],
        [],
        CARD_NUM
    ],
    ['SF_PhysicalConnector',
        ['Tag', 'CreationClassName', 'ConnectorLayout', 'ElementName'],
        [['SF_ConnectorRealizesPort', 'SF_EthernetPort', 1],
         ['SF_ConnectorOnNIC','SF_NICCard', 1]
        ],
        CARD_NUM * 2,
    ],
    ['SF_ConnectorOnNIC',
        ['GroupComponent', 'PartComponent'],
        [],
        CARD_NUM * 2,
    ],
    ['SF_EthernetPort',
        ['Name', 'LinkTechnology', 'PermanentAddress',
         'NetworkAddresses', 'DeviceID', 'PortNumber',
         'PortType', 'Capabilities', 'EnabledCapabilities',
         'Speed', 'MaxSpeed'],
        [['SF_ControlledBy', 'SF_PortController', 1],
         ['SF_ConnectorRealizesPort', 'SF_PhysicalConnector', 1],
        ],
        CARD_NUM * 2
    ],
    ['SF_PortController',
        ['Name', 'DeviceID', 'ControllerType', 'ProtocolSupported',
         'CreationClassName', 'MaxNumberControlled'],
        [['SF_ControlledBy', 'SF_EthernetPort', 2],
         ['SF_CardRealizesController', 'SF_NICCard', 1],
        ],
        CARD_NUM
    ],
    ['SF_ControlledBy',
        ['Antecedent', 'Dependent'],
        [],
        CARD_NUM * 2
    ],    
    ['SF_CardRealizesController',
        ['Antecedent', 'Dependent'],
        [],
        CARD_NUM
    ],
    ['SF_ConnectorRealizesPort',
        ['Antecedent', 'Dependent'],
        [],
        CARD_NUM * 2
    ],
    ['SF_SystemDevice',
        ['GroupComponent', 'PartComponent'],
        [],
        CARD_NUM * 3
    ],
    ['SF_LANEndpoint',
        ['SystemCreationClassName', 'CreationClassName', 'SystemName',
         'Name', 'NameFormat', 'ProtocolIFType', 'MACAddress',
         'RequestedState', 'EnabledState', 'ElementName'],
        [['SF_NICSAPImplementation', 'SF_EthernetPort', 1],
        ],
        CARD_NUM * 2
    ],
    ['SF_NICSAPImplementation',
        ['Antecedent', 'Dependent'],
        [],
        CARD_NUM * 2
    ],
    ['SF_HostedAccessPoint',
        ['Antecedent', 'Dependent'],
        [],
        CARD_NUM * 2
    ]
]

WMI_UNSUPP_LIST = {
        'SF_NICCard' : ['PackageType', 'ElementName'],
        'SF_PhysicalConnector' : ['ConnectorLayout', 'ElementName'],
        'SF_LANEndpoint' : ['RequestedState', 'EnabledState', 'ElementName'],
        'SF_ComputerSystemPackage' : ['PlatformGUID'],
        'SF_PortCOntroller' : ['ProtocolSupported']
}

def test_function(param = {}):
    """Physical asset, Ethernet and Host LAN Network Port profiles testing"""
    TEST_NAME = "phys_net_profile"
    
    test_start(TEST_NAME, test_function.__doc__)
    res = profile_check(PROFILE_LIST, SPEC_CLASSES_LIST, TESTER_NS,
            TESTER_WMI and WMI_UNSUPP_LIST or {})
    test_result(TEST_NAME, res)
