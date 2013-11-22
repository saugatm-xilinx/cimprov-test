#!/usr/bin/python
#
# Stress test
#
# fixme: broken for WMI

import thread
import time
import logging
from defs import *
from test_lib import checkNS, assocTraversal, checkEnum

testDesc = "Stress run of base tests"

testSpec = """
Scenario:
 - run enumeration, association and reference tests in parallel
 - check that provider is still alive

Parameters:
 enum          If True, run enumeration thread
 assoc         If True, run association thread 
 refer         If True, run reference thread
 count         Number of test runs in each thread
"""

logger = logging.getLogger(getTesterCfg().loggerName)

def enumThread(data):
    logger.warning("Stress enumerate thread started...\n")
    for i in range(0, data['count']):
        checkEnum(data['conn'])
    logger.warning("Stress enumerate thread finished...\n")
    data['over'] = True

def assocThread(data):
    logger.warning("Stress association thread started...\n")
    for j in range(0, data['count']):
        assocTraversal(data['conn'],
                       {'isAssoc' : True,
                        'namesOnly' : True,
                        'role' : True,
                        'resRole' : True})
    logger.warning("Stress association thread finished...\n")
    data['over'] = True

def referThread(data):
    logger.warning("Stress reference thread started...\n")
    for j in range(0, data['count']):
        assocTraversal(data['conn'],
                       {'isAssoc' : False,
                        'namesOnly' : True,
                        'role' : True})
    logger.warning("Stress reference thread finished...\n")
    data['over'] = True

def stressMain(conn, params):
    log_level = logger.getEffectiveLevel()
    logger.setLevel(logging.WARNING)

    testList = {'enum' : enumThread,
                'assoc' : assocThread,
                'refer' : referThread}
    testData = {}
    for testType, testFunc in testList.items():
        if not params[testType]:
            continue
        testData[testType] = {}
        testData[testType]['over'] = False
        testData[testType]['count'] = params['count']
        testData[testType]['conn'] = WBEMConn().getWBEMConn()
        thread.start_new_thread(testFunc, (testData[testType], ))
    
    over = False
    while not over:
        over = True
        time.sleep(1)
        for test in testList:
            if params[testType]:
                over = over and testData[testType]['over']
    
    logger.setLevel(log_level)
    # Check that we are still alive
    return checkNS(conn, getTesterCfg().vendorNS)

def stressIter(self, params = {}):
    defParam = {'enum': [False, True],
                'assoc': [False, True],
                'refer': [False, True],
                'count': [1]}
    
    for parName in defParam:
        if params.get(parName) != None:
            if parName != 'count':
                defParam[parName] = map(Test.paramToBool, [params[parName]])
            else:
                defParam[parName] = map(int, params[parName])
        
    for enum in defParam['enum']:
        for assoc in defParam['assoc']:
            for refer in defParam['refer']:
                # We want to run at least 2 threads
                if int(enum) + int(assoc) + int(refer) < 2:
                    continue
                for count in defParam['count']:
                    self.testRun({'enum' : enum,
                                  'assoc' : assoc,
                                  'refer' : refer,
                                  'count' : count})


Test(name='stress',
     desc=testDesc,
     spec=testSpec,
     package='intrinsics',
     func=stressMain,
     iter=stressIter)
