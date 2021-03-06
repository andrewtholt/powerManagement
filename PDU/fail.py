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
    
    sql = "update outlets set pf_state=state where start_state='RESTORE';"
    cur.execute(sql)
    con.commit()

    sql="update outlets set requested_state=pf_action where pf_action <> 'NONE';"
    cur.execute(sql)
    con.commit()

    con.close()



main()


# On power failure copy current state to pf_state
# update outlets set pf_state=state where start_state='RESTORE';
# 
# Set the requested state to the state required on power fail
# update outlets set requested_state=pf_action where pf_action <> 'NONE';
