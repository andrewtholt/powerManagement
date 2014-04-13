#!/usr/bin/env python

import sys
import sqlite3 as sqlite
import os.path
from os import getenv

def main():
    
    locked="NO"

    if os.path.basename( sys.argv[0] ) == "lock":
        locked="YES"
    elif os.path.basename( sys.argv[0] ) == "unlock":
        locked="NO"
    else:
        print "ERROR"

    db = getenv("POWER_DB")
    if db == None:
        db = "/etc/local/power/data/power.db"

    pdir = getenv("PDIR")
    if pdir == None:
        pdir = "/usr/local/apps/power"


    if db == None or pdir == None:
        print "FATAL ERROR: setup PDIR & POWER_DB env variables"
        sys.exit(1)

    try:
        port =  sys.argv[1]

        if port == "all":
            sql = "update outlets set locked='%s';" % locked
        else:
            sql = "update outlets set locked='%s' where name='%s';" % (locked,port)

        con = sqlite.connect( db )
        cur = con.cursor()
        cur.execute( sql)
        con.commit()
        con.close()

    except:
        print "Arg error"

main()

