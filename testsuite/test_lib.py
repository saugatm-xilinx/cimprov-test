#!/usr/bin/python

import os
import sys
import string
import time
import logging
from defs import *

logger = logging.getLogger(getTesterCfg().loggerName)

#######################
# Enumeration
#######################

def checkEnum(conn, checkKeys=False):
    """Check EnumerateInstance and EnumerateInstanceName operations
    for specified class list.
    
    conn        WBEM connection
    checkKeys   True if we  need to check keys consistance

    Return True if test passed for all classes and False otherwise
    """
    passed = True
    for cl in getTesterCfg().classList:
        res = 'PASSED'
        logger.info('Checking class %(class)s', {'class' : cl})
        try:
            instList = conn.EnumerateInstances(cl)
            instNameList = conn.EnumerateInstanceNames(cl)
        except Exception, e:
            logger.error('Failed to enumerate class instances:\n%(error)s',
                         {'error' : e})
            passed = False
            logger.info("%(class)s: FAILED", {'class' : cl})
            continue
        
        if len(instList) != len(instNameList):
            logger.error('Number of ei and ein doesn\'t match: ' +
                         '%(eiNum)d != %(einNum)d',
                         {'eiNum': len(instList),
                          'einNum': len(instNameList)})
            passed = False
            logger.info("%(class)s: FAILED", {'class' : cl})
            continue
        
        if len(instList) == 0:
            logger.warn("No instances of %(class)s", {'class' : cl})
        logger.info("Number of instances of %(class)s: %(#)d",
                    {'class': cl, '#': len(instList)})
        
        if checkKeys:
            for idx in range(len(instList)):
                for key in instNameList[idx].properties:
                    try:
                        if (instNameList[idx].properties[key] !=
                                instList[idx].properties[key]):
                            passed = False
                            logger.error("Key property %(key)s doesn't match " + 
                                         "for EI and EIN", {'key': key})
                            res = 'FAILED'
                    except Exception, e:
                        logger.error("Error while comparing keys: %(error)s",
                                     {'error': e})
                        passed = False
                        res = 'FAILED'
        logger.info("%(class)s: %(result)s", {'class': cl, 'result': res})
    return passed

##################
# Namespace
##################

def checkNS(conn, ns):
    # split full namespace path to its name and parent namespace path
    parent, nsName = ns.rsplit(WBEMConn.nsDelimiter(), 1)
    
    logger.info('Checking %(namespace)s existance in %(parent)s...', 
                {'namespace': nsName, 'parent': parent})
    
    # try to find namespace name in the namespaces list of our parent
    try:
        nsList = conn.EnumerateInstanceNames('__Namespace', parent)
    except Exception, e:
        logger.error('Failed to enumerate namespaces names: %(error)s',
                     {'error': e})
        return False
    for inst in nsList:
        instName = WBEMConn.getPropVal(inst, 'Name')
        if instName.upper() == nsName.upper():
            return True
    return False

################
# Profiles
################

def isProfileRegistered(conn, profileName):
    try:
        instList = conn.EnumerateInstances('SF_RegisteredProfile',
                                            getTesterCfg().interopNS)
    except Exception, e:
        logger.error('Failed to enumerate SF_RegisteredProfile instances: %(error)s',
                     {'error': e})
        return False
    for inst in instList:
        checkName = WBEMConn.getPropVal(inst, 'RegisteredName')
        if checkName == profileName:
            return True
    return False

def checkProfile(conn, profList, descList):
    globalPassed = True
    
    logger.info("Checking profiles registration...")
    for profile in profList:
        res = isProfileRegistered(conn, profile)
        if not res:
            globalPassed = False
        logger.info("Profile %(profile)s: %(result)s",
                    {'profile': profile,
                     'result': res and "OK" or "ENOENT"})
    
    logger.info("Checking profile classes...")

    for cimClass in descList:
        logger.info("Trying to enumerate instances of %(class)s",
                    {'class': cimClass.name})
        classPassed = True
        try:
            instList = conn.EnumerateInstances(cimClass.name)
        except Exception, e:
            logger.error("Failed to enumerate instances:\n%s", e)
            globalPassed = False
            logger.info("Class %(class)s FAILED",
                        {'class': cimClass.name})
            continue
        
        instCount = 0
        for inst in instList:
            instCount += 1
            logger.info("Instance #%(#)s", {'#': instCount})

            # Checking properties
            for reqProp in cimClass.properties:
                if not WBEMConn.isPropRequired(cimClass.name, reqProp):
                    continue
                logger.info("Checking property %(prop)s",
                            {'prop': reqProp})
                try:
                    if not WBEMConn.isPropPresented(inst, reqProp):
                        logger.error("Required property %(prop)s " +
                                      "has NULL value", {'prop': reqProp})
                except Exception, e:
                    logger.error(("Failed to get property " +
                                  "%(prop)s of %(instance)s:\n%(error)s"),
                                 {'prop': reqProp,
                                  'instance': inst,
                                  'error': e})
                    classPassed = False
                    continue

            # Checking associations
            for cimAssoc in cimClass.associations:
                logger.info("Checking association instance counts:" +
                            " %(assocDescr)s", {'assocDescr': cimAssoc})
                if getTesterCfg().isWMI:
                    logger.warn("Unsupported for WMI")
                    break
                try:
                    associatorsList = conn.Associators(inst.path,
                        AssocClass=cimAssoc.associationName,
                        ResultClass=cimAssoc.associatorName)
                except Exception, e:
                    logger.error("Failed to get associators:\n%s", e)
                    classPassed = False
                    continue

                associatorsCount = len(associatorsList)
                if associatorsCount != cimAssoc.associatorsCount:
                    logger.error("%(title)s: unexpected assotiators number: " +
                                 "%(count)s instead of %(expected)s",
                                 {'title': cimAssoc,
                                  'count': associatorsCount,
                                  'expected': cimAssoc.associatorsCount})
                    classPassed = False
                continue
        
        total = "%(class)s: expected %(count)s %(inst)s" % {
                        'class': cimClass.name,
                        'count': cimClass.count,
                        'inst': CountDescr.strInstWord(cimClass.count)}
        logger.info(total)
        
        if (cimClass.count != instCount):
            logger.error("%(class)s: unexpected instance count: " +
                         "%(count)s instead of %(expected)s",
                         {'class': cimClass.name,
                          'count': instCount,
                          'expected': cimClass.count})
            classPassed = False

        if not classPassed:
            globalPassed = False

        logger.info("Class %(class)s %(result)s",
                    {'class': cimClass.name,
                     'result': classPassed and 'PASSED' or 'FAILED'})
    return globalPassed

######################
# Request state change
######################
def changeState(conn, instPath, state):
    try:
        (rval, out_params) = conn.InvokeMethod('RequestStateChange',
                                    instPath, RequestedState=state)
    except Exception, e:
        logger.error("Failed to change state of %(instance)s to " +
                     "%(state)s:\n%(error)s",
                     {'instance': instPath, 'state': state, 'error': e})
        (rval, outParams) = (WBEMConn.RC_FAIL, {})
    
    #TODO: do we need outParams ?
    return rval

def checkReqStateChange(conn, className, state, timeout):
    passed = True
    instList = conn.EnumerateInstances(className)
    instNum = 0
    for inst in instList:
        instNum += 1
        logger.info("instance #%(#)s", {'#': instNum})
        # Change state
        rc = changeState(conn, inst.path,
                         wbemcli.Uint16(state)) #FIXME: <-function
        if rc != WBEMConn.RC_OK:
            passed = False
            continue
        
        # Wait for the state update
        time.sleep(timeout)
        
        # Check oper state
        newInst = conn.GetInstance(inst.path)
        if newInst['EnabledState'] != state:
            logger.error("Oper state of instance %(inst)s is unexpected: " +
                         "%(state)s instead of %(expected)s",
                         {'inst': inst.path,
                          'state': newInst['EnabledState'],
                          'expected': state})
            passed = False
            continue
        
        # Restore old state
        rc = changeState(conn, inst.path,
                         wbemcli.Uint16(inst['EnabledState'])) #fixme!
        if rc != WBEMConn.RC_OK:
            passed = False
    return passed

#######################
# Association traversal
#######################

def checkAssociators(cli, num, ref, cl, params):
    func = params['namesOnly'] and cli.AssociatorNames or cli.Associators
    try:
        ref[num]['assoc'] = func(ref[num]['path'],
                                 AssocClass=cl,
                                 ResultClass=ref[1 - num]['inst'].classname,
                                 Role=params['role'] and
                                      ref[num]['name'] or '',
                                 ResultRole=params['resRole'] and
                                            ref[1 - num]['name'] or '')
    except:
        return False
    for assoc in ref[num]['assoc']:
        if string.find(str(params['namesOnly'] and assoc or assoc.path),
                       str(ref[1 - num]['inst'].path)) != -1:
            return True
    return False

def checkReferences(cli, num, ref, cl, params):
    func = params['namesOnly'] and cli.ReferenceNames or cli.References
    try:
        ref[num]['assoc'] = func(ref[num]['path'],
                                 ResultClass=cl,
                                 Role=params['role'] and
                                      ref[num]['name'] or '')
    except:
        return False
    for assoc in ref[num]['assoc']:
        if string.find(str(params['namesOnly'] and assoc or assoc.path),
                       str(ref[1 - num]['inst'].path)) != -1:
            return True
    return False

def assocTraversal(conn, params):
    '''Perform association/reference traversal.

    conn       WBEM connection
    isAssoc    True for association, False for reference check
    namesOnly  If True, use AssociatorNames instead of Associators
    role       If True, specify the role for required operation
    resRole    If True, specify the result role for required operation

    Return True if test passed for all association and False otherwise
    '''
    globalPassed = True
    
    for cl in getTesterCfg().classList:
        passed = True
        cl_obj = conn.GetClass(cl, LocalOnly=False, IncludeQualifiers=True)
        if not ('Association' in cl_obj.qualifiers):
            continue
        
        logger.info("Checking association class %(class)s", {'class': cl})
        try:
            instList = conn.EnumerateInstances(cl)
        except Exception, e:
            logger.error("Failed to enumerate instances of %(class)s "
                         "class:\n%(error)s", {'class': cl, 'error': e})
        for inst in instList:
            ref = []
            crossNs = False
            logger.info("Checking %(class)s instance %(instance)s",
                        {'class': cl, 'instance': inst})
            for key, prop in inst.properties.items():
                if prop.type == 'reference':
                    ref.append({'path': prop.value, 'name': key})

            # We have only 2-way associations
            if len(ref) != 2:
                logger.error("Association instance %(instance)s has " +
                             "invalid count of references (%(#)s)",
                             {'instance': inst, '#': len(ref)})
            
            for r in ref:
                logger.info("Get reference instance:\n%s", r)
                try:
                    r['inst'] = conn.GetInstance(r['path'])
                except Exception, e:
                    logger.error("Failed to get reference instance:\n" +
                                 "%(error)s", {'error': e})
                    logger.info("Namespace is %(namespace)s\n",
                                {'namespace': r['path'].namespace})
                    passed = False
                    globalPassed = False
                       
            if not passed:
                break
            
            func = params['isAssoc'] and  checkAssociators or checkReferences
            passed = func(conn, 0, ref, cl, params)
            passed = passed and func(conn, 1, ref, cl, params)
            continue
        
        logger.info("%(class)s: %(result)s",
                    {'class': cl,
                     'result': passed and 'PASSED' or 'FAILED'})
        continue
    
    return globalPassed


def checkAssociatorsCount(conn, classList):
    passed = True
    
    for className in classList:
        try:
            res = conn.AssociatorsCheck(className)
        except Exception, e:
            logger.error("Class %(class)s associators exception: %(error)s",
                         {'class': className, 'error': e})
            res = False
            passed = False
        logger.info("Associators check of %(class)s result: %(result)s",
                    {'class': className,
                     'result': res and 'PASSED' or 'FAILED'})
    return passed

