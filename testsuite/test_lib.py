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
    """For each class:
        - check EnumerateInstance operation
        - check EnumerateInstanceName operation
        - check that number of instances match for these operations
        - check uniqueness of instance names
        - if @p checkKeys is True, check keys consistance
    
    conn        WBEM connection
    checkKeys   True if we  need to check keys consistance

    Return True if test passed for all classes and False otherwise
    """
    passed = True
    for cl in getTesterCfg().classList:
        res = 'PASSED'
        logger.info('Checking class %(class)s', {'class' : cl})
        
        # Check EI and EIN operations
        try:
            instList = conn.EnumerateInstances(cl)
            instNameList = conn.EnumerateInstanceNames(cl)
        except Exception, e:
            logger.error('Failed to enumerate class instances:\n%(error)s',
                         {'error' : e})
            passed = False
            logger.info("%(class)s: FAILED", {'class' : cl})
            continue
        
        # Check that EI and EIN operations returned the same
        # number of instances
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
        
        # Check uniqness of instances
        uniqueList = []
        for inst in instNameList:
            instStr = str(inst)
            logger.info("%s", instStr)
            if instStr not in uniqueList:
                uniqueList.append(instStr)
            else:
                logger.error("Found instances with duplicated object "
                             "paths %(path)s", {'inst': instStr})
                passed = False
        
        #  Check that keys are non-empty if needed
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
    """Check that name of given namespace exists in the list of
    parent namespace childs.

    conn        WBEM connection
    ns          Namespace path

    Return True if namespace name exists and False otherwise
    """
    
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
    """Check that there is a profile instance with given profile Name.

    conn                WBEM connection
    profileName         Profile name

    Return True if corresponding instance exists and False otherwise
    """
    
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
    """Check that profiles from @p profList are registered and that
    all instances of profile classes descibed in @p descList specification
    pass all requirements from this specification.

    conn        WBEM connection
    profList    List of profile names
    descList    Specification of profile classes

    Return True if corresponding instance exists and False otherwise
    """
    
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
    """Request state change for given instance to given state.

    conn        WBEM connection
    instPath    Instance path
    state       Requested state

    Return True if operation succeeded and False otherwise
    """
    try:
        (rval, out_params) = conn.InvokeMethod('RequestStateChange',
                                               instPath,
                                               RequestedState=state)
    except Exception, e:
        logger.error("Failed to change state of %(instance)s to " +
                     "%(state)s:\n%(error)s",
                     {'instance': instPath, 'state': state, 'error': e})
        (rval, outParams) = (WBEMConn.RC_FAIL, {})
        
    #fixme: do we need outParams ?
    return rval

def checkReqStateChange(conn, params):
    """Check RequestStateChange method for each instance of given class.
    
    conn        WBEM connection
    class       Class name
    state       Requested state
    timeout     Timeout

    Return True if method succeeded for all instances and False overwise
    """
    
    passed = True
    instList = conn.EnumerateInstances(params['class'])
    instNum = 0
    for inst in instList:
        instNum += 1
        logger.info("instance #%(#)s", {'#': instNum})
        # Change state
        rc = changeState(conn, inst.path,
                         WBEMConn.uint16(params['state']))
        if (rc != WBEMConn.RC_OK and rc != WBEMConn.RC_NO_CHANGE):
            passed = False
            continue
        
        # Wait for the state update
        time.sleep(params['timeout'])
        
        newInst = conn.GetInstance(inst.path)
        
        # Check requested state
        reqState = newInst['RequestedState']
        if reqState != params['state']:
            logger.error("Requested state of instance %(inst)s" +
                         "is %(valCur)s(%(strCur)s), " +
                         "expected: %(valExp)s(%(strExp)s)",
                         {'inst': inst.path,
                          'valCur': reqState,
                          'strCur': RequestState.stateToStr(reqState),
                          'valExp': params['state'],
                          'strExp': RequestState.stateToStr(params['state'])
                         })
            passed = False
            continue
        
        # Check oper state
        newState = newInst['EnabledState']
        if ((newState != params['state']) and
            (newState != RequestState.ENABLED_BUT_OFFLINE or
             params['state'] != RequestState.ENABLED) and
            (params['state'] != RequestState.RESET)):
            logger.error("Oper state of instance %(inst)s " +
                         "is unexpected: %(valCur)s(%(strCur)) " +
                         "instead of %(valExp)s(%(strExp)s)",
                         {'inst': inst.path,
                          'valCur': newState,
                          'strCur': RequestState.stateToStr(newState),
                          'valExp': params['state'],
                          'strExp': RequestState.stateToStr(params['state'])
                         })
            passed = False
            continue
        
        # Restore old state
        rc = changeState(conn, inst.path,
                         WBEMConn.uint16(inst['EnabledState']))
        if (rc != WBEMConn.RC_OK and rc != WBEMConn.RC_NO_CHANGE):
            passed = False
    return passed

#######################
# Association traversal
#######################

def checkAssociators(conn, num, ref, cl, params):
    """Check that specified reference instance of association instance
    contains in associators list paired reference instance.

    conn        WBEM connection
    num         Number of reference instance (0 or 1)
    ref         List with two references of association instance
    cl          Association class name
    params      Associator method parameters

    Return True if check passed and False overwise
    """
    
    func = params['namesOnly'] and conn.AssociatorNames or conn.Associators
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

def checkReferences(conn, num, ref, cl, params):
    """Check that specified reference instance of association instance
    contains in references list association instance with path containing 
    paired reference instance path.

    conn        WBEM connection
    num         Number of reference instance (0 or 1)
    ref         List with two references of association instance
    cl          Association class name
    params      Reference method parameters

    Return True if check passed and False overwise
    """
    
    func = params['namesOnly'] and conn.ReferenceNames or conn.References
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
    """Perform association/reference traversal.

    conn       WBEM connection
    isAssoc    True for association, False for reference check
    namesOnly  If True, use AssociatorNames instead of Associators
    role       If True, specify the role for required operation
    resRole    If True, specify the result role for required operation

    Return True if test passed for all association and False otherwise
    """
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
    """Check for each class in @p classList that Associators
    method returns non-empty list of instances.

    classList   List of class names

    Return True if associators exist for all classes and False otherwise
    """
    
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

###################
# Diagnostics
###################

def checkDiagnosticsRun(conn, timeout):
    """For all DiagnosticTest instance and associated ManagedElement
    invoke RunDiagnosticService method and check corresponding Job state.

    conn        WBEM connection
    timeout     Timeout for Job running in seconds

    Return      True if tests passed and False overwise
    """
    passed = True
    try:
        instList = conn.EnumerateInstances('SF_DiagnosticTest')
    except Exception, e:
        logger.error("Failed to enumerate instances of "+
                     "SF_DiagnosticTest:\n%(error)s", {'error': e})
    
    for inst in instList:
        logger.info("Diagnostic Test for: %(instance)s",
                    {'instance': inst.path})
        try:
            assocList = conn.Associators(
                            inst.path,
                            AssocClass='SF_AvailableDiagnosticService')
        except Exception, e:
            logger.error("Failed to get associators:\n%(error)s",
                         {'error': e})
            passed = False
            continue
                
        for elem in assocList:
            logger.info("Managed Element: %(instance)s",
                        {'instance': elem.path})
            try:
                (rval, outParams) = conn.InvokeMethod(
                                            'RunDiagnosticService',
                                            inst.path,
                                            ManagedElement=elem.path)
            except Exception, e:
                logger.error("Failed to run diagnostics %(diag)s "
                             "on %(managedElem)s:\n%(error)s",
                             {'diag': inst.path,
                              'managedElem': elem.path,
                              'error': e})
                (rval, outParams) = (WBEMConn.RC_FAIL, {})
            
            if (rval == WBEMConn.RC_FAIL):
                passed = False
            else:
                logger.info("Job: %(instance)s",
                            {'instance': outParams})
            try:
                jobPath = outParams['Job']
            except Exception, e:
                logger.error("Failed to get result Job:\n%(error)s",
                             {'error': e})
                passed = False
                continue
            
            t = 0
            state = -1
            while t < timeout:
                try:
                    jobInst = conn.GetInstance(jobPath)
                except:
                    logger.error("Failed to get Job instance:\n" +
                                 "%(error)s", {'error': e})
                    passed = False
                    break
                state = jobInst['JobState']
                if state == JobState.COMPLETED:
                    logger.info("Job completed.")
                    break
                
                if (state > JobState.COMPLETED or
                    state == JobState.SUSPENDED):
                    logger.error("Unexcpected Job state: %(state)s",
                                 {'state': state})
                    passed = False
                    break
                t += 1
                time.sleep(1)
            
            if t >= timeout:
                logger.error("Timeout while waiting for the job end. " +
                             "Current state is %(state)s",
                             {'state': state})
    return passed


def checkDisabledIntf(conn):
    """For each SF_EthernetPort instance:
        - get DeviceID
        - disable corresponding interface
        - check that we still can get the same instances
        - enable corresponding interface
    
    conn        WBEM connection

    Return      True if tests passed and False overwise
    """
    
    passed = True
    try:
        instList = conn.EnumerateInstances('SF_EthernetPort')
    except Exception, e:
        logger.error("Failed to enumerate instances:\n%(error)s",
                     {'error': e})
        return False

    for inst in instList:
        deviceId = WBEMConn.getPropVal(inst, 'DeviceID')
        if deviceId:
            logger.info("DeviceId: %(id)s", {'id': deviceId})
        else:
            logger.error("Failed to get device ID")
            passed = False
            continue
       
        # Disable interface
        rc = changeState(conn, inst.path,
                         WBEMConn.uint16(RequestState.DISABLED))
        if (rc != WBEMConn.RC_OK and rc != WBEMConn.RC_NO_CHANGE):
            # If failed to change via RequestStateChange method,
            # try to change it via internal method (if supported).
            try:
                conn.SetIntfState(deviceId, False)
            except Exception, e:
                logger.error("Failed to set interface state:\n%(error)s",
                             {'error': e})
                passed = False
                continue
        
        try:
            instListNew = conn.EnumerateInstances('SF_EthernetPort')
        except Exception, e:
            logger.error("Failed to enumerate instances:\n%(error)s",
                         {'error': e})
            passed = False
        else:
            if len(instListNew) != len(instList):
                logger.error("SF_EthernetPort counts differs after" +
                             "interface disabling: %(before)s != %(after)s",
                             {'before': len(instList),
                              'after': len(instListNew)})
         
        # Enable interface
        rc = changeState(conn, inst.path,
                         WBEMConn.uint16(RequestState.ENABLED))
        if (rc != WBEMConn.RC_OK and rc != WBEMConn.RC_NO_CHANGE):
            # If failed to change via RequestStateChange method,
            # try to change it via internal method (if supported).
            try:
                conn.SetIntfState(deviceId, True)
            except Exception, e:
                logger.error("Failed to set interface state:\n%(error)s",
                             {'error': e})
                passed = False
                continue
     
    return passed

   
