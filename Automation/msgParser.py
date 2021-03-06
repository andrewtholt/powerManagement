import sys 
# import pymysql as mysql
import getopt
import os.path
from os import getenv
import os.path

from enum import Enum

class databaseType(Enum):
    NONE = 0
    MYSQL = 1
    SQLITE = 2
    LAST = 3

class msgParser:
    echo=False
    db=None
    cursor=None
    rowIdx=0
    sqlResults=[]
    log=None

    database = databaseType.NONE

    param = {
            'database'      : 'NODATA',
            'user'          : 'NOBODY',
            'password'      : 'NOTHING',
            'host'          : 'localhost',
            'connected'     : 'false',
            'verbose'       : 'false',
            'output-format' : 'interactive', # interactive, json, or forth
            'database-type' : 'NONE',
            'database-dir'  : "/var/tmp",
            'log-file'      : "NONE",
            'mqtt-host'     : "localhost",
            'mqtt-port'     : "1883"
            }

    def __init__(self):
        localDbDir = getenv("CTL_DB")

        if localDbDir != None :
            self.param['database-dir']=localDbDir

        if self.param['log-file'] != "NONE":
            self.log =  open( self.param['log-file'], 'a') 

    def __del__(self):
        pass

    def output(self, msg ):
        opFormat = self.param['output-format']

        if opFormat == 'forth':
            op=chr(len(msg)) + msg
            print( op,end="" )
        elif opFormat == 'interactive':
            print( msg )
        else:
            print(msg)

        if self.log !=None:
            self.inToLog( str(msg ))


    def dumpData(self):
        print("database      : " + self.param['database'])
        print("user          : " + self.param['user'])
        print("password      : " + self.param['password'])
        print("host          : " + self.param['host'])
        print("Database Type :" , self.param['database-type'])
        print("Database Dir  :" , self.param['database-dir'])
        print("Database State:" , self.param['connected'])
        print("Output Format :" , self.param['output-format'])
        print("MQTT Server   :" , self.param['mqtt-host'])
        print("MQTT Port     :" , self.param['mqtt-port'])


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
                dbDir = self.param['database-dir']
#                self.param['database-dir'] = localDbDir

            dbPath = dbDir + "/" + self.param['database'] + ".db"

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

        if rc[0]:
            self.param['connected'] = 'false'
        else:
            self.param['connected'] = 'true'
            self.output("CONNECTED")


        return rc


    def outToLog(self, msg):
        self.log.write("-->:" + msg + ":\n")
        self.log.flush()
        os.fsync(self.log)

    def inToLog(self, msg):
        self.log.write("<--:" + msg + ":\n")
        self.log.flush()
        os.fsync(self.log)

    def localParser(self,cmd):
        failFlag=True
        rc=[failFlag, ""]

        c = cmd.split()
        paramCount = len(c)

        if self.log != None:
            self.outToLog(cmd)


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
                failFlag=False
                rc=[failFlag, ""]
            elif c[0] == "get-columns":
                fLen = len(self.cursor.description)
                fieldName = [i[0] for i in self.cursor.description]

                print(fieldName)
                failFlag=False
                rc=[failFlag, ""]

            elif c[0] == "get-row-count":
                print(len(self.sqlResults))
                failFlag=False
                rc=[failFlag, ""]

            elif c[0] == "get-row":
                try:
#                    print("get-row", self.rowIdx)
                    print(self.sqlResults[self.rowIdx])
                    failFlag=False
                except:
                    print("ERROR")
                    failFlag = True

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
            elif c[0] == "get-next":
                if self.rowIdx >= len(self.sqlResults)-1 :
                    print("END")
                else:
                    self.rowIdx += 1
                    print(self.sqlResults[self.rowIdx])

                failFlag=False
                rc=[failFlag, ""]

            elif c[0] == "go-next":
                if self.rowIdx >= len(self.sqlResults)-1 :
                    print("END")
                else:
                    print("OK")
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
                    self.output(rc[1])
                else:
                    self.output(rc[1])

            elif c[0] == "test":
                self.output(c[1])
                failFlag=False
                rc=[failFlag, ""]
            elif c[0] == "get-col":
                fred=self.sqlResults[self.rowIdx]
                self.output(fred[c[1]])
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
#            print(sql)
            self.cursor.execute(sql)
            self.db.commit()

            if self.cursor.description == None:
                pass
            else:
                fLen = len(self.cursor.description)
                fieldName = [i[0] for i in self.cursor.description]

                results = self.cursor.fetchall()
                rowCount = len(results);

                if fLen > 0:
                    for row in results:
                        count=0
                        fred={}
                        for col in row:
                            fred[ fieldName[count] ] = col
                            count += 1

                        self.sqlResults.append(fred)

        except Exception :
            self.output("ERROR:SQL")
#            self.output(sys.exc_info()[0])


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
                rc=self.localParser( m[1:] )
            else:
#                print("sql")
                failFlag=False
                rc=[failFlag, ""]
                self.executeSql(msg)

        return rc



