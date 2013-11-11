#!/usr/bin/python
#
# Association test
#
# fixme: broken for WMI

from defs import *
from test_lib import assocTraversal

testDesc="Check associations traversal"

testSpec = """
Scenario:
 - for each instance of each association class do:
       - get references
       - get associators of references
       - check that associators contain corersponding references
 
Parameters:
 namesOnly    If True, use AssociatorNames instead of Associators
 role         If True, specify the role for AI/AIN operation
 resRole      If True, specify the result role for AI/AIN operation
"""

def assocMain(conn, params): 
    params['isAssoc'] = True
    return assocTraversal(conn, params)

def assocIter(self, params = {}):    
    defParam = {'namesOnly':  [False, True],
                'role':       [False, True],
                'resRole':    [False, True]}
    
    for parName in defParam:
        if params.get(parName) != None:
            defParam[parName] = map(Test.paramToBool, [params[parName]])
    
    for namesOnly in defParam['namesOnly']:
        for role in defParam['role']:
            for resRole in defParam['resRole']:
                self.testRun({'namesOnly' : namesOnly,
                              'role' : role,
                              'resRole' : resRole})

Test(name='association',
     desc=testDesc,
     spec=testSpec,
     package='intrinsics',
     func=assocMain,
     iter=assocIter)

