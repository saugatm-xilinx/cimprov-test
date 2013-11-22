#!/usr/bin/python
#
# fixme: broken for WMI

from defs import *
from test_lib import checkReqStateChange

testDesc = "Check RequestStateChange method"

testSpec = """
Scenario:
    - for each instance of each class from the list do:
        - get instance property state value
        - invoke RequestStateChange method
        - restore instance property state

Parameters:
 class          Class name
 state          Requested state
 timeout        Timeout in seconds
"""

classList = ['SF_EthernetPort', 'SF_RecordLog', 'SF_DiagnosticLog']
stateList = [RequestState.ENABLED,
             RequestState.DISABLED,
             RequestState.RESET]

def reqStateMain(conn, params):
    return checkReqStateChange(conn, params)

def reqStateIter(self, params = {}):
    defParam = {'class': classList,
                'state': stateList,
                'timeout': [RequestState.TIMEOUT]}
    
    for parName in defParam:
        if params.get(parName) != None:
            if parName == 'state':
                defParam[parName] = map(int, [params[parName]])
            else:
                defParam[parName] = [params[parName]]
    
    for cl in defParam['class']:
        for st in defParam['state']:
            for t in defParam['timeout']:
                self.testRun({'class': cl, 'state': st, 'timeout': t})


Test(name='req_state_change',
     desc=testDesc,
     spec=testSpec,
     package='misc',
     func=reqStateMain,
     iter=reqStateIter)
