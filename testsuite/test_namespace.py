#!/usr/bin/python

from test_lib import ns_check
from tester_vars import *
from tester_hlpr import *

def test_function(param = {}):
    """Check namespaces existance"""
    TEST_NAME = "namespace"

    test_start(TEST_NAME, test_function.__doc__)
    spl = TESTER_NS.rsplit("/", 1)
    res = ns_check(spl[0], spl[1])
    res = ns_check("root", "PG_InterOp") and res
    test_result(TEST_NAME, res)
