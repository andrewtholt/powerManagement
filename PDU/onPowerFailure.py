#!/usr/bin/env python 
#
# POWER_DB  Database
# PDIR      Base directory for utilities
#
# Schedules power off command for those outlets that have a policy defined.
#
import sys
import sqlite3 as sqlite
from os import getenv,path


def main():
    
    db = getenv("POWER_DB")
    if db == None:
        db = "/etc/local/power/data/power.db"

    pdir = getenv("PDIR")
    if pdir == None:
        pdir = "/usr/local/apps/power"


    con = sqlite.connect( db )
    cur = con.cursor()

    sql = "update outlets set requested_state=pf_action,touched=datetime(\'NOW\') where pf_action <> 'NONE';"
    cur.execute( sql )
    con.commit()

    # test for files existence.  If does not exist capture current state.

    if not path.exists( '/var/tmp/powerfail.txt'):
        sql = "update outlets set pf_state=state,touched=datetime(\'NOW\') where start_state = 'RESTORE';"
        cur.execute( sql )
        con.commit()
    # and create file

        open('/var/tmp/powerfail.txt','a').close()
    con.close()


main()

