#!/usr/bin/python

import logging
import sys
import re
import getopt
from defs import *

testerOptList = ["help", "cfg=", "list-cfg", "list-test", "print-test-spec"]
testerFakeRun = False
testerPrintSpec = False

def usage():
    print """
USAGE: tester.py [options] [package|test[:param=value[,param=value]*]]*
Options:
 --help                Display this information
 --cfg=<CFG>           Configuration to be used
 --list-cfg            Print the list of known configurations
 --list-test           Print the list of available tests
 --print-test-spec     Print tests specification instead of running
"""

# Parse command line arguments
try:
    opts, args = getopt.getopt(sys.argv[1:], "h", testerOptList)
except getopt.GetoptError, e:
    print "Failed to parse command-line options: %(error)s" % {'error': e}
    usage()
    sys.exit(1)

for opt, arg in opts:
    if opt in ("-h", "--help"):
        usage()
        sys.exit(0)
    elif opt == "--cfg":
        # Set configuration
        if not setTesterCfg(arg):
            sys.exit(1)
    elif opt == "--list-cfg":
        listTesterCfg()
        sys.exit(0)
    elif opt == "--list-test":
        testerFakeRun = True
    elif opt == "--print-test-spec":
        testerFakeRun = True
        testerPrintSpec = True

if not getTesterCfg():
    print """
Please specify tester configuration.
Use --list-cfg to get full list of known configurations
"""
    sys.exit(1)

# Set up logger
logger = logging.getLogger(getTesterCfg().loggerName)
hdlr = logging.FileHandler(getTesterCfg().loggerFile, mode="w")
formatter = logging.Formatter("%(levelname)s %(asctime)s \n%(message)s\n")
hdlr.setFormatter(formatter)
logger.addHandler(hdlr) 
logger.setLevel(logging.INFO)

# Set number of NICs
if not setCfgNICNum():
    hdlr.close()
    sys.exit(1)

# Set classes file
getTesterCfg().setClassList()

# Get available tests list
from tests import *

# Handle tests list
packageList = {}
for testName, test in Test.testList.items():
    if test.package in packageList:
        packageList[test.package][testName] = {}
    else:
        packageList[test.package] = {testName : {}}

# Handle tests parameters
testrunList = {}
for arg in args:
    testSpec = arg.split(':', 1)
    testName = testSpec[0]
    if len(testSpec) > 1:
        spl = re.split('[=,]', testSpec[1])
        testParams = dict(zip(spl[0::2], spl[1::2]))
    else:
        # Handle package case:
        if testName in packageList:
            testrunList[testName] = {}
            continue
        
        testParams = {}
    found = False
    for package, testList in packageList.items():
        if testName in testList:
            found = True
            if package in testrunList:
                testrunList[package][testName] = testParams
            else:
                testrunList[package] = {testName : testParams}
    if not found:
        print "Unknown test/pakage name: %(name)s" % \
              {'name': testName}
        hdlr.close()
        sys.exit(1)
        
if not testrunList:
    testrunList = packageList

if not testerFakeRun:
    logger.info("Test run list: " + str(testrunList))

# List tests

# Run tests
for package, testListParams in testrunList.items():
    if testerFakeRun:
        print "Package:", package
    else:
        print "Starting package", package
    
    if not testListParams:
        # Using default parameters
        testListParams = packageList[package]

    for testName in testListParams:
        test = Test.testList[testName]

        if testerFakeRun:
            print "TEST %(test)-20s %(description)s" % \
                  {'test': testName,
                   'description': test.desc}
            if testerPrintSpec:
                print test.spec
        else:
            # Iterate test through given parameters
            test.iter(test, testListParams[testName])
    if testerFakeRun:
        print ""
    else:
        print "Done package", package

hdlr.close()
