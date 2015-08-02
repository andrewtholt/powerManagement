#!/usr/bin/env python

import sys
import getopt
import sqlite3 as sqlite
import os.path
from os import getenv,popen,remove
from time import sleep
import socket
import subprocess

def usage():
    print "\nUsage: discover -a -s <subnet> -v -h\n"
    print "\t-u <user>\tName to connect top spread with."
    print "\t-h\t\tHelp\n"
    print "\t-v\tVerbose\n"

def main():
    verbose = False
    test =  False
    add = False
    sqlList = []

    db = getenv("POWER_DB")
    pdir = getenv("PDIR")

    bufsize=1024
    user = "populate"

    if db == None or pdir == None:
        print "FATAL ERROR: setup PDIR & POWER_DB env variables"
        sys.exit(1)

    try:
        opts,args = getopt.getopt(sys.argv[1:], "u:hv",['user','help','verbose'])
    except:
        print "Argument Error"
        usage()
        sys.exit(1)

    for o,a in opts:
        if o in ("-h","--help"):
            usage()
            sys.exit(0)
        elif o in ("-u","--user"):
            user = a
        elif o in ("-v","--verbose"):
            verbose = True

    if verbose:
        print"User   :",user

    cmd = "toSpread -u %s -c %s/data/.start.rc" % ( user,pdir )

    if verbose:
        print cmd

    p = subprocess.Popen(cmd, shell=True, bufsize=bufsize, stdin=subprocess.PIPE, stdout=subprocess.PIPE, close_fds=False)

    (child_stdin, child_stdout) = (p.stdin, p.stdout)
    line = child_stdout.readline()
#    print line

    child_stdin.write("^connect\n")
    child_stdin.flush()

    line = child_stdout.readline()
#    print line

    con = sqlite.connect( db )
    cur = con.cursor()

    while True:
        line = child_stdout.readline()
        tmp = line.strip()
        data = tmp.split()

#         print data
        l = len(data)

        name = ""
        if data[0] == "addhost":
            if l == 4:
                name=data[1]
                ip=data[2]
                mac=data[3]
            elif len == 3:
                name = data[1]
                ip = data[1]
                mac = data[2]

            sql = "select count(*) from hosts where name = '%s';" % name
            cur.execute( sql )
            res = cur.fetchone()
            count = res[0]

            if count == 0:
                print "INSERT"
                sql = "insert into hosts (name,ip,mac) values ('%s','%s','%s');" % (name,ip,mac) 
            else:
                print "UPDATE"
                sql = "update hosts set status='UP' where name='%s';" % name


            cur.execute( sql )
            con.commit()
            if verbose:
                print sql
                print "==============="
        elif data[0] == 'newhost':
            print "Report"
            print data


#        sql = "select count(*) from hosts where "
#        line = (data.readline()).strip()


    con.commit()
    con.close()



main()
