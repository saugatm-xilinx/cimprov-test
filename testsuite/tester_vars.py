import os

TESTER_WMI = False
if 'TESTER_WMI' in os.environ:
    if os.environ['TESTER_WMI'] in ['y', 'Y', 'yes', 'YES']:
        TESTER_WMI = True

cfg_name = None
if 'TESTER_CFG' in os.environ:
    cfg_name = os.environ['TESTER_CFG']

if not cfg_name:
    cfg_name = 'default'

tester_vars_filename = 'tester_vars_' + cfg_name
tester_vars_mod = __import__(tester_vars_filename)

TESTER_HOST = tester_vars_mod.TESTER_HOST
TESTER_USER = tester_vars_mod.TESTER_USER
TESTER_PASSWORD = tester_vars_mod.TESTER_PASSWORD
TESTER_NS = tester_vars_mod.TESTER_NS
TESTER_INTEROP_NS = tester_vars_mod.TESTER_INTEROP_NS
LOGGER_NAME = tester_vars_mod.LOGGER_NAME
LOGGER_FILE = tester_vars_mod.LOGGER_FILE
