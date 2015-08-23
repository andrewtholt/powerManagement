from os import getenv,popen,path
import sqlite3 as sqlite
from time import sleep

class powerClass:
    db = ""
    pdir = ""
    verb=False
    hostName = ""

    def __init__(self,host):
#        print "Power Class"
        self.db=getenv("POWER_DB")
        self.pdir=getenv("PDIR")

        self.con = sqlite.connect( self.db )
        self.cur = self.con.cursor()
        self.verb=False
        self.hostName=host
        return

    def verbose(self,flag):
        self.verb=flag

        if self.verb:
            print "set Verbose"


    def executeSql(self,sql):
        failed=True

        while failed:
            try:
                self.cur.execute( sql )
                self.con.commit()
                failed=False
            except sqlite.OperationalError:
                print "Database Locked"
                failed=True
                sleep(1)

        return

    def on(self,delay):
        request = 'ON'
        #
        # Need a way to test if running on UPS.
        #
        sql = """update outlets 
                set requested_state='%s',touched=datetime(\'NOW\'),
                act_when=datetime(\'NOW\','+%d minutes')
                where locked='NO' and name ='%s';""" % (request,delay, self.hostName)

        if self.verb:
            print "On",self.hostName
            print sql

        self.executeSql( sql )
        return

    def off(self,delay):
        request = 'OFF'

        sql = """update outlets 
                set requested_state='%s',touched=datetime(\'NOW\'),
                act_when=datetime(\'NOW\','+%d minutes')
                where locked='NO' and name ='%s';""" % (request,delay, self.hostName)

        if self.verb:
            print "Off",self.hostName
            print sql

        self.executeSql( sql )
        return

    def reboot(self,delay):
        request = 'REBOOT'

        sql = """update outlets 
                set requested_state='%s',touched=datetime(\'NOW\'),
                act_when=datetime(\'NOW\','+%d minutes')
                where locked='NO' and name ='%s';""" % (request,delay, self.hostName)

        if self.verb:
            print "Reboot",self.hostName
            print sql

        self.executeSql( sql )
        return

    def cancel(self):
        if self.hostName == 'all':
            sql="update outlets set requested_state='NA'";
        else:
            sql="update outlets set requested_state='NA' where name = '%s';" % self.hostName

        if self.verb:
            print "Cancel"
            print sql

        self.executeSql( sql )
        return

    def status(self):
        sql ="select hosts.name,outlets.name,outlets.state from hosts,outlets where hosts.idx = outlets.hostidx and outlets.name = '%s';" % self.hostName
        print "Status",self.hostName


        if self.verb:
            print "Status"
            print sql

        self.cur.execute( sql )

        res=self.cur.fetchone()

        return res[2]

    def dump(self):
        print "POWER_DB :", self.db
        print "PDIR     :",self.pdir

if __name__ == "__main__":
    x=powerClass()

    x.on("Fans")
