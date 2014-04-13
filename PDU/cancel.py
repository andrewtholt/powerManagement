#!/usr/bin/env python
#
# POWER_DB  Database
# PDIR      Base directory for utilities
#
# Cancel all pending power state changes.
#
import sys
import getopt
from pysqlite2 import dbapi2 as sqlite

import os.path
from os import getenv,popen,remove
from time import sleep

def main():
    db = getenv("POWER_DB")
    if db == None:
        db = "/etc/local/power/data/power.db"

    pdir = getenv("PDIR")
    if pdir == None:
        pdir = "/usr/local/apps/power"


    if db == None or pdir == None:
        print "FATAL ERROR: setup PDIR & POWER_DB env variables"
        sys.exit(1)

    con = sqlite.connect( db )
    cur = con.cursor()

    sql="update outlets set requested_state='NA';"
    print sql
    cur.execute(sql)
    con.commit()
    con.close()

main()