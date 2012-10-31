#!/usr/bin/python

import pywbem

#TODO: parameters
NS = 'root/solarflare'
CARD_NUM = 1
HOST='http://127.0.0.1:5988'
USER=''
PASSWORD=''

# Indices of specification list
SC_NAME         = 0
SC_PROP         = 1
SC_ASSOC        = 2
SC_INST_NUM     = 3

SC_ASSOC_ACL    = 0
SC_ASSOC_RCL    = 1
SC_ASSOC_NUM    = 2

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
        [],
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
    ]
]

wbemclient = pywbem.WBEMConnection(HOST, (USER, PASSWORD), NS)
print "Checking Physical Asset and Ethernet Port profile classes:"
for class_spec in SPEC_CLASSES_LIST:
    print "%-60s" % class_spec[SC_NAME],
    try:
        inst_list = wbemclient.EnumerateInstances(class_spec[SC_NAME])
    except:
        print "ENOENT\n"
    else:
        print "OK"
        inst_count = 0
        for inst in inst_list:
            inst_count += 1
            print "    instance #", inst_count

            # Checking properties
            for req_prop in class_spec[SC_PROP]:
                print "        %-52s" % req_prop,
                try:
                    if inst.properties[req_prop].value != None:
                        print "OK"
                    else:
                        print "NULL"
                except:
                    print "ENOENT"

            # Checking associations
            for assoc_spec in class_spec[SC_ASSOC]:
                title = "{0} - {1} x{2}".format(assoc_spec[SC_ASSOC_ACL],
                                                assoc_spec[SC_ASSOC_RCL],
                                                assoc_spec[SC_ASSOC_NUM])
                print "        %-52s" % title,
                try:
                    assoc_list = wbemclient.Associators(inst.path,
                        AssocClass=assoc_spec[SC_ASSOC_ACL],
                        ResultClass=assoc_spec[SC_ASSOC_RCL])
                except:
                    print "FAIL"
                else:
                    assoc_count = 0
                    for assoc in assoc_list:
                        assoc_count += 1
                    if assoc_count != assoc_spec[SC_ASSOC_NUM]:
                        print "EINVAL (%d)" % assoc_count
                    else:
                        print "OK"
                   
        total = "Expected: " + str(class_spec[SC_INST_NUM]) + (
                class_spec[SC_INST_NUM] == 1 and " instance" or " instances")
        print "%-60s" % total,
        
        if (class_spec[SC_INST_NUM] > 0 and
            class_spec[SC_INST_NUM] != inst_count):
            print "EINVAL (%d)\n" % inst_count
        else:
            print "OK\n"
