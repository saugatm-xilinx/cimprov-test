#!/usr/bin/python

from test_lib import req_state_change_check
from tester_vars import *
from tester_hlpr import *

REQ_STATES = {
    "disable" : 3,
    "enable" : 2,
    #"reset" : 11
}

TIMEOUT = 0.5
CLASSES_LIST = ["CIM_NetworkPort", "CIM_RecordLog", "CIM_DiagnosticLog"]

def test_function(param = {}):
    """RequestStateChange() testing"""
    TEST_NAME = "req_state_change"
    
    logger = logging.getLogger(LOGGER_NAME)
    test_start(TEST_NAME, test_function.__doc__)
    res = True
    for cl in CLASSES_LIST:
        logger.info("Checking RequestStateChange() " +
                 "for {0} class...".format(cl))
        for state_name, state_code in REQ_STATES.items():
            passed = True
            logger.info("Checking " + state_name)
            passed = req_state_change_check(TESTER_NS, cl,
                                            state_code, TIMEOUT)
            res = res and passed
            logger.info("State {0}: {1}".format(
                        state_name, passed and "PASSED" or "FAILED"))
    test_result(TEST_NAME, res)