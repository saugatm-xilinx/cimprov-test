#!/usr/bin/python

import pywbem
import time

HOST='http://127.0.0.1:5988'
USER=''
PASSWORD=''

RC_OK   = 0
RC_FAIL = 2

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

