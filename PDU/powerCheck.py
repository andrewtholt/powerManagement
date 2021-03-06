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

def executeSql(cur,sql):
    failed = True
    while failed:
        try:
            cur.execute( sql )
            failed=False
        except sqlite.OperationalError:
            print "Database Locked."
            failed=True
            sleep(1)
    return

def main():

    verbose=False
    if len(sys.argv) > 1:
        verbose=True

    db = getenv("POWER_DB")

    pdir = getenv("PDIR")

    if db == None or pdir == None:
        print "FATAL ERROR: setup PDIR & POWER_DB env variables"
        sys.exit(1)

    print "Connecting to ", db
    con = sqlite.connect( db )
    cur = con.cursor()
    
    sql = """select 
        hosts.name,outlets.name,outlets.state,hosts.ro_community,outlets.oid,
        hosts.on_value,hosts.off_value,hosts.rw_community,hosts.status,
        hosts.ip
        from hosts,outlets where hosts.idx=outlets.hostidx and hosts.status='UP';"""

    if verbose:
        print sql

#    cur.execute(sql)
    executeSql(cur,sql)
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
        ip=r[9]
        
        if hostStatus != "DOWN":
            cmd = "snmpget -OvQ -t 10 -v1 -c %s %s %s 2> /dev/null" % ( ro, ip,oid)
            if verbose:
                print cmd
        
            t = popen( cmd )
            ret =  int( t.readline() )
        
            cState="UNKNOWN"
            if ret == int(onValue):
                cState="ON"
            elif ret == int(offValue):
                cState="OFF"

#            print "Ret value ",ret
#            print "On Value  ",int(onValue)
#            print "Off Value ",int(offValue)

            print "Host %s Host status %s" % ( outletName,cState )
            sqlCmd = "update outlets set state='%s' where name='%s';" % ( cState, outletName )

            if verbose:
                print sqlCmd
                print "===================================================="

            executeSql(cur,sql)
#            cur.execute(sqlCmd)

            sqlCmd = "update outlets set pf_state='%s' where name='%s' and start_state='RESTORE';" % ( cState, outletName )

            if verbose:
                print sqlCmd
                print "===================================================="

#            executeSql(cur,sql)
            cur.execute(sqlCmd)
            
    con.commit()
    con.close()



main()






