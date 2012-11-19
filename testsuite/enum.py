#!/usr/bin/python

import pywbem
from test_classes import enum_check

HOST='http://127.0.0.1:5988'
USER=''
PASSWORD=''
NS = 'root/solarflare'

wbemclient = pywbem.WBEMConnection(HOST, (USER, PASSWORD), NS)

print 'Checking EnumerateInstance and EnumerateInstanceName operations...',
print enum_check(wbemclient) and "FAILED" or "PASSED"

