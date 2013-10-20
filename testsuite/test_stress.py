#!/usr/bin/python
#
# Stress test
#
# Scenario:
#
# - get classes list
# - run enumeration, association and reference tests in parallel
# - check that provider is still alive
#
# Parameters:
# enum          If True, run enumeration thread
# assoc         If True, run association thread 
# refer         If True, run reference thread
# count         Number of test runs in each thread
#

import thread
import time
import logging
from test_lib import ns_check, class_list_fget, assoc_traversal, enum_check
from tester_vars import *
from tester_hlpr import *
if TESTER_WMI:
    import pywmic as wbemcli
else:
    import pywbem as wbemcli

logger = logging.getLogger(LOGGER_NAME)

def enum_thread(data):
    logger.warning("Stress enumerate thread started...\n")
    for i in range(0, data["count"]):
        enum_check(data["cli"], data["classes"])
    logger.warning("Stress enumerate thread finished...\n")
    data["over"] = True

def assoc_thread(data):
    logger.warning("Stress association thread started...\n")
    for j in range(0, data["count"]):
        assoc_traversal(data["cli"], data["classes"], True,
                        True, True, True)
    logger.warning("Stress association thread finished...\n")
    data["over"] = True

def refer_thread(data):
    logger.warning("Stress reference thread started...\n")
    for j in range(0, data["count"]):
        assoc_traversal(data["cli"], data["classes"], False,
                        True, True)
    logger.warning("Stress reference thread finished...\n")
    data["over"] = True

def test_function(param = {}):
    """Stress run of base tests"""
    TEST_NAME = "stress"

    def_param = {"enum": [False, True],
                 "assoc": [False, True],
                 "refer": [False, True],
                 "count": [1]}
    
    for par_name in def_param:
        if param.get(par_name) != None:
            def_param[par_name] = [param[par_name]]
    
    test_list = {
        "enum" : {"func" : enum_thread},
        "assoc" : {"func" : assoc_thread},
        "refer" : {"func" : refer_thread}
    }

    class_list = class_list_fget()

    #TODO: fix hardcode
    for enum in def_param["enum"]:
        enum = test_param_str2bool(enum)
        test_list["enum"]["run"] = enum
        for assoc in def_param["assoc"]:
            assoc = test_param_str2bool(assoc)
            test_list["assoc"]["run"] = assoc
            for refer in def_param["refer"]:
                refer = test_param_str2bool(refer)
                test_list["refer"]["run"] = refer

                if not enum and not assoc and not refer:
                    continue

                for count in def_param["count"]:
                    count = int(count)
                    test_start(TEST_NAME, test_function.__doc__)
                    logger.info("Parameters:\nenum = %s\nassoc = %s" +
                                "\nrefer = %s\ncount = %s",
                                enum, assoc, refer, count)
                    if class_list == None:
                        test_result(TEST_NAME, None)
                        continue
                    
                    log_level = logger.getEffectiveLevel()
                    logger.setLevel(logging.WARNING)
                    
                    for test, data in test_list.items():
                        if not data["run"]:
                            continue
                        data["over"] = False
                        data["count"] = count
                        data["classes"] = class_list
                        data["cli"] = wbemcli.WBEMConnection(TESTER_HOST,
                                            (TESTER_USER, TESTER_PASSWORD),
                                            TESTER_NS)
                        thread.start_new_thread(data["func"], (data, ))
                    over = False
                    while not over:
                        over = True
                        time.sleep(1)
                        for test, data in test_list.items():
                            if data["run"]:
                                over = over and data["over"]

                    logger.setLevel(log_level)
                    # Check that we are still alive
                    test_result(TEST_NAME, ns_check(TESTER_NS))

