#!/usr/bin/env python

import sys
import getopt
import sqlite3 as sqlite
import os.path
from os import getenv,popen,remove,path
from time import sleep
import socket
import subprocess

def usage():
    print "\nUsage: spreadDiscover -u <user> -a -s <subnet> -v -h\n"
    print "\t-u <user>\tName to connect to spread with."
    print "\t-s <subnet>\tScan subnet, e.g. 10.0.1.0/24"
    print "\t-h\t\tHelp\n"
    print "\t-v\tVerbose\n"

def main():
    verbose = False
    test =  False
    add = False
    sqlList = []
    user='discover'

    db = getenv("POWER_DB")
    pdir = getenv("PDIR")

    subnet=""

    if db == None or pdir == None:
        print "FATAL ERROR: setup PDIR & POWER_DB env variables"
        sys.exit(1)

    try:
        opts,args = getopt.getopt(sys.argv[1:], "u:s:hv",['user','subnet','help','verbose'])
    except:
        print "Argument Error"
        usage()
        sys.exit(1)

    for o,a in opts:
        if o in ("-h","--help"):
            usage()
            sys.exit(0)
        elif o in ("-s","--subnet"):
            subnet = a
        elif o in ("-v","--verbose"):
            verbose = True
        elif o in ("-u","--user"):
            user = a

    if verbose:
        print"Subnet :",subnet

    if subnet == "" :
        usage();
        sys.exit(1)

    if path.isfile("/usr/local/bin/toSpread"):
        print "toSpread exists."
    else:
        print "toSpread not found."
        sys.exit(10)

    cmd = "toSpread -u %s -c %s/data/.start.rc" % (user, pdir )

    if verbose:
        print cmd

    bufsize=1024;

    p = subprocess.Popen(cmd, shell=True, bufsize=bufsize, stdin=subprocess.PIPE, stdout=subprocess.PIPE, close_fds=False)

    (child_stdin, child_stdout) = (p.stdin, p.stdout)

    line = child_stdout.readline()
    print line

    child_stdin.write("^connect\n")
    child_stdin.flush()

    line = child_stdout.readline()
    print line

#    con = sqlite.connect( db )
#    cur = con.cursor()

#    cmd = "fping -ad -g %s 2> /dev/null" % subnet

#    if verbose:
#        print cmd

#    data = popen( cmd )

    child_stdin.write("listhost ip %s\n" % subnet )
    child_stdin.flush()

    while True:

        line = (child_stdout.readline()).strip()

        tmp = line.split()

        if tmp[1] != "ip":
            print tmp
            cmd = "fping -ad -c 2 " + tmp[1]
            print cmd
            data = popen( cmd )

            line = (data.readline()).strip()
            print line
#
#



main()
