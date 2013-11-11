#!/usr/bin/python
#
# Reference test
#
# fixme: broken for WMI

from defs import *
from test_lib import assocTraversal

testDesc="Check association references"

testSpec="""
Scenario:
 - for each instance of each association class do:
       - get references
       - check that references contain this association instance
       in references list

Parameters:
 namesOnly     If True, use ReferenceNames instead of References
 role          If True, specify the role for RI/RIN operation
"""

def refMain(conn, params):
    params['isAssoc'] = False
    return assocTraversal(conn, params)

def refIter(self, params = {}):
    defParam = {'namesOnly':  [False, True],
                'role':       [False, True]}
    
    for parName in defParam:
        if params.get(parName) != None:
            defParam[parName] = map(Test.paramToBool, [params[parName]])
    
    for namesOnly in defParam['namesOnly']:
        for role in defParam['role']:
            self.testRun({'namesOnly' : namesOnly,
                          'role' : role})

Test(name='reference',
     desc=testDesc,
     spec=testSpec,
     package='intrinsics',
     func=refMain,
     iter=refIter)

