#!/usr/bin/python

from test_lib import ns_check
from tester_vars import *
from tester_hlpr import *

def test_function(param = {}):
    """Check namespaces existance"""
    TEST_NAME = "namespace"

    test_start(TEST_NAME, test_function.__doc__)
    res = ns_check(TESTER_NS)
    res = ns_check(TESTER_INTEROP_NS) and res
    test_result(TEST_NAME, res)
