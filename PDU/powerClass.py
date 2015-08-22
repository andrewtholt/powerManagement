from os import getenv,popen,path
import sqlite3 as sqlite
from time import sleep

class powerClass:
    db = ""
    pdir = ""

    def __init__(self):
#        print "Power Class"
        self.db=getenv("POWER_DB")
        self.pdir=getenv("PDIR")

        self.con = sqlite.connect( self.db )
        self.cur = self.con.cursor()
        return

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

    def on(self,host,delay):
        print "On",host
        request = 'ON'
        #
        # Need a way to test if running on UPS.
        #
        sql = """update outlets 
                set requested_state='%s',touched=datetime(\'NOW\'),
                act_when=datetime(\'NOW\','+%d minutes')
                where locked='NO' and name ='%s';""" % (request,delay, host)

#        print sql

        self.executeSql( sql )
        return

    def off(self,host,delay):
        print "Off",host
        request = 'OFF'

        sql = """update outlets 
                set requested_state='%s',touched=datetime(\'NOW\'),
                act_when=datetime(\'NOW\','+%d minutes')
                where locked='NO' and name ='%s';""" % (request,delay, host)

#        print sql
        self.executeSql( sql )
        return

    def reboot(self,host,delay):
        print "Reboot",host
        request = 'REBOOT'

        sql = """update outlets 
                set requested_state='%s',touched=datetime(\'NOW\'),
                act_when=datetime(\'NOW\','+%d minutes')
                where locked='NO' and name ='%s';""" % (request,delay, host)

#        print sql
        self.executeSql( sql )
        return

    def cancel(self,host):

        if host == 'all':
            sql="update outlets set requested_state='NA'";
        else:
            sql="update outlets set requested_state='NA' where name = '%s';" % host

        print sql
        self.executeSql( sql )
        return

    def status(self,host):
        sql ="select hosts.name,outlets.name,outlets.state from hosts,outlets where hosts.idx = outlets.hostidx and outlets.name = '%s';" % host
        print "Status",host
#        print sql
        self.cur.execute( sql )

        res=self.cur.fetchone()
#        print res[2]
        return res[2]

    def dump(self):
        print "POWER_DB :", self.db
        print "PDIR     :",self.pdir

if __name__ == "__main__":
    x=powerClass()

    x.on("Fans")
