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
    print "\nUsage: spreadAdd.py -u <user> -v -h\n"
    print "\t-u <user>\tName to connect top spread with."
    print "\t-h\t\tHelp"
    print "\t-v\t\tVerbose\n"
    print
    print "\tdefault beahaviour:"
    print "\t\tspreadAdd.py -u populate\n"

    print "\tSee also toSpread -h\n"

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

    child_stdin.write("^set SHOW_SENDER true\n")
    child_stdin.flush()

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

#        print data
        l = len(data)

        name = ""
        sender = data[0]
        request = data [1]

        if verbose:
            print "Sender : " + sender
            print "Request: " + request

        if request == "addhost":
            if l == 5:
                name=data[2]
                ip=data[3]
                mac=data[4]
            elif len == 4:
                name = data[2]
                ip = data[3]
                mac = data[4]

            sql = "select count(*) from hosts where name = '%s';" % name
            cur.execute( sql )
            res = cur.fetchone()
            count = res[0]

            if count == 0:
                print "INSERT"
                sql = "insert into hosts (name,ip,mac,status) values ('%s','%s','%s','UP');" % (name,ip,mac) 
            else:
                print "UPDATE"
                sql = "update hosts set status='UP' where name='%s';" % name


            cur.execute( sql )
            con.commit()
            if verbose:
                print sql
                print "==============="
        elif request == 'listhost':
            what = data[2]  # name or ip
            selector = data[3] # name or IP pattern e.g. name www-% or ip 192.168.0.%

            sql = "select name from hosts where "

            if what == 'name':
                sql += "name like "
            elif what == "ip":
                sql += "ip like "

            sql +="'%s';" % selector


            if verbose:
                print sql

            cur.execute( sql )
            res = cur.fetchall()
            for r in res:
                response = '^send ' + sender + " " + request + " "
                response += (' ' + r[0] ) 

                if verbose:
                    print response
                child_stdin.write("%s\n" % response )
                child_stdin.flush()

            child_stdin.write("^send " + sender + " " + request + " -END-\n")
            child_stdin.flush()

        elif request == 'update':
            name = data[2]
            state = data[3]
            sql = "update hosts set status='%s' where name='%s';" % (state,name)

            if verbose:
                print "Update"
                print "Name  :" + name
                print "State :" + state
                print sql
                print "=========================="

        elif request == 'newhost':
            print "Report"
            print data[0]


#        sql = "select count(*) from hosts where "
#        line = (data.readline()).strip()


    con.commit()
    con.close()



main()
