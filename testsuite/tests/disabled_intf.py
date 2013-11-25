#!/usr/bin/python
#
# Disabled interface test
#
# fixme: support for Linux

from test_lib import checkDisabledIntf
from defs import *

testDesc="Test instances with disabled interface"

testSpec = """
Scenario:
- for each SF_EthernetPort instance:
    - get DeviceID
    - disable corresponding interface
    - check that we still can get the same instances
    - enable corresponding interface
"""

def disabledIntfMain(conn, params):
    return checkDisabledIntf(conn)

def disabledIntfIter(self, params={}):
    if getTesterCfg().isWMI:
        self.testRun(params)

Test(name='disabled_intf',
     desc=testDesc,
     spec=testSpec,
     package='misc',
     func=disabledIntfMain,
     iter=disabledIntfIter)

