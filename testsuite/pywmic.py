import subprocess

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
    def EnumerateInstances(self, ClassName, namespace = None, **params):
        if namespace is None:
            namespace = self.default_namespace
        cmd = ["wmic.exe", "/namespace:" + namespace, "path", ClassName, "get", "/value"]
        proc = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        instances = []
        inst = {}
        for line in proc.stdout:
            stripped_line = line.strip()
            if stripped_line:
                prop = stripped_line.split('=')
                inst[prop[0]] = prop[1]
            else:
                if inst:
                    instances.append(inst)
                    inst = {}
        return instances
    def EnumerateInstanceNames(self, ClassName, namespace = None, **params):
        return self.EnumerateInstances(ClassName, namespace, params)


