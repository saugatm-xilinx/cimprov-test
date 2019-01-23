#!/usr/bin/python

"""set_vm_ptp_privilege.py --

Configure Solarflare SR-IOV based PTP time sync/distribution.

This script demonstrates the use of vSphere APIs to implement the configuration
of an ESX host with PTP capable Solarflare NICs to do PTP time synchronization
and distribution via SR-IOV.

This script is mostly an empty shell which needs to be hooked up to Solarflare's
management callbacks (possibly via the CIM provider).

Prerequisities:

    # Python 2.7.x

    # Python bindings for vSphere API [1].
    $ sudo pip install --upgrade pyvmomi

Usage:

    # Print summary info about host/vm
    $ ./set_vm_ptp_privilege.py info HOSTNAME VMNAME

    # Configure a Host-Local PTP Master VM.
    $ ./set_vm_ptp_privilege.py set-master HOSTNAME VMNAME

[1] https://github.com/vmware/pyvmomi/tree/master/docs.
"""

import atexit
import getpass
import os
import time
import sys
import ssl
import pyVim
import pyVim.connect
import pyVmomi
import pywbem
import pywbem.exceptions

SOLARFLARE_VENDORID = 0x1924

def fatal(msg):
    print "FATAL: %s" % msg
    sys.exit(1)


def wbem_establish_connection(hostname, user, password):
    try:
        port_string = ':5989'
        hostname = 'https://'+ hostname + port_string
        conn =  pywbem.WBEMConnection(hostname, (user, password),
                                      default_namespace = 'solarflare/cimv2',
                                      no_verification = True)
        return conn
    except ConnectionError:
        fatal('Connection to host %s failed',hostname)
         
def get_current_privileges(wbem_conn, instance, pci_id, ptp_set = False):
    try:
        ptp_priv_set = 0
        priv_dict = wbem_conn.InvokeMethod("GetFuncPrivileges", 
                                           instance, PCIAddr = pci_id)
        priv_list =  priv_dict[1].get('PrivilegeNames')
        if not len(priv_list):
           if ptp_set == True:
               fatal('PTP privilege not set')
           else:
               print "No Privileges Set Currently"
        else :
           print "Following Privileges are set currently:"
           for priv in priv_list:
               print priv
               if priv == 'PTP':
                   ptp_priv_set = 1
           if ptp_set == True and ptp_priv_set == 0:
               fatal('PTP privilege not set')
    except pywbem.exceptions.CIMError, e:
        fatal ("get_current_privileges: CIMError "+ e.status_description)
    except Exception, e:
        fatal ("get_current_privileges: Exception "+ str(e))
                 

def get_host_pci_devices(host_ref, vendor_id=None):
    """Return a list of all host PCI devices. Optionally, filter by given
    vendor ID.
    """
    return [ pci_dev for pci_dev in host_ref.hardware.pciDevice
                if vendor_id is None or pci_dev.vendorId == vendor_id ]


def get_pci_device(host_ref, sbdf):
    """Return a vim.host.PCIDevice object corresponding to its id in SBDF
    notation. Returns None if device is not found.
    """
    for pci_dev in host_ref.hardware.pciDevice:
        if pci_dev.id == sbdf:
            return pci_dev
    return None


def get_sriov_nics(vm_ref):
    """Return vim.vm.device.VirtualSriovEthernetCard objects representing
    Virtual NICs backed by an SR-IOV passthru device.
    """
    nics = []
    for device in vm_ref.config.hardware.device:
        if isinstance(device, pyVmomi.vim.vm.device.VirtualSriovEthernetCard):
            nics.append(device)
    return nics


def vm_power_state(vm_ref):
    """Return a string representation of a given VM's power-state. on, off,
    or suspended.
    """
    return ('on'  if (vm_ref.runtime.powerState ==
                       pyVmomi.vim.VirtualMachine.PowerState.poweredOn)   else
            ('off' if (vm_ref.runtime.powerState ==
                       pyVmomi.vim.VirtualMachine.PowerState.poweredOff)  else
             'suspended'))


def find_vm(host_ref, vm_name):
    """Find a VM by its registered name.
    """
    for vm_ref in host_ref.vm:
        if vm_ref.name == vm_name:
            return vm_ref
    return None


def usage(msg=None, ec=0):
    """Print usage and exit with given errorcode.
    """
    if msg is not None:
        print "%s" % msg
    print "usage: %s info|set-master HOSTNAME VMNAME" % sys.argv[0]
    sys.exit(ec)


def do_info(host_ref, vm_ref, conn):
    """Print summary info about Solarflare NICs attached to the Host,
    and VMs with Solarflare Virtual Functions passed through.
    """

    print "Solarflare NICs"
    for dev in get_host_pci_devices(host_ref, vendor_id=SOLARFLARE_VENDORID):
        print "  id: %s deviceId: 0x%04x (%s)" % (
                dev.id, dev.deviceId, dev.deviceName)

    if vm_ref is None:
        vm_refs = host_ref.vm
    else:
        vm_refs = [vm_ref]
    for vm_ref in vm_refs:
        print ""
        print "[%s]" % vm_ref.name
        print "  power: %s" % vm_power_state(vm_ref)
        print "  sriov ethernet devices:"
        for eth in get_sriov_nics(vm_ref):
            print "    %s (%s)"   % (eth.deviceInfo.label, eth.deviceInfo.summary)
            print "      pf: %s"   % eth.sriovBacking.physicalFunctionBacking.id
            print "      vf: %s"   % eth.sriovBacking.virtualFunctionBacking.id


def do_set_master(host_ref, vm_ref, conn):
    """Configure given VM to be host-local PTP master.
    """
    print ("Configuring %s:%s to be the Local Master ..." %
           (host_ref.name, vm_ref.name))

    # Step 1. Power-on VM
    #
    #   An SR-IOV backed virtual ethernet card is only assigned a VF at
    #   VM power-on, and persisted until the VM is powered-off (or
    #   suspended.
    power_state = vm_power_state(vm_ref)
    print "Current VM power state is %s" % power_state
    if power_state != 'on':
        answer = raw_input("Power on vm? [y/n] ")
        if answer != 'y':
            fatal("The VM needs to be powered on. Bailing out.") 
        print "Powering on %s ..." % vm_ref.name
        task = vm_ref.PowerOn()
        while task.info.state in ('queued', 'running'):
            time.sleep(1)
        if task.info.state == 'error':
            fatal("Failed to power on VM. Bailing out.")

    # Step 2. Wait for guest to become ready
    #
    #   Since FLR to the device may reset its PHC configuration, we ensure
    #   that we only proceed once the guest has successfully booted. We use
    #   VMware tools ready status as a reliable indication of this.
    print "Waiting for guest operations to become ready..."
    while not vm_ref.guest.guestOperationsReady:
        time.sleep(1)

    # Step 3. Query VM for backing VF information
    #
    #   Find SR-IOV backed Virtual NICs with Solarflare Vendor ID. If there
    #   are none, then it's a misconfiguration, and if there are many, we
    #   pick the first one.
    nics = get_sriov_nics(vm_ref)
    sf_nics = []
    for nic in nics:
        vf_pci = get_pci_device(host_ref, nic.sriovBacking.virtualFunctionBacking.id)
        if vf_pci.vendorId == SOLARFLARE_VENDORID:
            sf_nics.append(nic)

    if len(sf_nics) == 0:
        fatal("No Solarflare SR-IOV ethernet cards found attached to VM.")

    # Step 4. Configure VM
    #
    #   VM is in a powered-on state, and we have the necessary information to
    #   configure the device.
    #
    #   TBD: This is where the script issues a request to Solarflare's CIM
    #   provider to configure a VF to allow read/write access, and the
    #   remaining VFs read-access to the adapter clock.
    nic = sf_nics[0]
    vf_pci = get_pci_device(host_ref,
                            nic.sriovBacking.virtualFunctionBacking.id)
    print "Configuring VF for Local Master mode of operation"
    print "        id: %s" % vf_pci.id
    print "  deviceId: 0x%x" % vf_pci.deviceId
    print "      name: %s" % vf_pci.deviceName

    try:
        instances = conn.EnumerateInstanceNames('SF_API')
        get_current_privileges(conn, instances[0], vf_pci.id)
        print "Setting PTP Privilege .."
        conn.InvokeMethod("ModifyFuncPrivileges",instances[0],PCIAddr=vf_pci.id,AddedMask='PTP')
        get_current_privileges(conn, instances[0], vf_pci.id, True)
    except pywbem.exceptions.CIMError, e:
        fatal ("do_set_master: CIMError "+ e.status_description)
    except IndexError, e:
        fatal ("do_set_master: Enumerate Instance SF_API:" + str(e))
    except Exception, e:
        fatal ("get_current_privileges: Exception "+ str(e))

def main():
    if len(sys.argv) < 3:
        usage(ec=1)
    sub_cmd = sys.argv[1]
    host_name = sys.argv[2]
    vm_name = None
    wbemconn = None
    if len(sys.argv) > 3:
        vm_name = sys.argv[3]

    if sub_cmd == 'info':
        do_fn = do_info
    elif sub_cmd == 'set-master':
        if vm_name is None:
            usage("VM name is required for set-master", 1)
        do_fn = do_set_master
    else:
        usage("unrecognized command: %s" % sub_cmd)

    print "Connecting to %s ..." % host_name
    user, pwd = raw_input("username: "), getpass.getpass("password: ") 
    si = pyVim.connect.SmartConnect(host=host_name, user=user, pwd=pwd)
    atexit.register(pyVim.connect.Disconnect, si)
    content = si.RetrieveContent()
    objview = content.viewManager.CreateContainerView(
                content.rootFolder, [pyVmomi.vim.HostSystem], True
              )
    for h in objview.view:
        print h.name
        if h.name == host_name:
            host_ref = h
            break
    else:
        usage("Failed to find host: %s" % host_name)

    vm_ref = None
    if vm_name is not None:
        vm_ref = find_vm(host_ref, vm_name)
        if vm_ref is None:
            fatal("Failed to find %s in %s" % (vm_name, host_name))
 
    if sub_cmd == 'set-master':
        print 'Establishing connection to host:' + host_name
        wbemconn = wbem_establish_connection(host_name, user, pwd)
    do_fn(host_ref, vm_ref, wbemconn)
    sys.exit(0)

"""Added to pass SSL certificate verification """
if (not os.environ.get('PYTHONHTTPSVERIFY', '') and
    getattr(ssl, '_create_unverified_context', None)):
    ssl._create_default_https_context = ssl._create_unverified_context

main()
