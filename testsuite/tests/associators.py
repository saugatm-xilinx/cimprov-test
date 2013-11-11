#!/usr/bin/python

from test_lib import checkAssociatorsCount
from defs import *

testDesc = "Check associators existance"
testSpec = """
Scenario:
    - for each association class from given list check
      that it has >0 associators
"""

# Associations classes list for which we're expecting >0 associators
CLASSES_LIST = [
'SF_SoftwareIdentity',
'SF_NICCard',
'SF_PhysicalConnector',
'SF_EthernetPort',
'SF_PortController',
'SF_EnabledLogicalElementCapabilities',
'SF_LANEndpoint',
'SF_SoftwareInstallationService',
'SF_SoftwareInstallationServiceCapabilities',
'SF_RecordLog',
'SF_RecordLogCapabilities',
'SF_LogEntry',
'SF_DiagnosticLog',
'SF_DiagnosticLogCapabilities',
'SF_DiagnosticTest',
'SF_DiagnosticServiceCapabilities',
'SF_RegisteredProfile',
]

def associatorsMain(conn, params={}):
    return checkAssociatorsCount(conn=conn, classList=CLASSES_LIST)

def associatorsIter(self, params={}):
    if getTesterCfg().isWMI:
        self.testRun()

Test(name='associators',
     desc=testDesc,
     spec=testSpec,
     package='misc',
     func=associatorsMain,
     iter=associatorsIter)
