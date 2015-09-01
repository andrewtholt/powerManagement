#!/usr/bin/env python 
#
# POWER_DB  Database
# PDIR      Base directory for utilities
#
#
import sys
import getopt
from pysqlite2 import dbapi2 as sqlite
from os import getenv

import os.path
from os import getenv,popen,remove
from time import sleep

def usage():

    print "\nUsage:purge <host|all>"
    print "\tRemove the named host, or all, marked as down in the database.\n"

def main():

    verbose=False
    if len(sys.argv) > 1:
        sql = "delete from hosts where status='DOWN'"
        host = sys.argv[1]
        if host == "all":
            sql += ";"
        else:
            sql += " and name=%s;" % host
    else:
        usage()
        sys.exit(0)


    db = getenv("POWER_DB")

    pdir = getenv("PDIR")

    if db == None or pdir == None:
        print "FATAL ERROR: setup PDIR & POWER_DB env variables"
        sys.exit(1)

    con = sqlite.connect( db )
    cur = con.cursor()
    
    cur.execute(sql)

    con.commit()
    con.close()



main()






