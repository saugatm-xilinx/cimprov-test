#!/usr/bin/python

from test_classes import profile_check

NS = 'root/solarflare'
CARD_NUM = 1

PROFILE_LIST = ['Physical Asset', 'Ethernet Port', 'Host LAN Network Port']

# Specification
SPEC_CLASSES_LIST = [
    ['CIM_Card',
        ['Tag', 'CreationClassName', 'PackageType', 'ElementName'],
        [['CIM_Realizes', 'CIM_PortController', 1],
         ['CIM_ConnectorOnPackage', 'CIM_PhysicalConnector', 2]
        ],
        CARD_NUM,
    ],
    ['CIM_PhysicalConnector',
        ['Tag', 'CreationClassName', 'ConnectorLayout', 'ElementName'],
        [['CIM_Realizes', 'CIM_EthernetPort', 1],
         ['CIM_ConnectorOnPackage','CIM_Card', 1]
        ],
        CARD_NUM * 2,
    ],
    ['CIM_ConnectorOnPackage',
        ['GroupComponent', 'PartComponent'],
        [],
        CARD_NUM * 2,
    ],
    ['CIM_EthernetPort',
        ['Name', 'LinkTechnology', 'PermanentAddress',
         'NetworkAddresses', 'DeviceID', 'PortNumber'],
        [['CIM_ControlledBy', 'CIM_PortController', 1],
         ['CIM_Realizes', 'CIM_PhysicalConnector', 1],
        ],
        CARD_NUM * 2
    ],
    ['CIM_PortController',
        ['Name', 'DeviceID', 'ControllerType'],
        [['CIM_ControlledBy', 'CIM_EthernetPort', 2],
         ['CIM_Realizes', 'CIM_Card', 1],
        ],
        CARD_NUM
    ],
    ['CIM_ControlledBy',
        ['Antecedent', 'Dependent'],
        [],
        CARD_NUM * 2
    ],    
    ['CIM_Realizes',
        ['Antecedent', 'Dependent'],
        [],
        CARD_NUM * 3
    ],
    ['CIM_SystemDevice',
        ['GroupComponent', 'PartComponent'],
        [],
        CARD_NUM * 3
    ],
    ['CIM_LANEndpoint',
        ['SystemCreationClassName', 'CreationClassName', 'SystemName',
         'Name', 'NameFormat', 'ProtocolIFType', 'MACAddress',
         'RequestedState', 'EnabledState', 'ElementName'],
        [['CIM_DeviceSAPImplementation', 'CIM_EthernetPort', 1],
        ],
        CARD_NUM * 2
    ],
    ['CIM_DeviceSAPImplementation',
        ['Antecedent', 'Dependent'],
        [],
        CARD_NUM * 2
    ],
    ['CIM_HostedAccessPoint',
        ['Antecedent', 'Dependent'],
        [],
        CARD_NUM * 2
    ]
]

profile_check(PROFILE_LIST, SPEC_CLASSES_LIST, NS)
