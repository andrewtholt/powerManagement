import sys 
import pymysql as mysql
import getopt
import os.path
from os import getenv


class msgParser:
    verbose=False
    db=None
    cursor=None
    rowIdx=0

    param = {
            'database' : 'automation',
            'user' : 'NOBODY',
            'password' : 'NOTHING',
            'host' : 'localhost'
            }

    def __init__(self):
        print("Constructor")

    def __del__(self):
        print("Cleaning up")

    def dumpData(self):
        print("database : " + self.param['database'])
        print("user     : " + self.param['user'])
        print("password : " + self.param['password'])
        print("host     : " + self.param['host'])

    def setVerbose(self,flag):
        self.verbose = flag

        print("msgParser verbose ", end="")
        if self.verbose:
            print("On")
        else:
            print("Off")


    def getParam(self,name):
        failFlag=True
        rc=[failFlag, ""]

        if name in self.param:
            failFlag=False
            rc=[ failFlag,self.param[name] ]
        else:
            failFlag=True
            rc=[ failFlag,"" ]

        return rc

    def setParam(self,name, value):
        failFlag=True

        if name in self.param:
            self.param[name] = value
            failFlag=False
        else:
            failFlag=True

        return failFlag

    def dbConnect(self):
        failFlag=True
        rc=[failFlag, ""]

        self.db=mysql.connect( self.param['host'],
                self.param['database'],
                self.param['user'],
                self.param['password'] )
        
        self.cursor = self.db.cursor()
        sql=' use ' + self.param['database']

        if self.verbose:
            print("sql>"+sql)

        if 0 == self.cursor.execute( sql ):
            failFlag=False
        else:
            failFlag=True

        rc=[failFlag, ""]
        return rc


    def localParser(self,cmd):
        failFlag=True
        rc=[failFlag, ""]

        c = cmd.split()
        paramCount = len(c)
        print( paramCount )

        if paramCount == 1:
            if c[0] == "help": 
                print("Help")
                failFlag=False
                rc=[failFlag, ""]
            elif c[0] == "print":
                self.dumpData()
                failFlag=False
                rc=[failFlag, ""]
            elif c[0] == "connect":
                rc=self.dbConnect()
            else:
                failFlag=True
                rc=[failFlag, ""]
        elif paramCount == 2:
            if c[0] == "get":
                rc = self.getParam(c[1])
        elif paramCount == 3:
            if c[0] == "set":
                failFlag=self.setParam(c[1],c[2])
                rc=[failFlag, ""]

        return rc


    def executeSql(self, sql):

        self.rowIdx=0
        if self.verbose:
            print("executeSql:" + sql)

        try:
            self.cursor.execute(sql)


            if self.cursor.description == None:
                pass
            else:
                fLen = len(self.cursor.description)
                if self.verbose:
                    print("Number of fields ",fLen)
                    print("Number of rows   ",self.cursor.rowcount)
        except Exception :
            print(sys.exc_info()[0])


    def parseMsg(self,msg):
        failFlag=True
        rc=[failFlag, ""]

        m=msg.strip()
        if self.verbose:
            print("msg >" + m + "<")

        if m[0] == '^':
            print("Client command")
            rc=self.localParser( m[1:] )
        else:
            print("sql")
            failFlag=False
            rc=[failFlag, ""]
            self.executeSql(msg)

        return rc



