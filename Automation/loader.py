#!/usr/bin/env python3

import sys
import getopt
import os.path
from os import getenv


verbose=False
dbType="NONE"
pdir="."
dbDir=None
con=None

def usage():
    print("Usage: loader.py ...")
    print("\t-h|--help\tHelp.")
    print("\t-v|--verbose\tverbose.")
    print("\t-c|--clean\tCreate a new empty database to replace any existing data.")
    print("\t-i <file>|--init=<file>")
    print("\t\t\tPopulate the database.")
    print("\t-t <type>|--type=<type>")

def cleanUp(dbName):
    global dbType
    global pdir
    global con

    if dbType == "SQLITE":
        print("SQLITE")

        setupFile = pdir + '/' + "sqliteSetup.sql"

        dbPath = dbDir + '/' + dbName + ".db"

        if verbose:
            print("cleanUp:" + dbPath)

        if os.path.exists( dbPath ):
            if verbose:
                print("cleanUp:removing db File")

            con.close()
            os.remove( dbPath )
            import sqlite3 as sqlite
            con = sqlite.connect(dbPath)

    elif dbType == "MYSQL":
        setupFile = pdir + '/' + "mySqlSetup.sql"


    if verbose:
        print("Cleaning database " )
        print("... And creating anew from " + setupFile)

#    if os.path.exists(pdir +"/mySqlSetup.sql"):
#        if verbose:
#            print("SQL setup exists")
#
        tmp=open(setupFile).readlines()
        data=map(str.strip,tmp)

        cur = con.cursor()
        for sql in data:
            if verbose:
                print(sql)
            if len(sql) > 0:
                cur.execute( sql )

        con.commit()
        cur.close()



def initDb( initFile):
    global verbose
    global con

    if verbose:
        print("Initialize database....")
        print("... from " + initFile + " ...")

    if os.path.exists( initFile):
        if verbose:
            print("... " + initFile + " exists ...")

    tmp=open(initFile).readlines()
    data=map(str.strip,tmp)

    sql = "delete from io_point"
    if verbose:
        print("... Empty tables ...")
        print(">> " + sql )
        print()

    cur = con.cursor()

    try:
        cur.execute( sql )
        con.commit()
    except:
        print("Ooops " + sql )


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
    cur.close()

def main():

    global verbose
    global dbType
    global pdir
    global dbDir
    global con

    dbName=None
    clean=False
    initFile=None

    dbName = getenv("CTL_DB")
    if dbName != None:
        dbDir = os.path.dirname(dbName)

    pdir = getenv("CTL_PDIR")
    dbDir = getenv("CTL_DB")

    if dbName == None or pdir == None or dbDir == None:
        print( "FATAL ERROR: setup CTL_PDIR & CTL_DB env variables")
        sys.exit(1)

    try:
        opts,args = getopt.getopt(sys.argv[1:], "cd:hvi:t:",["clean","database","help","verbose","init","type" ])
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
        elif o in ("-t","--type"):
            dbType = a

    if dbType not in ("MYSQL", "SQLITE"):
        print("You must select a valid database type, options are MYSQL or SQLITE.")
        print()
        usage()
        sys.exit(0)

    if verbose:
        print("Database is " + dbName)


    try:
        if dbType == "MYSQL":
            import pymysql as mysql
            con = mysql.connect("localhost", "automation","automation","automation")
        elif dbType == "SQLITE":
            import sqlite3 as sqlite
            fred = dbDir + "/" + dbName +".db" 
            print("SQLITE db is " + fred )
            con = sqlite.connect(fred)
            print(con)

    except:
        print("\nProblem with database, re-run with -v")
        sys.exit(1)


    if clean:
        if verbose:
            print("Clean:remove the existing database")
        cleanUp(dbName)

    if initFile != None:
        initDb(initFile)

main()

