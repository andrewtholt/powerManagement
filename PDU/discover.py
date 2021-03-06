#!/usr/bin/env python3

import sys
import getopt
import sqlite3 as sqlite
import os.path
from os import getenv,popen,remove
from time import sleep
import socket

def usage():
    print("\nUsage: discover -a -s <subnet> -v -h\n")
    print("\t-a\t\tAdd new machines to database.")
    print("\t-s <subnet>\tScan subnet, e.g. 10.0.1.0/24")
    print("\t-h\t\tHelp\n")
    print("\t-v\tVerbose\n")

def main():
    verbose = False
    test =  False
    add = False
    sqlList = []

    db = getenv("POWER_DB")
    pdir = getenv("PDIR")

    subnet=""

    if db == None or pdir == None:
        print("FATAL ERROR: setup PDIR & POWER_DB env variables")
        usage()
        sys.exit(1)

    try:
        opts,args = getopt.getopt(sys.argv[1:], "as:hv",['subnet','help','verbose'])
    except:
        print("Argument Error")
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
        print("Subnet :",subnet)

    if subnet == "" :
        usage();
        sys.exit(1)

    con = sqlite.connect( db )
    cur = con.cursor()

    # Are we on the internet ?
    # Try pinging Googles public DNS

    rc=os.system("fping 8.8.8.8")
    if rc == 0: # Internet available
        cmd = "fping -ad -g %s 2> /dev/null" % subnet
    else:
        cmd = "fping -a -g %s 2> /dev/null" % subnet

    if verbose:
        print(cmd)

    data = popen( cmd )

    while True:
        sql = "select count(*) from hosts where "
        line = (data.readline()).strip()

        if line == '':
            break

        if line[0].isdigit():
            name = False
            sql += " ip = '%s' ;" % line
        else:
            name = True
            sql += " name = '%s' ;" % line

        if verbose:
            print(sql)

        cur.execute(sql)
        row=(cur.fetchone())[0]

        if add == False:
            if row == 0:
                print("Unknown host:%s" % line)
        else:
            mac='UNKNOWN'
            if row == 0:
                if name:
                    cmd = "arp -a %s" % line
                else:
                    cmd = "arp -a | grep '(%s)' " % line

                print(cmd)
                f=popen( cmd )
                stuff = ((f.readline()).strip()).split()
                print(stuff)

                if len(stuff) >= 4:
                    mac=stuff[3]

                sql = "insert into hosts "
                if name:
                    dns=socket.gethostbyaddr(line)
                    address=dns[2][0]
                    sql += "( name,ip,mac,status ) values ( '%s','%s','%s','UP' );" % (line,address,mac)
                else:
                    sql += "( name, ip,mac,status ) values ( '%s','%s','%s','UP' );" % (line,line,mac)

            else:
                sql = "update hosts set status = 'UP' where "
                if name:
                    sql += " name = '%s' ;" % line
                else:
                    sql += " ip = '%s' ;" % line
                    

            sqlList.append( sql )
            if verbose:
                print(sql)

    for sql in sqlList:
        if verbose:
            print(sql)

        cur.execute(sql)

    con.commit()
    con.close()



main()
