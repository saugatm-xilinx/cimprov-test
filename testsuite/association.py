#!/usr/bin/python

import pywbem
from test_classes import assoc_traversal 

HOST='http://127.0.0.1:5988'
USER=''
PASSWORD=''
NS = 'root/solarflare'

wbemclient = pywbem.WBEMConnection(HOST, (USER, PASSWORD), NS)

print 'Checking Association traversal...'
for is_assoc in [True, False]:
    print is_assoc and "I. Associators" or "II. References"
    for names_only in [True, False]:
        for role in [True, False]:
            for res_role in [True, False]:
                print 'Names only = %s, role = %s, result role = %s' % (
                        names_only, role, res_role)
                assoc_traversal(wbemclient, False, is_assoc, names_only, role, res_role)
                print ''
