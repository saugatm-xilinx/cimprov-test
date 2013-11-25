import subprocess
import re

class CIMInstance:
    def __init__(self, path = None, prop = {}):
        self.properties = dict(prop)
        self.path = path
        
    def __repr__(self):
        if self.path is None:
            return "%s" % self.properties
        else:
            return self.path

    def __eq__(self, other):
        return self.properties == other.properties

class WBEMConnection(object):
    def __init__(self, url, creds = None, default_namespace = '\\\\root\\cimv2'):
        self.url = url
        self.creds = creds
        self.default_namespace = default_namespace
    def __repr__(self):
        if self.creds is None:
            user = 'anonymous'
        else:
            user = 'user=%s' % `self.creds[0]`
        return "%s(%s, %s, namespace=%s)" % (self.__class__.__name__, `self.url`,
                                             user, `self.default_namespace`)
    def EnumerateInstances(self, ClassName, namespace = None):
        if namespace is None:
            namespace = self.default_namespace
        cmd = ["wmic.exe", "/namespace:" + namespace, "path", ClassName, "get", "/value"]
        proc = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        out, err = proc.communicate()
        if proc.returncode != 0:
            raise Exception('wmic exited with status %r: %r' % (proc.returncode, err))
        lines = out.split('\r\r\n')
        instances = []
        inst = CIMInstance()
        for line in lines:
            stripped_line = line.strip()
            if stripped_line:
                prop = stripped_line.split('=', 1)
                inst.properties[prop[0]] = prop[1]
            else:
                if inst.properties:
                    instances.append(inst)
                    inst = CIMInstance()
        if inst.properties:
            instances.append(inst)
        return instances

    def EnumerateInstanceNames(self, ClassName, namespace = None):
        if namespace is None:
            namespace = self.default_namespace
        cmd = ["wmic.exe", "/namespace:" + namespace, "path", ClassName, "get", "__path", "/value"]
        proc = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        out, err = proc.communicate()
        if proc.returncode != 0:
            raise Exception('wmic exited with status %r: %r' % (proc.returncode, err))
        lines = out.split('\r\r\n')
        instances = []
        inst = CIMInstance()
        for line in lines:
            stripped_line = line.strip()
            if stripped_line:
                prop = stripped_line.split('=', 1)
                if prop[0][0] != '_':
                    inst.properties[prop[0]] = prop[1]
                else:
                    if prop[0] == '__PATH':
                        inst.path = prop[1]
            else:
                if inst.properties:
                    instances.append(inst)
                    inst = CIMInstance()
        if inst.properties:
            instances.append(inst)
        return instances

    def AssociatorsCheck(self, ClassName, namespace = None):
        if namespace is None:
            namespace = self.default_namespace
        cmd =  ["wmic.exe", "/namespace:" + namespace, "path", ClassName, "assoc"]
        proc = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        out, err = proc.communicate()
        if proc.returncode != 0:
            raise Exception('wmic exited with status %r:\n%r' % (proc.returncode, err))
        if err:
            raise Exception("wmic stderr:\n%r" % err)
        return True

    def DeviceIdToGUID(self, id):
        pattern='\\DEVICE\\TCPIP_'
        if id.find(pattern) == 0:
            return id[len(pattern):]
        else:
            return None

    def SetIntfState(self, guid, state):
        cmd = ['wmic.exe', 'path', 'Win32_NetworkAdapter', 'WHERE',
               "GUID='%s'" % guid, 'call',
               state and 'enable' or 'disable']
        proc = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        out, err = proc.communicate()
        if proc.returncode != 0:
            raise Exception(("wmic exited with status %r:\n%r\n" +
                            "cmd was: %s") % (proc.returncode, err, cmd))
        return True
       

