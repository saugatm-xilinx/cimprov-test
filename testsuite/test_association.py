#!/usr/bin/python
#
# Association test
#
# Scenario:
# - get classes list
# - for each instance of each association class do:
#       - get references
#       - get associators of references
#       - check that associators contain corersponding references
# 
# Parameters:
# names_only    If True, use AssociatorNames instead of Associators
# role          If True, specify the role for AI/AIN operation
# res_role      If True, specify the result role for AI/AIN operation
#


import pywbem
from test_lib import assoc_traversal, class_list_fget
from tester_vars import *
from tester_hlpr import *

def test_function(param = {}):
    """Check associations traversal"""
    TEST_NAME = "association"
    
    class_list = class_list_fget()
    def_param = {'names_only':  [False, True],
                 'role':        [False, True],
                 'res_role':    [False, True]}
    
    for par_name in def_param:
        if param.get(par_name) != None:
            def_param[par_name] = [param[par_name]]
    
    wbemclient = pywbem.WBEMConnection(TESTER_HOST,
                                       (TESTER_USER, TESTER_PASSWORD),
                                       TESTER_NS)
    for names_only in def_param['names_only']:
        names_only = test_param_str2bool(names_only)
        for role in def_param['role']:
            role = test_param_str2bool(role)
            for res_role in def_param['res_role']:
                res_role = test_param_str2bool(res_role)
                test_start(TEST_NAME, test_function.__doc__)
                if class_list:
                    res = assoc_traversal(wbemclient, class_list, True,
                                          names_only, role, res_role)
                else:
                    res = None
                test_result(TEST_NAME, res)
    
