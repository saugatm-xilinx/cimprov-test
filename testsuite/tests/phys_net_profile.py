#!/usr/bin/python

from vars import *
from defs import *
from profile import *

profileList = ['Physical Asset', 'Ethernet Port', 'Host LAN Network Port']

# Specification
classDescList = [
    CIMClassDescr(
        name='SF_NICCard',
        properties=['Tag',
                    'CreationClassName',
                    'PackageType',
                    'Name',
                    'ElementName',
                    'HostingBoard',
                    'Manufacturer',
                    'Model',
                    'SerialNumber',
                    'PartNumber',
                    'SKU'],
        associations=[CIMAssocDescr(
                        associationName='SF_CardRealizesController',
                        associatorName='SF_PortController',
                        associatorsCount=PORT_CTRL_PER_NIC),
                      CIMAssocDescr(
                        associationName='SF_ConnectorOnNIC',
                        associatorName='SF_PhysicalConnector',
                        associatorsCount=PORT_PER_NIC)],
        count=getTesterCfg().NICNum),

    CIMClassDescr(
        name='SF_ComputerSystemPackage',
        properties=['Antecedent', 'Dependent', 'PlatformGUID'],
        associations=[],
        count=getTesterCfg().NICNum),

    CIMClassDescr(
        name='SF_PhysicalConnector',
        properties=['Tag',
                    'CreationClassName',
                    'ConnectorLayout',
                    'ElementName'],
        associations=[CIMAssocDescr(
                        associationName='SF_ConnectorRealizesPort',
                        associatorName='SF_EthernetPort',
                        associatorsCount=1),
                      CIMAssocDescr(
                        associationName='SF_ConnectorOnNIC',
                        associatorName='SF_NICCard',
                        associatorsCount=1)],
        count=getTesterCfg().NICNum * PORT_PER_NIC),

    CIMClassDescr(
        name='SF_ConnectorOnNIC',
        properties=['GroupComponent', 'PartComponent'],
        associations=[],
        count=getTesterCfg().NICNum * PORT_PER_NIC),

    CIMClassDescr(
        name='SF_EthernetPort',
        properties=['Name',
                    'LinkTechnology',
                    'PermanentAddress',
                    'NetworkAddresses',
                    'DeviceID',
                    'PortNumber',
                    'PortType',
                    'Capabilities',
                    'EnabledCapabilities',
                    'Speed',
                    'MaxSpeed'],
        associations=[CIMAssocDescr(
                        associationName='SF_ControlledBy',
                        associatorName='SF_PortController',
                        associatorsCount=1),
                      CIMAssocDescr(
                         associationName='SF_ConnectorRealizesPort',
                         associatorName='SF_PhysicalConnector',
                         associatorsCount=1)],
        count=getTesterCfg().NICNum * PORT_PER_NIC),

    CIMClassDescr(
        name='SF_PortController',
        properties=['Name',
                    'DeviceID',
                    'ControllerType',
                    'ProtocolSupported',
                    'CreationClassName',
                    'MaxNumberControlled'],
        associations=[CIMAssocDescr(
                        associationName='SF_ControlledBy',
                        associatorName='SF_EthernetPort',
                        associatorsCount=PORT_PER_NIC),
                      CIMAssocDescr(
                        associationName='SF_CardRealizesController',
                        associatorName='SF_NICCard',
                        associatorsCount=1)],
        count=getTesterCfg().NICNum * PORT_CTRL_PER_NIC),

    CIMClassDescr(
        name='SF_ControlledBy',
        properties=['Antecedent', 'Dependent'],
        associations=[],
        count=getTesterCfg().NICNum * PORT_PER_NIC),

    CIMClassDescr(
        name='SF_CardRealizesController',
        properties=['Antecedent', 'Dependent'],
        associations=[],
        count=getTesterCfg().NICNum * PORT_CTRL_PER_NIC),

    CIMClassDescr(
        name='SF_ConnectorRealizesPort',
        properties=['Antecedent', 'Dependent'],
        associations=[],
        count=getTesterCfg().NICNum * PORT_PER_NIC),

    CIMClassDescr(
        name='SF_SystemDevice',
        properties=['GroupComponent', 'PartComponent'],
        associations=[],
        count=getTesterCfg().NICNum * (1 + PORT_PER_NIC)),

    CIMClassDescr(
        name='SF_LANEndpoint',
        properties=['SystemCreationClassName',
                    'CreationClassName',
                    'SystemName',
                    'Name',
                    'NameFormat',
                    'ProtocolIFType',
                    'MACAddress',
                    'RequestedState',
                    'EnabledState',
                    'ElementName'],
        associations=[CIMAssocDescr(
                        associationName='SF_NICSAPImplementation',
                        associatorName='SF_EthernetPort',
                        associatorsCount=1)],
        count=getTesterCfg().NICNum * PORT_PER_NIC),

    CIMClassDescr(
        name='SF_NICSAPImplementation',
        properties=['Antecedent', 'Dependent'],
        associations=[],
        count=getTesterCfg().NICNum * PORT_PER_NIC),
    
    CIMClassDescr(
        name='SF_HostedAccessPoint',
        properties=['Antecedent', 'Dependent'],
        associations=[],
        count=getTesterCfg().NICNum * PORT_PER_NIC),
]

ProfileTest(
    name='phys_net_profile',
    desc="Physical asset, Ethernet and Host LAN Network Port",
    params=ProfileParams(profileList, classDescList))


