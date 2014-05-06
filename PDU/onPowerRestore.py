#!/usr/bin/env python 
#
# POWER_DB  Database
# PDIR      Base directory for utilities
#
# Schedules power off command for those outlets that have a policy defined.
#
import sys
import sqlite3 as sqlite
from os import getenv,path,remove


def main():
    
    db = getenv("POWER_DB")

    pdir = getenv("PDIR")

    con = sqlite.connect( db )
    cur = con.cursor()

    if path.exists( '/var/tmp/powerfail.txt'):
        print "Power Failed."
#        remove('/var/tmp/powerfail.txt')
#        sql="update outlets set pf_state='UNKNOWN';"
#        cur.execute( sql )
#        con.commit()


    sql = "update outlets set requested_state=pf_action,touched=datetime(\'NOW\') where pf_action <> 'NONE';"
    cur.execute( sql )
    con.commit()
    con.close()


main()

