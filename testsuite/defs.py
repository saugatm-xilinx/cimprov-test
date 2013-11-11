#!/usr/bin/python

import sys
import logging
from config import *
import config.cfg as cfg

def listTesterCfg():
    for name, inst in cfg.TesterConfiguration.cfgList.items():
        print "%(name)-10s %(description)s" % \
              {'name': name, 'description': inst.desc}

def setTesterCfg(name):
    if name in cfg.TesterConfiguration.cfgList:
        cfg.TesterConfiguration.current = cfg.TesterConfiguration.cfgList[name]
        return True
    else:
        print "Unknown tester configuration name: %(name)s" % {'name': name}
        return False

def getTesterCfg():
    return cfg.TesterConfiguration.current

def setCfgNICNum():
    try:
        cardList = WBEMConn().getWBEMConn().EnumerateInstanceNames('SF_Card')
    except Exception, e:
        print "Failed to enumerate SF_Card: %(error)s" % {'error': e}
        return False
    
    cfg.TesterConfiguration.NICNum = len(cardList)
    return True

class WBEMConn:
    # Return codes from InvokeMethod opreations
    RC_OK   = 0
    RC_FAIL = 2
    
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
        return self.conn
   
    @staticmethod
    def isPropRequired(className, propName):
        if getTesterCfg().isWMI:
            from wmi_unsupp import WMI_UNSUPP_LIST
            
            if className in WMI_UNSUPP_LIST.keys():
                if propName in WMI_UNSUPP_LIST[className]:
                    return False
        return True
    
    @staticmethod
    def isPropPresented(inst, propName):
        return ((getTesterCfg().isWMI and inst.properties[propName]) or
                (not getTesterCfg().isWMI and
                 inst.properties[propName].value != None))
    
    @staticmethod
    def getPropVal(inst, propName):
        return (getTesterCfg().isWMI and inst.properties[propName] or
                inst[propName])
    
    @staticmethod
    def nsDelimiter():
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
        if type(s) == type(''):
            return s in ['True', 'true', 'T', 't', '1']
        else:
            return s
    
    def logTestIterName(self, params={}):
        if type(params) == type({}):
            logger = logging.getLogger(getTesterCfg().loggerName)
            logger.info("ITER: %(name)s %(params)s",
                        {'name': self.name, 'params': params})
     
    def logTestStart(self):
        fmt = "Starting test " + self.name
        fmt = "%-60s" % fmt
        sys.stdout.write(fmt)
        sys.stdout.flush()
        logger = logging.getLogger(getTesterCfg().loggerName)
        logger.info("TEST %(name)s: %(description)s",
                    {'name': self.name, 'description': self.desc})
    
    def logTestEnd(self):
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
        self.conn = WBEMConn(self.ns).getWBEMConn()

    def testPost(self):
        pass

    def testRun(self, params):
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
    POSITIVE = -1
    ANY = -2
    
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
        if type(count) == type(CountDescr()):
            num = count.num
        else:
            num = count
        
        if (num == 1):
            return 'instance'
        else:
            return 'instances'
   
   
