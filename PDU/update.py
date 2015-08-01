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
from os import getenv,popen,remove,system
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
    
    sql = "select name, ip, status from hosts; "

    cur.execute(sql)

    for r in cur.fetchall():
        name=r[0]
        ip=r[1]
        status = r[2]

        cmd = "fping -c 2 %s > /dev/null 2>&1" % name

        if verbose:
            print cmd


        ret=system( cmd )
        if ret == 0:
            state = 'UP'
        else:
            state = 'DOWN'

        sqlCmd = "update hosts set status='%s' where name='%s';" % ( state, name )

        if verbose:
            print sqlCmd
        
        cur.execute(sqlCmd)
            
    con.commit()
    con.close()



main()






