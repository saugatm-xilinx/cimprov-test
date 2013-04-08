#!/usr/bin/python

import pywbem
import os
import sys
import string
import time
import logging
from tester_vars import *

CLASS_LIST_ENV_NAME = 'CIMPROV_CLASSES'
logger = logging.getLogger(LOGGER_NAME)

def class_list_fget():
    """Get classes list from the file specified by
    CLASS_LIST_ENV_NAME environment variable."""

    if not (CLASS_LIST_ENV_NAME in os.environ):
        logger.warning('Please set CIMPROV_CLASSES environment variable')
        return None
    class_list_path = os.environ[CLASS_LIST_ENV_NAME]

    f = open(class_list_path, 'r')
    class_list = []
    for line in f:
        class_list.append(line.strip())
    f.close()
    return class_list


#######################
# Enumeration
#######################

def enum_check(cli, class_list):
    """Check EnumerateInstance and EnumerateInstanceName operations
    for specified class list.
    
    cli         WBEM client
    class_list  List of classes to check

    Return True if test passed for all classes and False otherwise
    """
    passed = True
    for cl in class_list:
        res = "PASSED"
        logger.info('Checking class %s', cl)
        try:
            inst_list = cli.EnumerateInstances(cl)
            inst_name_list = cli.EnumerateInstanceNames(cl)
        except Exception, e:
            logger.error("Failed to enumerate class instances:\n{0}".format(e))
            passed = False
            continue
        if len(inst_list) != len(inst_name_list):
            logger.error('Number of ei and ein doesn\'t match: %d != %d',
                          len(inst_list), len(inst_name_list))
            passed = False
            res = "FAILED"
        logger.info("%s: %s", cl, res)
    return passed

##################
# Namespace
##################

def ns_check(check_ns):
    spl = check_ns.rsplit("/", 1)
    wbemclient = pywbem.WBEMConnection(TESTER_HOST,
                                       (TESTER_USER, TESTER_PASSWORD),
                                       spl[0])
    logger.info('Checking %s/%s existance...',  spl[0], spl[1])
    try:
        ns_list = wbemclient.EnumerateInstanceNames('__Namespace')
    except:
        logger.error('Failed to enumerate namespaces names')
        return False
    for ns in ns_list:
        if ns[u'Name'] == spl[1]:
            return True
    return False

################
# Profiles
################

def profile_registered(profile_name):
    ns = TESTER_INTEROP_NS
    
    wbemclient = pywbem.WBEMConnection(TESTER_HOST,
                                       (TESTER_USER, TESTER_PASSWORD), ns)
    inst_list = wbemclient.EnumerateInstances('SF_RegisteredProfile')
    for inst in inst_list:
        if inst[u'RegisteredName'] == profile_name:
            return True
    return False

def profile_check(prof_list, spec_list, ns):
    
    # Indices of specification list
    SC_NAME         = 0
    SC_PROP         = 1
    SC_ASSOC        = 2
    SC_INST_NUM     = 3
    
    SC_ASSOC_ACL    = 0
    SC_ASSOC_RCL    = 1
    SC_ASSOC_NUM    = 2

    global_passed = True
    
    logger.info("Checking profiles registration...")
    for profile in prof_list:
        res = profile_registered(profile)
        logger.info("Profile {0}: {1}".format(
                    profile, res and "OK" or "ENOENT"))
    
    logger.info("Checking profile classes...")
    
    wbemclient = pywbem.WBEMConnection(TESTER_HOST,
                                       (TESTER_USER, TESTER_PASSWORD),
                                       ns)
    for class_spec in spec_list:
        logger.info("Trying to enumerate instances of " + class_spec[SC_NAME])
        class_passed = True
        try:
            inst_list = wbemclient.EnumerateInstances(class_spec[SC_NAME])
        except Exception, e:
            logger.error("Failed to enumerate instances:\n{0}".format(e))
            global_passed = False
            logger.info("Class {0} FAILED".format(class_spec[SC_NAME]))
            continue
        
        inst_count = 0
        for inst in inst_list:
            inst_count += 1
            logger.info("Instance #{0}".format(inst_count))

            # Checking properties
            for req_prop in class_spec[SC_PROP]:
                logger.info("Checking property {0}".format(req_prop))
                try:
                    if inst.properties[req_prop].value == None:
                        logger.error(("Required property {0} " +
                                      "has NULL value").format(req_prop))
                except Exception, e:
                    logger.error(("Failed to get property " +
                                "{0} of {1}:\n{2}").format(
                                            req_prop, inst.path, e))
                    global_passed = False
                    class_passed = False

            # Checking associations
            for assoc_spec in class_spec[SC_ASSOC]:
                title = "{0} - {1} x{2}".format(assoc_spec[SC_ASSOC_ACL],
                                                assoc_spec[SC_ASSOC_RCL],
                                                assoc_spec[SC_ASSOC_NUM])
                logger.info("Checking association instance counts: " + title)
                try:
                    assoc_list = wbemclient.Associators(inst.path,
                        AssocClass=assoc_spec[SC_ASSOC_ACL],
                        ResultClass=assoc_spec[SC_ASSOC_RCL])
                except Exception, e:
                    logger.error("Failed to get associators:\n{0}".format(e))
                    global_passed = False
                    class_passed = False
                    continue

                assoc_count = 0
                for assoc in assoc_list:
                    assoc_count += 1
                if assoc_count != assoc_spec[SC_ASSOC_NUM]:
                    logger.error(("{0}: unexpected assotiators number: " +
                                  "{1} instead of {2}").format(
                                            title, assoc_count,
                                            assoc_spec[SC_ASSOC_NUM]))
                    global_passed = False
                    class_passed = False

        if class_spec[SC_INST_NUM] > 0:
            total = "{0}: expected {1} {2}".format(
                class_spec[SC_NAME], class_spec[SC_INST_NUM],
                class_spec[SC_INST_NUM] == 1 and "instance" or "instances")
            logger.info(total)
            
            if (class_spec[SC_INST_NUM] != inst_count):
                logger.error(("{0}: unexpected instance count: " +
                              "{1} instead of {2}").format(
                                class_spec[SC_NAME], inst_count,
                                class_spec[SC_INST_NUM]))
                global_passed = False
                class_passed = False
        logger.info("Class {0} {1}".format(class_spec[SC_NAME],
                        class_passed and "PASSED" or "FAILED"))
    return global_passed

######################
# Request state change
######################
RC_OK   = 0
RC_FAIL = 2

def state_change(conn, inst_path, state):
    try:
        (rval, out_params) = conn.InvokeMethod('RequestStateChange',
                                    inst_path, RequestedState=state)
    except Exception, e:
        logger.error("Failed to change state of {0} to {1}:\n{2}".format(
                                                inst_path, state, e))
        (rval, out_params) = (RC_FAIL, {})
    
    #TODO: do we need out_params ?
    return rval

def req_state_change_check(ns, class_name, state, timeout):
    passed = True
    wbemclient = pywbem.WBEMConnection(TESTER_HOST,
                                       (TESTER_USER, TESTER_PASSWORD), ns)
    inst_list = wbemclient.EnumerateInstances(class_name)
    inst_num = 0
    for inst in inst_list:
        inst_num += 1
        logger.info("instance #{0}".format(inst_num))
        # Change state
        rc = state_change(wbemclient, inst.path,
                          pywbem.Uint16(state))
        if rc != RC_OK:
            passed = False
            continue
        
        # Wait for the state update
        time.sleep(timeout)
        
        # Check oper state
        new_inst = wbemclient.GetInstance(inst.path)
        if new_inst[u'EnabledState'] != state:
            logger.error(("Oper state of instance {0} is unexpected: " +
                        "{1} instead of {2}").format(inst.path,
                        new_inst[u'EnabledState'], state))
            passed = False
            continue
        
        # Restore old state
        rc = state_change(wbemclient, inst.path,
                          pywbem.Uint16(inst[u'EnabledState']))
        if rc != RC_OK:
            passed = False
    return passed

#######################
# Association traversal
#######################

def associators_check(cli, num, ref, cl, names_only, role, res_role):
    func = names_only and cli.AssociatorNames or cli.Associators
    try:
        ref[num]['assoc'] = func(ref[num]['path'],
                            AssocClass=cl,
                            ResultClass=ref[1 - num]['inst'].classname,
                            Role=role and ref[num]['name'] or '',
                            ResultRole=res_role and ref[1 - num]['name'] or '')
    except:
        return False
    for assoc in ref[num]['assoc']:
        if string.find(str(names_only and assoc or assoc.path),
                       str(ref[1 - num]['inst'].path)) != -1:
            return True
    return False

def references_check(cli, num, ref, cl, names_only, role, res_role):
    func = names_only and cli.ReferenceNames or cli.References
    try:
        ref[num]['assoc'] = func(ref[num]['path'],
                            ResultClass=cl,
                            Role=role and ref[num]['name'] or '')
    except:
        return False
    for assoc in ref[num]['assoc']:
        if string.find(str(names_only and assoc or assoc.path),
                       str(ref[1 - num]['inst'].path)) != -1:
            return True
    return False

def assoc_traversal(cli, class_list, is_assoc,
                    names_only, role, res_role=False):
    '''Perform association/reference traversal.

    cli         WBEM client
    class_list  Classes list to check
    is_assoc    True for association, False for reference check
    names_only  If True, use AssociatorNames instead of Associators
    role        If True, specify the role for required operation
    res_role    If True, specify the result role for required operation

    Return True if test passed for all association and False otherwise
    '''
    global_passed = True
    if is_assoc:
        logger.info("Association traversal parameters:\n" +
                    "names_only = {0}\nrole = {1}\nres_role = {2}".format(
                    names_only, role, res_role))
    else:
        logger.info("Reference traversal parameters:\n" +
                    "names_only = {0}\nrole = {1}".format(
                    names_only, role))

    for cl in class_list:
        passed = True
        cl_obj = cli.GetClass(cl, LocalOnly=False, IncludeQualifiers=True)
        if not ('Association' in cl_obj.qualifiers):
            continue
        
        logger.info("Checking association class " + cl)
        inst_list = cli.EnumerateInstances(cl)
        for inst in inst_list:
            ref = [{}, {}]
            num = 0
            cross_ns = False
            logger.info("Checking {0} instance {1}".format(cl, inst))
            for key, prop in inst.properties.items():
                if prop.type == 'reference':
                    ref[num]['path'] = prop.value
                    ref[num]['name'] = key
                    num +=1
                    #TODO: ignore >2-way associations
            for r in ref:
                logger.info("Get reference instance:\n{0}".format(r))
                if r['path'].namespace != TESTER_NS:
                    #TODO: proper handling
                    logger.warn("Cross-namespace reference - ignore");
                    cross_ns = True
                    break
                try:
                    r['inst'] = cli.GetInstance(r['path'])
                except Exception, e:
                    logger.error(
                    "Failed to get reference instance:\n{0}".format(e))
                    logger.info("Namespace is {0}\n".format(r['path'].namespace))
                    passed = False
                    global_passed = False
            
            if cross_ns:
                continue
            if not passed:
                break
            
            func = is_assoc and  associators_check or references_check
            passed = func(cli, 0, ref, cl, names_only,
                                     role, res_role)
            passed = passed and func(cli, 1, ref, cl, names_only,
                                     role, res_role)
        logger.info("{0}: {1}".format(cl, passed and "PASSED" or "FAILED"))
    return global_passed

