#!/usr/bin/python

import pywbem
import os
import sys
import string
import time

HOST='http://127.0.0.1:5988'
USER=''
PASSWORD=''

RC_OK   = 0
RC_FAIL = 2

CLASS_LIST_ENV_NAME = 'CIMPROV_CLASSES'

def class_list_fget():
    # Get path of the file with classes list 
    if not (CLASS_LIST_ENV_NAME in os.environ):
        print 'Please set CIMPROV_CLASSES environment variable'
        sys.exit(0)
    class_list_path = os.environ[CLASS_LIST_ENV_NAME]

    # Get classes list
    f = open(class_list_path, 'r')
    class_list = f.readlines()
    f.close()
    return class_list

def enum_check(cli):
    failed_list = []
    class_list = class_list_fget()
    # Check classes
    for cl in class_list:
        inst_list = cli.EnumerateInstances(cl)
        inst_name_list = cli.EnumerateInstanceNames(cl)
        if len(inst_list) != len(inst_name_list):
            failed_list.append(cl)
    return failed_list

def ns_check(parent_ns, check_ns):
    wbemclient = pywbem.WBEMConnection(HOST, (USER, PASSWORD), parent_ns)
    exist = False
    print 'Checking',  parent_ns + '/' + check_ns, 'existance...',
    try:
        ns_list = wbemclient.EnumerateInstanceNames('__Namespace')
    except:
        print 'FAIL'
    else:
        for ns in ns_list:
            if ns[u'Name'] == check_ns:
                exist = True
        if exist:
            print 'OK'
        else:
            print 'FAIL'

def profile_registered(profile_name):
    NS = 'root/PG_InterOp'
    
    wbemclient = pywbem.WBEMConnection(HOST, (USER, PASSWORD), NS)
    inst_list = wbemclient.EnumerateInstances('SF_RegisteredProfile')
    for inst in inst_list:
        if inst[u'RegisteredName'] == profile_name:
            return True
    return False 

def class_operations_check(class_name, ns):
    #TODO: GetInstance
    OP_LIST = ['Associators', 'AssociatorNames',
               'References', 'ReferenceNames',
               'EnumerateInstances', 'EnumerateInstanceNames']

    wbemclient = pywbem.WBEMConnection(HOST, (USER, PASSWORD), ns) 
    print "Checking standard operations for %s class:" % class_name
    for op in OP_LIST:
        print "    %-56s" % op,
        try:
            pywbem.WBEMConnection.__dict__[op].__get__(
                    wbemclient, pywbem.WBEMConnection)(class_name)
            print "OK"
        except:
            print "ENOTSUPP"

def profile_check(prof_list, spec_list, ns):
    
    # Indices of specification list
    SC_NAME         = 0
    SC_PROP         = 1
    SC_ASSOC        = 2
    SC_INST_NUM     = 3
    
    SC_ASSOC_ACL    = 0
    SC_ASSOC_RCL    = 1
    SC_ASSOC_NUM    = 2
    
    print "Checking profile registration:"
    for profile in prof_list:
        print "%-60s" % profile, profile_registered(profile) and 'OK' or 'ENOENT'

    print "\nChecking profile classes:"
    
    wbemclient = pywbem.WBEMConnection(HOST, (USER, PASSWORD), ns)
    for class_spec in spec_list:
        print "%-60s" % class_spec[SC_NAME],
        try:
            inst_list = wbemclient.EnumerateInstances(class_spec[SC_NAME])
        except:
            print "ENOENT\n"
        else:
            print "OK"
            inst_count = 0
            for inst in inst_list:
                inst_count += 1
                print "    instance #", inst_count

                # Checking properties
                for req_prop in class_spec[SC_PROP]:
                    print "        %-52s" % req_prop,
                    try:
                        if inst.properties[req_prop].value != None:
                            print "OK"
                        else:
                            print "NULL"
                    except:
                        print "ENOENT"

                # Checking associations
                for assoc_spec in class_spec[SC_ASSOC]:
                    title = "{0} - {1} x{2}".format(assoc_spec[SC_ASSOC_ACL],
                                                    assoc_spec[SC_ASSOC_RCL],
                                                    assoc_spec[SC_ASSOC_NUM])
                    print "        %-52s" % title,
                    try:
                        assoc_list = wbemclient.Associators(inst.path,
                            AssocClass=assoc_spec[SC_ASSOC_ACL],
                            ResultClass=assoc_spec[SC_ASSOC_RCL])
                    except:
                        print "FAIL"
                    else:
                        assoc_count = 0
                        for assoc in assoc_list:
                            assoc_count += 1
                        if assoc_count != assoc_spec[SC_ASSOC_NUM]:
                            print "EINVAL (%d)" % assoc_count
                        else:
                            print "OK"
            if class_spec[SC_INST_NUM] > 0:
                total = "Expected: " + str(class_spec[SC_INST_NUM]) + (
                        class_spec[SC_INST_NUM] == 1 and " instance" or " instances")
                print "%-60s" % total,
                
                if (class_spec[SC_INST_NUM] != inst_count):
                    print "EINVAL (%d)\n" % inst_count
                else:
                    print "OK\n"
            else:
                print ""


def state_change(conn, inst_path, state, timeout):
    try:
        (rval, out_params) = conn.InvokeMethod('RequestStateChange', inst_path,
                                    RequestedState=state,
                                    #TimeoutPeriod=pywbem.CIMDateTime(
                                    #     pywbem.timedelta(seconds=timeout))
                                    )
    except:                         
        (rval, out_params) = (RC_FAIL, {}) 
    
    #TODO: do we need out_params ?
    return rval

def req_state_change_check(ns, class_name, state, timeout):
    wbemclient = pywbem.WBEMConnection(HOST, (USER, PASSWORD), ns)
    inst_list = wbemclient.EnumerateInstances(class_name)
    inst_num = 0
    for inst in inst_list:
        inst_num += 1
        print '    instance #%d...' % inst_num,
        # Change state
        rc = state_change(wbemclient, inst.path,
                          pywbem.Uint16(state),
                          timeout)
        if rc != RC_OK:
            print "FAIL"
            continue
        
        # Wait for the state update
        time.sleep(timeout)
        
        # Check oper state
        new_inst = wbemclient.GetInstance(inst.path)
        if new_inst[u'EnabledState'] != state:
            print "FAIL"
            continue
        
        # Restore old state
        rc = state_change(wbemclient, inst.path,
                          pywbem.Uint16(inst[u'EnabledState']),
                          timeout)
        if rc != RC_OK:
            print "FAIL"
        else:
            print "OK"

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

def assoc_traversal(cli, silent, is_assoc, names_only, role, res_role):
    class_list = class_list_fget()
    for cl in class_list:
        passed = True
        cl_obj = cli.GetClass(cl, LocalOnly=False, IncludeQualifiers=True)
        if not ('Association' in cl_obj.qualifiers):
            continue
        if not silent:
            print "%-60s" % (cl[-1] == '\n' and cl[:-1] or cl),
        inst_list = cli.EnumerateInstances(cl)
        for inst in inst_list:
            ref = [{}, {}]
            num = 0
            for key, prop in inst.properties.items():
                if prop.type == 'reference':
                    ref[num]['path'] = prop.value
                    ref[num]['name'] = key
                    num +=1
                    #TODO: ignore >2-way associations
            #print "Trying to get references:"
            #print ref[0]
            #print ref[1]
            try:
                ref[0]['inst'] = cli.GetInstance(ref[0]['path'])
                ref[1]['inst'] = cli.GetInstance(ref[1]['path'])
            except:
                passed = False
                continue
            
            func = is_assoc and  associators_check or references_check
            passed = passed and func(cli, 0, ref, cl, names_only, 
                                     role, res_role)
            passed = passed and func(cli, 1, ref, cl, names_only, 
                                     role, res_role)
        if not silent:                                     
            print passed and "PASSED" or "FAILED"
    return passed


