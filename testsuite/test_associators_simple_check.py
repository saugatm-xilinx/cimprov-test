#!/usr/bin/python

from test_lib import associators_simple_check
from tester_vars import *
from tester_hlpr import *

if TESTER_WMI:
    import pywmic as wbemcli
else:
    import pywbem as wbemcli

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
'SF_DiagnosticSettingData',
'SF_RegisteredProfile',
]

def test_function(param = {}):
    """Check associators existance"""
    TEST_NAME = "associators_simple_check"

    wbemclient = wbemcli.WBEMConnection(TESTER_HOST,
                                       (TESTER_USER, TESTER_PASSWORD),
                                       TESTER_NS)

    test_start(TEST_NAME, test_function.__doc__)
    res = associators_simple_check(wbemclient, CLASSES_LIST, TESTER_NS)
    test_result(TEST_NAME, res)
