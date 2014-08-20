#!/usr/bin/env python 
#
# POWER_DB  Database
# PDIR      Base directory for utilities
#
# Basic template to modify for a new command.
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

    pdir = getenv("PDIR")

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
        ip=r[9]
        
# Set sql command here
        cur.execute(sqlCmd)
            
    con.commit()
    con.close()



main()






