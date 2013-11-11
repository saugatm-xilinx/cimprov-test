#!/usr/bin/python

from test_lib import checkNS
from defs import *

testDesc = "Check namespaces existance"

testSpec = """
Scenario:
    - check for given namespace path that its name
      exists in namespaces list of its parent
"""

def nsMain(conn, params):
    return checkNS(conn, params['ns'])

def nsIter(self, params={}):
    vendorNS = getTesterCfg().vendorNS
    interopNS = getTesterCfg().interopNS
    self.testRun(params={'ns' : vendorNS})
    if vendorNS != interopNS:
        self.testRun(params={'ns' : interopNS})

Test(name='namespace',
     desc=testDesc,
     spec=testSpec,
     package='misc',
     func=nsMain,
     iter=nsIter)
