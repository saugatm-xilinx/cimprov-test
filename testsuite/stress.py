#!/usr/bin/python

import pywbem
import thread
import time
from test_classes import enum_check
from test_classes import assoc_traversal
from test_classes import ns_check

HOST='http://127.0.0.1:5988'
USER=''
PASSWORD=''
NS = 'root/solarflare'
STRESS_COUNT = 100

enum_over = False
assoc_over = False

def enum_thread(cli):
    global enum_over
    #print 'Stress enumerate testing...\n'
    for i in range(0, STRESS_COUNT):
        enum_check(cli)
    #print "Enum is over!"
    enum_over = True

def assoc_thread(cli):
    global assoc_over
    #print 'Stress association traversal testing...\n'
    for j in range(0, STRESS_COUNT):
        assoc_traversal(cli, True, True, True, True, True)
    #print "Assoc is over!"
    assoc_over = True

enum_cli = pywbem.WBEMConnection(HOST, (USER, PASSWORD), NS)
assoc_cli = pywbem.WBEMConnection(HOST, (USER, PASSWORD), NS)

thread.start_new_thread(enum_thread, (enum_cli, ))
thread.start_new_thread(assoc_thread, (assoc_cli, ))

while not enum_over or not assoc_over:
    time.sleep(1)

# Check that we are still alive
ns_check('root', 'solarflare')

