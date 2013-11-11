class TesterConfiguration:
    cfgList = {}
    current = None
    def __init__(self,
                 name,
                 desc,
                 vendorNS,
                 interopNS,
                 isWMI,
                 classListPath,
                 host='http://127.0.0.1:5988',
                 user='', passwd='',
                 loggerName='tester',
                 loggerFile='tester.log'):
        self.desc = desc
        self.host = host
        self.user = user
        self.passwd = passwd
        self.vendorNS = vendorNS
        self.interopNS = interopNS
        self.isWMI= isWMI
        self.classListPath = classListPath
        self.loggerName = loggerName
        self.loggerFile = loggerFile
        self.cfgList[name] = self

    def setClassList(self):
        self.classList = []
        try:
            f = open(self.classListPath, 'r')
        except Exception, e:
            print ("Failed to open file with classes list " + 
                  "<%(filePath)s>:%(error)s") % \
                  {'filePath':  self.classListPath,'error': e}
            return
        
        for line in f:
            self.classList.append(line.strip())
        f.close()

