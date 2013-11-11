#!/usr/bin/python
#
# Enumeration test
#

from test_lib import checkEnum
from defs import *

testDesc="Check EI and EIN enumeration operations"

testSpec = """
Scenario:
- run EnumerateInstance and EnumerateInstanceName operations
- verify that the same number of results are returned for 
  both operations
- check consistance of key properties depending
  on @p checkKeys parameter
"""

def enumMain(conn, params):
    return checkEnum(conn, checkKeys=params['checkKeys'])

def enumIter(self, params={}):
    if getTesterCfg().isWMI:
        self.testRun({'checkKeys' : True})
    else:
        self.testRun({'checkKeys' : False})

Test(name='enumeration',
     desc=testDesc,
     spec=testSpec,
     package='intrinsics',
     func=enumMain,
     iter=enumIter)

