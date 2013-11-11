from defs import *
from test_lib import checkProfile

profileSpecFmt="""
Scenario:
    Check for %(profList)s profiles:
        - registration
        - classes existance
        - instances counts
        - mandatory properties

Classes specification:
%(descList)s
"""

def profileMain(conn, params):
    return checkProfile(conn,
                        profList=params.profList,
                        descList=params.descList)

class ProfileParams:
    def __init__(self, profList, descList):
        self.profList = profList
        self.descList = descList

class ProfileTest(Test):
    @staticmethod
    def profListToStr(profList):
        res = profList[0]
        for prof in profList[1:]:
            res += ", " + prof
        return res
    
    @staticmethod
    def descListToStr(descList):
        res = str(descList[0])
        for desc in descList[1:]:
            res += "\n" + str(desc)
        return res
    
    def profileIter(self, params={}):
        self.testRun(self.params)
    
    def __init__(self, name, desc, params, ns=None):
        testSpec = profileSpecFmt % \
                   {'profList': ProfileTest.profListToStr(params.profList),
                    'descList': ProfileTest.descListToStr(params.descList)}
        Test.__init__(self, name, desc,
                      spec=testSpec,
                      package='profile',
                      func=profileMain,
                      iter=ProfileTest.profileIter,
                      ns=ns)
        self.params = params