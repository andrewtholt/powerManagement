#!/usr/bin/env python

import sys
import getopt
import sqlite3 as sqlite
import os.path
from os import getenv,popen,remove
from time import sleep

def usage():
    print "Help\n\n"

def main():
    verbose = False
    test =  False

    db = getenv("POWER_DB")
    pdir = getenv("PDIR")

    address=""
    hostName=""

    if db == None or pdir == None:
        print "FATAL ERROR: setup PDIR & POWER_DB env variables"
        sys.exit(1)

    try:
        opts,args = getopt.getopt(sys.argv[1:], "a:n:hv",['address','name','help','verbose'])
    except:
        print "Argument Error"
        usage()
        sys.exit(1)

    for o,a in opts:
        if o in ("-h","--help"):
            usage()
            sys.exit(0)
        elif o in ("-a","--address"):
            address = a
        elif o in ("-n","--name"):
            hostName = a
        elif o in ("-v","--verbose"):
            verbose = True

    if verbose:
        print"Hostname:",hostName
        print"Address :",address

    if address == "" and hostName == "":
        usage();
        sys.exit(1)

    sql = "select count(*) from hosts where "

    if hostName != "":
        sql += "name = '%s' " % hostName

    if address != "" and hostName != "":
        sql += " and "

    if address != "":
        sql += "ip = '%s' " % address

    sql += ";"

    if verbose:
        print sql

    con = sqlite.connect( db )
    cur = con.cursor()

    cur.execute(sql)

    row=cur.fetchone()

    if row[0] == 0:
        if verbose:
            print "Not in database."

        sys.exit(1);
    else:
        if verbose:
            print "In database."
        sys.exit(0);


main()
