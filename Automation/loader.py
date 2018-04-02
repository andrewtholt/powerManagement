#!/usr/bin/env python3

import sys
import sqlite3 as sqlite
import getopt
import os.path
from os import getenv


verbose=False

def usage():
    print("Usage: loader.py ...")
    print("\t-h|--help\tHelp.")
    print("\t-v|--verbose\tverbose.")
    print("\t-c|--clean\tCreate a new empty database to replace any existing data.")
    print("\t-i <file>|--init=<file>")
    print("\t\t\tPopulate the database.")

def cleanUp(db,pdir):
    if verbose:
        print("Cleaning database " + db)

    try:
        os.unlink(db)
    except:
        pass

    if os.path.exists(pdir +"/setup.sql"):
        if verbose:
            print("SQL setup exists")

    cmd = "sqlite3 %s < %s" % ( db,(pdir + "/setup.sql"))

    if os.system( cmd ) != 0:
        print("FATAL ERROR:%s failed" % cmd)
        sys.exit(1)
    else:
        if verbose:
            print("%s success" % cmd)




def initDb(databaseName, initFile):
    global verbose

    if verbose:
        print("Initialize database....")
        print("... from " + initFile + " ...")

    if os.path.exists( initFile):
        if verbose:
            print("... " + initFile + " exists ...")

    try:
        con = sqlite.connect( databaseName )
        cur = con.cursor()

    except:
        print("\nProblem with database, re-run with -v")
        sys.exit(1)

    tmp=open(initFile).readlines()
    data=map(str.strip,tmp)

    sql = "delete from io_point"
    if verbose:
        print("... Empty tables ...")
        print(">> " + sql )

    cur.execute( sql )
    con.commit()

    print()
    for n in data:
        d = n.split(":")

        name      = d[0]
        topic     = d[1]
        direction = d[2]

        state     = d[3]
        if len(state) == 0:
            state = 'UNKNOWN'

        on_state  = d[4]
        if len(on_state) == 0:
            on_state = 'ON'

        off_state = d[5]
        if len(off_state) == 0:
            off_state = 'OFF'

        if verbose:
            print("Name     : " + name )
            print("Topic    : " + topic )
            print("Direction: " + direction)
            print("State    : " + state)
            print("On  State: " + on_state)
            print("Off State: " + off_state)
        sql = "insert into io_point (name, topic,direction,state) values ('%s','%s','%s','%s'); " % (name,topic,direction,state)

        if verbose:
            print(">> " + sql )
        cur.execute( sql )

    con.commit()

def main():

    global verbose

    dbName=None
    clean=False
    initFile=None

    pdir = "."
    dbDir = None

    dbName = getenv("CTL_DB")
    if dbName != None:
        dbDir = os.path.dirname(dbName)

    pdir = getenv("CTL_PDIR")
    if dbName == None or pdir == None or dbDir == None:
        print( "FATAL ERROR: setup CTL_PDIR & CTL_DB env variables")
        sys.exit(1)

    try:
        opts,args = getopt.getopt(sys.argv[1:], "cd:hvi:",["clean","database","help","verbose","init"])
    except:
        print("Argument error")
        usage()
        sys.exit(2)

    for o,a in opts:
        if o in ("-d","--database"):
            dbName = a
            if verbose:
                print("Database         : " + dbName )
        elif o in ("-i","--init"):
            initFile=a
        elif o in ("-c","--clean"):
            clean=True
        elif o in ("-v","--verbose"):
            verbose=True
            print("Verbose")
        elif o in ("-h","--help"):
            usage()

    if verbose:
        print("Database is " + dbName)

    if clean:
        if verbose:
            print("Clean:remove the existing database")
        cleanUp(dbName, pdir)

    if initFile != None:
        initDb(dbName, initFile)

main()

