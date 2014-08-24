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

    sql = "select count(*) from outlets where requested_state <> 'NA';"
    cur.execute(sql)
    data = cur.fetchone()

    val = data[0]
    
    con.close()
    sys.exit(val)

main()



