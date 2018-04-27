import sys 
# import pymysql as mysql
import getopt
import os.path
from os import getenv
import os.path

from enum import Enum, auto

class databaseType(Enum):
    NONE = auto()
    MYSQL = auto()
    SQLITE = auto()
    LAST = auto()

class msgParser:
    echo=False
    db=None
    cursor=None
    rowIdx=0
    sqlResults=[]

    database = databaseType.NONE

    param = {
            'database'      : 'NODATA',
            'user'          : 'NOBODY',
            'password'      : 'NOTHING',
            'host'          : 'localhost',
            'verbose'       : 'false',
            'output-format' : 'native', # native, json, or forth
            'database-type' : 'NONE',
            'database-dir'  : "/var/tmp"
            }

    def __init__(self):
        localDbDir = getenv("CTL_DB")

        if localDbDir != None :
            self.param['database-dir']=localDbDir


    def __del__(self):
        pass

    def dumpData(self):
        print("database     : " + self.param['database'])
        print("user         : " + self.param['user'])
        print("password     : " + self.param['password'])
        print("host         : " + self.param['host'])
        print("Database Type:" , self.param['database-type'])
        print("Database Dir :" , self.param['database-dir'])
        print("Output Format:" , self.param['output-format'])

    def setDatabaseType(self, dbType):
        if dbType == 'MYSQL':
            self.param['database-type'] = dbType
            self.database = databaseType.MYSQL
        elif dbType == 'SQLITE':
            self.param['database-type'] = dbType
            self.database = databaseType.SQLITE
        else:
            self.param['database-type'] = 'NONE'
            self.database = databaseType.NONE

    def setEcho(self,flag):
        self.echo=flag

    def getEcho(self):
        return self.echo


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

        print("HERE")

        if self.database is databaseType.MYSQL:
            import pymysql as mysql
            self.db=mysql.connect( self.param['host'],
                self.param['database'],
                self.param['user'],
                self.param['password'] )
        elif self.database is databaseType.SQLITE:
            import sqlite3 as sqlite

            localDbDir = getenv("CTL_DB")
            if localDbDir == None:
                self.param['database-dir'] = localDbDir

            print("dbDir=" , dbDir)
            dbPath = dbDir + "/" + self.param['database'] + ".db"
            print("dbPath=" , dbPath)
            if self.param['verbose'] == 'true':
                print("Connecting to " + dbPath )

            self.db = sqlite.connect( dbPath )
#            print("Self.db is " , self.db)
            failFlag=False
        
        self.cursor = self.db.cursor()

        if self.database is databaseType.MYSQL:
            sql=' use ' + self.param['database']
    
            if self.param['verbose'] == 'true':
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
                print("rc=", rc)
                failFlag=False
                rc=[failFlag, ""]
            elif c[0] == "get-columns":
                print("get columns")

                fLen = len(self.cursor.description)
                fieldName = [i[0] for i in self.cursor.description]

                print(fieldName)
                failFlag=False
                rc=[failFlag, ""]
            elif c[0] == "get-row":
                print("get-row", self.rowIdx)
                print(self.sqlResults[self.rowIdx])
                failFlag=False
                rc=[failFlag, ""]
            elif c[0] == "go-first":
                self.rowIdx=0
                failFlag=False
                rc=[failFlag, ""]
            elif c[0] == "go-last":
                self.rowIdx= len(self.sqlResults)-1
                failFlag=False
                rc=[failFlag, ""]
            elif c[0] == "go-prev":
                if self.rowIdx > 0:
                    self.rowIdx -=1
                failFlag=False
                rc=[failFlag, ""]
            elif c[0] == "go-next":
                if self.rowIdx >= len(self.sqlResults)-1 :
                    pass
                else:
                    self.rowIdx += 1

                failFlag=False
                rc=[failFlag, ""]
            else:
                failFlag=True
                rc=[failFlag, ""]

        elif paramCount == 2:
            if c[0] == "get":
                failFlag=False

                rc = self.getParam(c[1])
                rc[0]=False

                if rc[1] =="":
                    rc[1]="UNKNOWN"
                    print(rc[1])
                else:
                    print(rc[1])


            elif c[0] == "get-col":
                fred=self.sqlResults[self.rowIdx]
                print(fred[c[1]])
                failFlag=False
                rc=[failFlag, ""]
            elif c[0] == "load":
                rc = self.getParam('database-dir')
                if rc[0] == False:
                    cmdFile = rc[1] + '/' + c[1]

                if not os.path.isfile(cmdFile) :
                    print("NO-FILE")
                    return rc
                count=0
                with open(cmdFile) as fp:
                    line=fp.readline()
                    while line:
                        rc=self.parseMsg(line)

                        if self.getParam('verbose') == 'true':
                            print(count,":" + line)
                        line=fp.readline()

            elif c[0] == "set-database":
                self.setDatabaseType(c[1])
                failFlag=False
                rc=[failFlag, ""]

        elif paramCount == 3:
            if c[0] == "set":
                failFlag=self.setParam(c[1],c[2])
                rc=[failFlag, ""]

        return rc


    def executeSql(self, sql):
        self.sqlResults=[]
        self.rowIdx=0

        if self.getParam('verbose') == 'true':
            print("executeSql:" + sql)
        try:
            self.cursor.execute(sql)

            if self.cursor.description == None:
                pass
            else:
                fLen = len(self.cursor.description)
                fieldName = [i[0] for i in self.cursor.description]

                results = self.cursor.fetchall()
                rowCount = len(results);

                if fLen > 0:
#                    results = self.cursor.fetchall()
                    for row in results:
#                        print(row)
                        count=0
                        fred={}
                        for col in row:
                            fred[ fieldName[count] ] = col
                            count += 1

                        self.sqlResults.append(fred)

        except Exception :
            print(sys.exc_info()[0])


    def parseMsg(self,msg):
        failFlag=True
        rc=[failFlag, ""]

        m=msg.strip()

        if len(m) == 0:
            failFlag=False
            rc=[failFlag, ""]
        else:

            if self.param['verbose'] == 'true':
                print("msg >" + m + "<")
    
            if m[0] == '^':
    #            print("Client command")
                rc=self.localParser( m[1:] )
            else:
#                print("sql")
                failFlag=False
                rc=[failFlag, ""]
                self.executeSql(msg)

        return rc



