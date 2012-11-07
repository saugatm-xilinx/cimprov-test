#!/usr/bin/python

from test_classes import req_state_change_check

REQSTATE_DISABLE      = 3
REQSTATE_ENABLE       = 2
REQSTATE_RESET        = 11

NS = 'root/solarflare'
TIMEOUT = 0.5
CLASSES_LIST = ['CIM_NetworkPort', 'CIM_RecordLog', 'CIM_DiagnosticLog']

for name in CLASSES_LIST:
    print 'Checking RequestStateChange() for %s class...' % name
    print 'Disable:'
    req_state_change_check(NS, name, REQSTATE_DISABLE, TIMEOUT)
    print 'Enable:'
    req_state_change_check(NS, name, REQSTATE_ENABLE, TIMEOUT)
    print 'Reset:'
    req_state_change_check(NS, name, REQSTATE_RESET, TIMEOUT)
    print ''
