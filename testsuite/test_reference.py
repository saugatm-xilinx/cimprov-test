#!/usr/bin/python
#
# Reference test
#
# Scenario:
# - get classes list
# - for each instance of each association class do:
#       - get references
#       - check that references contain this association instance
#       in references list
#
# Parameters:
# names_only    If True, use ReferenceNames instead of References
# role          If True, specify the role for RI/RIN operation
#

from test_lib import assoc_traversal, class_list_fget
from tester_vars import *
from tester_hlpr import *
if TESTER_WMI:
    import pywmic as wbemcli
else:
    import pywbem as wbemcli

def test_function(param = {}):
    """Check association references"""
    TEST_NAME = "reference"
    
    class_list = class_list_fget()
    def_param = {'names_only':  [False, True],
                 'role':        [False, True]}

    for par_name in def_param:
        if param.get(par_name) != None:
            def_param[par_name] = [param[par_name]]
    
    wbemclient = wbemcli.WBEMConnection(TESTER_HOST,
                                       (TESTER_USER, TESTER_PASSWORD),
                                       TESTER_NS)
    for names_only in def_param['names_only']:
        names_only = test_param_str2bool(names_only)
        for role in def_param['role']:
            role = test_param_str2bool(role)
            test_start(TEST_NAME, test_function.__doc__)
            if class_list:
                res = assoc_traversal(wbemclient, class_list, False,
                                      names_only, role)
            else:
                res = None
            test_result(TEST_NAME, res)

