import sys 
# import pymysql as mysql
import getopt
import os.path
from os import getenv

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
            'output-format' : 'native',
            'database-type' : 'NONE'
            }

    def __init__(self):
#        print("Constructor")
        pass

    def __del__(self):
#        print("Cleaning up")
        pass

    def dumpData(self):
        print("database     : " + self.param['database'])
        print("user         : " + self.param['user'])
        print("password     : " + self.param['password'])
        print("host         : " + self.param['host'])
        print("Database Type:" , self.param['database-type'])

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

        print(self.database)

        if self.database is databaseType.MYSQL:
            import pymysql as mysql
            self.db=mysql.connect( self.param['host'],
                self.param['database'],
                self.param['user'],
                self.param['password'] )
        elif self.database is databaseType.SQLITE:
            import sqlite3 as sqlite

            dbDir = getenv("CTL_DB")
            if dbDir == None:
                dbDir='/var/tmp'

            dbPath = dbDir + "/" + self.param['database'] + ".db"
            if self.param['verbose'] == 'true':
                print("Connecting to " + dbPath )

            self.db = sqlite.connect( dbPath )
            print("Self.db is " , self.db)
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
                rc = self.getParam(c[1])
            elif c[0] == "get-col":
                fred=self.sqlResults[self.rowIdx]
                print(fred[c[1]])
                failFlag=False
                rc=[failFlag, ""]
            elif c[0] == "load":
                print("Load command file")
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



