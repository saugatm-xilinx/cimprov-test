#!/usr/bin/python

import sys
import logging
from tester_vars import *

def test_start(name, descr):
    logger = logging.getLogger(LOGGER_NAME)
    fmt = "Starting test " + name
    fmt = "{0:60s}".format(fmt)
    sys.stdout.write(fmt)
    logger.info("TEST {0}: {1}".format(name, descr))

def test_result(name, res):
    logger = logging.getLogger(LOGGER_NAME)
    if res == None:
        res = "SKIPPED"
    elif res:
        res = "PASSED"
    else:
        res = "FAILED"
    print res
    logger.info("TEST {0}: {1}".format(name, res))

def test_param_str2bool(s):
    if type(s) == type(""):
        return s in ["True", "true", "T", "t", "1"]
    else:
        return s
