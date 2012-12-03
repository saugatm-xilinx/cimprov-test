#!/usr/bin/python
#
# Enumeration test
#
# Scenario:
# - get classes list
# - run EnumerateInstance and EnumerateInstanceName operations
# - verify that the same number of results are returned for both operations
#

import pywbem
from test_lib import enum_check, class_list_fget
from tester_vars import *
from tester_hlpr import *

def test_function(param={}):
    """Check EnumerateInstance and EnumerateInstanceName operations"""
    TEST_NAME = "enumeration"

    test_start(TEST_NAME, test_function.__doc__)
    class_list = class_list_fget()
    if class_list == None:
        test_result(TEST_NAME, None)
        return
    
    wbemclient = pywbem.WBEMConnection(TESTER_HOST,
                                       (TESTER_USER, TESTER_PASSWORD),
                                       TESTER_NS)
    res = enum_check(wbemclient, class_list)
    test_result(TEST_NAME, res)

