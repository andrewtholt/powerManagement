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
    print "\nUsage: discover -a -s <subnet> -v -h\n"
    print "\t-a\t\tAdd new machines to database."
    print "\t-s <subnet>\tScan subnet, e.g. 10.0.1.0/24"
    print "\t-h\t\tHelp\n"
    print "\t-v\tVerbose\n"

def main():
    verbose = False
    test =  False
    add = False
    sqlList = []

    db = getenv("POWER_DB")
    pdir = getenv("PDIR")

    subnet=""

    if db == None or pdir == None:
        print "FATAL ERROR: setup PDIR & POWER_DB env variables"
        sys.exit(1)

    try:
        opts,args = getopt.getopt(sys.argv[1:], "as:hv",['subnet','help','verbose'])
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
        elif o in ("-a","--add"):
            add = True

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

    cmd = "toSpread -c %s/data/.start.rc" % pdir

    if verbose:
        print cmd

    bufsize=1024;

    p = subprocess.Popen(cmd, shell=True, bufsize=bufsize, stdin=subprocess.PIPE, stdout=subprocess.PIPE, close_fds=False)

    (child_stdin, child_stdout) = (p.stdin, p.stdout)

    line = child_stdout.readline()
    print line

    child_stdin.write("^set USER discover\n")
    child_stdin.flush()

    child_stdin.write("^connect\n")
    child_stdin.flush()

    line = child_stdout.readline()
    print line

    con = sqlite.connect( db )
    cur = con.cursor()

    cmd = "fping -ad -g %s 2> /dev/null" % subnet

    if verbose:
        print cmd

    data = popen( cmd )

    while True:
        line = (data.readline()).strip()
#
        if line == '':
            break
#
        if line[0].isdigit():
            name = False
        else:
            name = True

        row = 0
        mac = 'UNKNOWN'

        if row == 0:
            print"Unknown host:%s" % line

            if name:
                dns=socket.gethostbyaddr(line)
                address=dns[2][0]

                cmd = "arp -a | grep %s " % line
                print cmd
                f=popen( cmd )
                stuff = ((f.readline()).strip()).split()
                print stuff

                if len(stuff) >= 4:
                    mac=stuff[3]

                child_stdin.write("addhost %s %s %s\n" % (line, address, mac))
            else:
                cmd = "arp -a | grep '(%s)' " % line
                print cmd
                f=popen( cmd )
                stuff = ((f.readline()).strip()).split()
                print stuff

                if len(stuff) >= 4:
                    mac=stuff[3]

                child_stdin.write("newhost %s %s\n" % (line, mac))

            child_stdin.flush()



main()
