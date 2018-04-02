import sys 
import sqlite3 as sqlite
import getopt
import os.path
from os import getenv
import paho.mqtt.client as mqtt
import configparser as cp

class logicVM:
    ip=0
    logicFile=""
    db   = None
    pdir = None
    configFile="/etc/mqtt/bridge.ini"
    msg=False

    con = None
    cur = None

    prog = []
    acc = 0
    

    def __init__(self, lf="logic.txt"):
        self.logicFile = lf
        self.db = getenv("CTL_DB")
        self.pdir = getenv("CTL_PDIR")
        if self.db == None or self.pdir == None:
            print("FATAL ERROR: setup CTL_PDIR & CTL_DB env variables")
            sys.exit(1)

        self.con = sqlite.connect( self.db )
        self.cur = self.con.cursor()

    # 0=False 1=True, >2 unknown/invalid

    def getDb(self):
        return self.con, self.cur 

    def logicToStr(self, state):

        s="UNKNOWN"

        if state == 0:
            s="FALSE"
        elif state == 1:
            s="TRUE"
        else:
            s="UNKNOWN"

        return s

    # 0=False 1=True, >2 unknown/invalid
    def strToLogic(self, state):
        s = 0
        if state == "TRUE" or state == "ON" or state == "YES":
            s = 1
        elif state == "FALSE" or state == "OFF" or state == "NO":
            s = 0
        else:
            s = 2

        return s

    def load(self):
        print("Load")
        failFlag = True

        try:
            tmp=open(self.logicFile).readlines()
            data=map(str.strip,tmp)

            for n in data:
                d = n.split(" ")

                if len(d) == 0:
                    pass
                elif len(d[0]) == 0:
                    pass
                else:
                    self.prog.append(d)
   
                if self.verbose:
                    if len(d) == 0:
                        pass
                    if len(d) == 1:
                        if len(d[0]) > 0:
                            print(d[0])
                    if len(d) == 2:
                        print(d[0], d[1])
                

            print(self.prog)
            failFlag=False

        except FileNotFoundError:
            failFlag=True

        return failFlag

    def end(self):
        self.ip=0;

    def logicAndNot(self):
        name=self.prog[self.ip][1]

        sql = "select state from io_point where name = '%s'" % name

        if self.verbose:
            print("\t " + sql )

        self.cur.execute( sql )
        data = self.cur.fetchone()
        print("\t " + data[0])

        state = self.strToLogic(data[0])

        if state < 2:
            if state ==  1:
                state = 0
            elif state == 0:
                state = 1

            self.acc &= state

        self.ip += 1

    def logicOr(self):
        name=self.prog[self.ip][1]

        sql = "select state from io_point where name = '%s'" % name

        if self.verbose:
            print("\t " + sql )

        self.cur.execute( sql )
        data = self.cur.fetchone()
        print("\t " + data[0])

        state = self.strToLogic(data[0])

        if state < 2:
            self.acc |= state
        self.ip += 1


    def ld(self):
        name=self.prog[self.ip][1]

        sql = "select state from io_point where name = '%s'" % name

        if self.verbose:
            print("\t " + sql )

        self.cur.execute( sql )
        data = self.cur.fetchone()
        print("\t " + data[0])

        self.acc = self.strToLogic(data[0])
        self.ip += 1


    def logicOut(self):

        name=self.prog[self.ip][1]

        print(name)

        sql = "select state from io_point where name = '%s'" % name

        if self.verbose:
            print("\t " + sql )

        self.cur.execute( sql )
        data = self.cur.fetchone()

        if self.verbose:
            print("\t " + data[0])

        state=self.logicToStr( self.acc )
        self.acc=0;

        if state == "UNKNOWN":
            pass
        else:
            sql = "update io_point set state = '%s' where name = '%s'" % (state,name)

            if self.verbose:
                print("\t " + sql)

            self.cur.execute( sql )
            self.con.commit(  )
        self.ip += 1



    def run(self):
        print("Run logic.")

        runFlag=True

        while runFlag:
            if self.verbose:
                print(self.ip, end="")

                if len( self.prog[self.ip]) == 0:
                    pass
                elif len( self.prog[self.ip]) == 1:
                    print(" %s" % self.prog[self.ip][0])
                else:
                    print(" %s %s" % (self.prog[self.ip][0], self.prog[self.ip][1]))

            if  (self.prog[self.ip][0]).upper() == 'LD':
                self.ld()
            elif  (self.prog[self.ip][0]).upper() == 'ANDN':
                self.logicAndNot()
            elif  (self.prog[self.ip][0]).upper() == 'OR':
                self.logicOr()
            elif  (self.prog[self.ip][0]).upper() == 'OUT':
                self.logicOut()
            elif  (self.prog[self.ip][0]).upper() == 'END':
                self.end()
                runFlag=False

            if self.verbose:
                print("acc = %d" % self.acc)
                print("ip  = %d" % self.ip)


    def verbose(self):
        print("Verbose")
        self.msg=True

    def quiet(self):
        self.msg=False


    def dump(self):
        print("\tLogicVM Dump")
        print("Database : " + self.db)
        print("pdir     : " + self.pdir)
        print("Config   : " + self.configFile)

    def __del__(self):
        print("Destructor")
        self.con.commit()
        self.con.close()


# if __name__ == "__main__":
#     machine = logicVM()
#     machine.verbose()
#     machine.dump()
# 
#     if machine.load():
#         print("Load failed")
#         sys.exit(3)
# 
#     machine.run()


