#!/usr/bin/env python 
#
# POWER_DB  Database
# PDIR      Base directory for utilities
#
# Read each defined port and set the actual state in the database.
#
import sys
import getopt
from pysqlite2 import dbapi2 as sqlite
from os import getenv

import os.path
from os import getenv,popen,remove
from time import sleep

def main():

    verbose=False
    if len(sys.argv) > 1:
        verbose=True

    db = getenv("POWER_DB")
#    if db == None:
#        db = "/etc/local/power/data/power.db"

    pdir = getenv("PDIR")
#    if pdir == None:
#        pdir = "/usr/local/apps/power"


    if db == None or pdir == None:
        print "FATAL ERROR: setup PDIR & POWER_DB env variables"
        sys.exit(1)

    con = sqlite.connect( db )
    cur = con.cursor()
    
    sql = """select 
        hosts.name,outlets.name,outlets.state,hosts.ro_community,outlets.oid,
        hosts.on_value,hosts.off_value,hosts.rw_community,hosts.status,
        hosts.ip
        from hosts,outlets where hosts.idx=outlets.hostidx;"""
    cur.execute(sql)
    for r in cur.fetchall():
#        print r[0],r[1],r[2],r[3]
        
        pduName=r[0]
        outletName =r[1]
        outletState = r[2]
        ro=r[3]
        oid=r[4]
        
        onValue=r[5]
        offValue = r[6]
        rw=r[7]
        hostStatus = r[8]

        
        if hostStatus != "DOWN":
            cmd = "snmpget -OvQ -t 10 -v1 -c %s %s %s 2> /dev/null" % ( ro,pduName,oid)
            if verbose:
                print cmd
        
            t = popen( cmd )
            ret =  int( t.readline() )
        
            cState="UNKNOWN"
            if ret == int(onValue):
                cState="ON"
            elif ret == int(offValue):
                cState="OFF"

            print "Host %s Host status %s" % ( cState, outletName )
            sqlCmd = "update outlets set state='%s' where name='%s';" % ( cState, outletName )

            if verbose:
                print sqlCmd
                print "===================================================="

            cur.execute(sqlCmd)

#        if ret == int(onValue):
#            cState="ON"
#            print "%s is ON should be %s" % (outletName, outletState)
#            sqlCmd = "update outlets set state='%s' where name='%s';" % ( cState, outletName )
#            print sqlCmd
#            print "===================================================="

#        elif ret == int(offValue):
#            cState="OFF"
#            print "%s is OFF should be %s" % (outletName,outletState)
#
#        if( cState != outletState):
#            print "%s is %s should be %s" % (outletName, cState,outletState)
#
#            if outletState == 'ON':
#                reqState = onValue
#            elif outletState == 'OFF':
#                reqState = offValue
##            cmd = "snmpset -t 10 -v1 -c %s %s %s i %d > /dev/null 2>&1" % (rw,pduName,oid,reqState)
#            print cmd
#            res =  os.system( cmd )
            
    con.commit()
    con.close()



main()






