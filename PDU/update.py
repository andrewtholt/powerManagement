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
from socket import getaddrinfo,gaierror

import os.path
from os import getenv,popen,remove,system
from time import sleep

def executeSql(cur,sql):
    failed=True

    while failed:
        try:
            cur.execute( sql )
#            self.con.commit()
            failed=False
        except sqlite.OperationalError:
            print "Database Locked."
            failed=True
            sleep(01)
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

    con = sqlite.connect( db )
    cur = con.cursor()
    
    sql = "select name, ip, status from hosts; "

    cur.execute(sql)

    dnsFailed=True
    try:
        getaddrinfo(name,22)
        dnsFailed=False
    except:
        dnsFailed=True

    for r in cur.fetchall():
        name=r[0]
        ip=r[1]
        status = r[2]

        if dnsFailed:
            cmd = "fping -c 2 %s > /dev/null 2>&1" % ip
        else:
            cmd = "fping -c 2 %s > /dev/null 2>&1" % name

        if verbose:
            print cmd


        ret=system( cmd )
        if ret == 0:
            state = 'UP'
        else:
            state = 'DOWN'

        if dnsFailed:
            sqlCmd = "update hosts set status='%s' where ip='%s';" % ( state, ip )
        else:
            sqlCmd = "update hosts set status='%s' where name='%s';" % ( state, name )

        if verbose:
            print sqlCmd
        
        executeSql(cur, sqlCmd)
#        cur.execute(sqlCmd)
            
    con.commit()
    con.close()



main()






