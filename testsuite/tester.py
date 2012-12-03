#!/usr/bin/python

import logging
import sys
import re
from tester_vars import *

TESTER_PACKAGE_LIST = {
    "intrinsics" : {
        "enumeration" : {},
        "association" : {},
        "reference" : {},
        "stress": {}},
    "profile" : {
        "phys_net_profile" : {},
        "sw_profile" : {},
        "reg_profile" : {},
        "diag_profile" : {}},
    "misc" : {
        "req_state_change" : {},
        "namespace" : {}}
}

# Set up logger
logger = logging.getLogger(LOGGER_NAME)
hdlr = logging.FileHandler("/tmp/tester.log", mode="w")
formatter = logging.Formatter("%(levelname)s %(asctime)s \n%(message)s\n")
hdlr.setFormatter(formatter)
logger.addHandler(hdlr) 
logger.setLevel(logging.INFO)

# Handle tests parameters
testrun_list = {}
if len(sys.argv) > 1:
    for arg in sys.argv[1:]:
        test_spec = arg.split(':', 1)
        test_name = test_spec[0]
        if len(test_spec) > 1:
            spl = re.split("[=,]", test_spec[1])
            test_params = dict(zip(spl[0::2], spl[1::2]))
        else:
            test_params = {}
        for package, test_list in TESTER_PACKAGE_LIST.items():
            if test_name in test_list:
                if package in testrun_list:
                    testrun_list[package][test_name] = test_params
                else:
                    testrun_list[package] = {test_name : test_params}
        
if not testrun_list:
    testrun_list = TESTER_PACKAGE_LIST

logger.info("Test run list: " + str(testrun_list))

# Run tests
for package, test_list in testrun_list.items():
    print "Starting package", package
    for test in test_list:
        test_name = 'test_' + test
        test_mod = __import__(test_name)
        test_mod.test_function(test_list[test])
    print "Done package", package

hdlr.close()
