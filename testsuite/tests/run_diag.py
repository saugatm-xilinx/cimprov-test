#!/usr/bin/python

from test_lib import checkDiagnosticsRun
from defs import *

testDesc = "Check Diagnostic Service Run"

testSpec = """
Scenario:
    - enumerate DiagnosticTest instances
    - for each DiagnosticTest instace do:
        - get associated via AvailableDiagnosticService instances
        - for each associated ManagementElement instance do:
            - invoke RunDiagnosticService method
            - check returned code and returned Job instance
            - check job state until it finishing or timeout

Parameters:
 timeout        Timeout for Job running in seconds
"""

TIMEOUT = 40

def diagMain(conn, params):
    return checkDiagnosticsRun(conn, timeout=params['timeout'])

def diagIter(self, params={}):
    defParams = {'timeout': [TIMEOUT]}
    
    if params.get('timeout') != None:
        defParams['timeout'] = params['timeout']
    
    for t in defParams['timeout']:
        self.testRun({'timeout': t})


Test(name='run_diag',
     desc=testDesc,
     spec=testSpec,
     package='misc',
     func=diagMain,
     iter=diagIter)

