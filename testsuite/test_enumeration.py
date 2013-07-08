#!/usr/bin/python
#
# Enumeration test
#
# Scenario:
# - get classes list
# - run EnumerateInstance and EnumerateInstanceName operations
# - verify that the same number of results are returned for both operations
#

from test_lib import enum_check, class_list_fget
from tester_vars import *
from tester_hlpr import *

if TESTER_WMI:
    import pywmic as wbemcli
else:
    import pywbem as wbemcli

def test_function(param={}):
    """Check EnumerateInstance and EnumerateInstanceName operations"""
    TEST_NAME = "enumeration"

    test_start(TEST_NAME, test_function.__doc__)
    class_list = class_list_fget()
    if class_list == None:
        test_result(TEST_NAME, None)
        return
    
    wbemclient = wbemcli.WBEMConnection(TESTER_HOST,
                                       (TESTER_USER, TESTER_PASSWORD),
                                       TESTER_NS)
    res = enum_check(wbemclient, class_list)
    test_result(TEST_NAME, res)
    
    if TESTER_WMI:
        test_start(TEST_NAME, test_function.__doc__)
        res = enum_check(wbemclient, class_list, True)
        test_result(TEST_NAME, res)
