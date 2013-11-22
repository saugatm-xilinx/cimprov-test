#!/usr/bin/python

import sys
import logging
from config import *
import config.cfg as cfg

def listTesterCfg():
    """Get the list of tester configurations"""
    
    for name, inst in cfg.TesterConfiguration.cfgList.items():
        print "%(name)-10s %(description)s" % \
              {'name': name, 'description': inst.desc}

def setTesterCfg(name):
    """Set current configuration to configuration with specified name"""
    
    if name in cfg.TesterConfiguration.cfgList:
        cfg.TesterConfiguration.current = cfg.TesterConfiguration.cfgList[name]
        return True
    else:
        print "Unknown tester configuration name: %(name)s" % {'name': name}
        return False

def getTesterCfg():
    """Get current configuration"""
    return cfg.TesterConfiguration.current

def setCfgNICNum():
    """Retrieve Solarflare NICs number via enumerate request to WBEM and
    set corresponding configuration variable.
    """
    try:
        cardList = WBEMConn().getWBEMConn().EnumerateInstanceNames('SF_Card')
    except Exception, e:
        print "Failed to enumerate SF_Card: %(error)s" % {'error': e}
        return False
    
    cfg.TesterConfiguration.NICNum = len(cardList)
    return True

class WBEMConn:
    # Return codes from InvokeMethod opreations
    RC_OK               = 0
    RC_FAIL             = 2
    RC_NO_CHANGE        = 5
    
    def __init__(self, ns=None):
        if getTesterCfg().isWMI:
            import pywmic as wbemcli
        else:
            import pywbem as wbemcli
        if not ns:
            ns = getTesterCfg().vendorNS
       
        self.conn = wbemcli.WBEMConnection(getTesterCfg().host,
            (getTesterCfg().user, getTesterCfg().passwd),
            ns)
    
    def getWBEMConn(self):
        """Return WBEM client connection"""
        return self.conn
   
    @staticmethod
    def uint16(num):
        """Convert number to uint16 WBEM client type if needed"""
        
        if getTesterCfg().isWMI:
            return num
        else:
            from pywbem import Uint16
            return Uint16(num)
    
    @staticmethod
    def isPropRequired(className, propName):
        """Check if property is required for given class."""
        
        if getTesterCfg().isWMI:
            from wmi_unsupp import WMI_UNSUPP_LIST
            
            if className in WMI_UNSUPP_LIST.keys():
                if propName in WMI_UNSUPP_LIST[className]:
                    return False
        return True
   
    @staticmethod
    def isPropPresented(inst, propName):
        """Check if property exists in instance and has non-NULL value"""
        
        return ((getTesterCfg().isWMI and inst.properties[propName]) or
                (not getTesterCfg().isWMI and
                 inst.properties[propName].value != None))
    
    @staticmethod
    def getPropVal(inst, propName):
        """Wrapper for getting property value for different WBEM clients"""
        return (getTesterCfg().isWMI and inst.properties[propName] or
                inst[propName])
    
    @staticmethod
    def nsDelimiter():
        """Return namespace delimiter depending on WBEM client"""
        return getTesterCfg().isWMI and '\\' or '/'
            

class Test:
    testList = {}
    def __init__(self, name, desc, spec, package, func, iter, ns=None):
        self.name = name
        self.desc = desc
        self.spec = spec
        self.package = package
        self.testMain = func
        self.iter = iter
        self.ns = ns
        self.testList[name] = self
    
    @staticmethod
    def paramToBool(s):
        """Convert string parameter to bool or return it as is"""
        
        if type(s) == type(''):
            return s in ['True', 'true', 'T', 't', '1']
        else:
            return s
    
    def logTestIterName(self, params={}):
        """Log test name with test parameters"""
        
        if type(params) == type({}):
            logger = logging.getLogger(getTesterCfg().loggerName)
            logger.info("ITER: %(name)s %(params)s",
                        {'name': self.name, 'params': params})
     
    def logTestStart(self):
        """Log starting part of the test: name and description"""
        
        fmt = "Starting test " + self.name
        fmt = "%-60s" % fmt
        sys.stdout.write(fmt)
        sys.stdout.flush()
        logger = logging.getLogger(getTesterCfg().loggerName)
        logger.info("TEST %(name)s: %(description)s",
                    {'name': self.name, 'description': self.desc})
    
    def logTestEnd(self):
        """Log test result"""
        
        if self.res == None:
            res = 'SKIPPED'
        elif self.res:
            res = 'PASSED'
        else:
            res = 'FAILED'
        print res
        logger = logging.getLogger(getTesterCfg().loggerName)
        logger.info("TEST %(name)s: %(result)s",
                    {'name': self.name, 'result': res})
    
    
    def testPre(self):
        """Do test preparations: create WBEM connection"""
        self.conn = WBEMConn(self.ns).getWBEMConn()

    def testPost(self):
        pass

    def testRun(self, params):
        """Perform actions for test run: preparations, logging, running"""
        self.testPre()
        self.logTestStart()
        self.logTestIterName(params) 
        self.res = self.testMain(self.conn, params)
        self.logTestEnd()
        self.testPost()

class CIMAssocDescr:
    def __init__(self, associationName, associatorName, associatorsCount):
        self.associationName = associationName
        self.associatorName = associatorName
        self.associatorsCount = associatorsCount
    
    def __repr__(self):
        return "%(association)s - %(associator)s x%(count)s" % \
               {'association': self.associationName,
                'associator': self.associatorName,
                'count': self.associatorsCount}

class CIMClassDescr:
    def __init__(self, name, properties, associations, count):
        self.name = name
        self.properties = properties
        self.associations = associations
        self.count = count
    
    def __repr__(self):
        return ("Class %(class)s\nInstance count: %(#)s\nProperty List: %(props)s\n" +
                "Associations: %(assoc)s\n") % \
                {'class': self.name,
                 '#': self.count,
                 'props': self.properties,
                 'assoc': self.associations}

class CountDescr:
    # Checks that we perform for instance counts:
    # - specified positive number
    # - any positive number
    # - any (>=0)
   
    # Special cases
    POSITIVE = -1       # >0
    ANY = -2            # >=0
    
    def __init__(self, num=0):
        self.num = num

    def __str__(self):
        if self.num == CountDescr.POSITIVE:
            return '>0'
        if self.num == CountDescr.ANY:
            return '*'
        else:
            return str(self.num)
    
    def __eq__(self, other):
        if self.num == CountDescr.POSITIVE:
            return other > 0
        if self.num == CountDescr.ANY:
            return True
        else:
            return self.num == other
     
    def __ne__(self, other):
        return not (self == other)

    @staticmethod
    def strInstWord(count):
        """Depending on count return corresponding form of word instance"""
        if type(count) == type(CountDescr()):
            num = count.num
        else:
            num = count
        
        if (num == 1):
            return 'instance'
        else:
            return 'instances'
   
class RequestState:
    # Default timeout in seconds
    TIMEOUT = 0.5
    
    # Known states for RequestedState and EnabledState properties
    ENABLED             = 2
    DISABLED            = 3
    ENABLED_BUT_OFFLINE = 6
    RESET               = 11
    NOT_APPLICABLE      = 12
   
    # String representation of known states
    knownStates = {ENABLED: 'enabled',
                   DISABLED: 'disabled',
                   ENABLED_BUT_OFFLINE: 'enabled but offline',
                   RESET: 'reset',
                   NOT_APPLICABLE: 'not applicable'}
    
    @staticmethod
    def stateToStr(state):
        """Return string representation of state"""
        
        if state in RequestState.knownStates.keys():
            return RequestState.knownStates[state]
        else:
            return str(state)

class JobState:
    # Known Job states:
    NEW                  = 2
    STARTING             = 3
    RUNNING              = 4
    SUSPENDED            = 5
    SHUTTING_DOWN        = 6
    COMPLETED            = 7
    TERMINATED           = 8
    KILLED               = 9
    EXCEPTION            = 10

