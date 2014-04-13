#!/usr/bin/env python 
#
# POWER_DB  Database
# PDIR      Base directory for utilities
#
import sys
import getopt
import sqlite3 as sqlite
import os.path
from os import getenv,popen,remove
from time import sleep

from string import strip

def usage():
    print "Usage: act -s <d>|--suspend <d> -t|--test -h|--help -v|--verbose"

def main():
    cmd = ""
    
    verbose = False
    test = False
    clean = False
    suspend = 0
    initFile = None
    lock = "/tmp/act_lock.txt"

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
        opts,args = getopt.getopt(sys.argv[1:], "s:thv",["suspend","test","help","verbose"])
    except:
        print "Argument Error"
        usage()
        sys.exit(2)

    for o,a in opts:
        if o in ("-h","--help"):
            usage()
            sys.exit(0)
        elif o in ("-t","--test"):
            verbose = True
            test = True
        elif o in ("-v","--verbose"):
            verbose = True
        elif o in ("-s","--suspend"):
            suspend=int(a)
            if suspend != 0:
                suspend = ((suspend + 1)/30) * 30

    if suspend > 0:
        if verbose:
            print "... suspend : %d ..." % suspend
        sleep( suspend )
        
    while os.path.exists(lock):
        if verbose: print "...LOCKED..."
        sleep(10)


    (open(lock,"w")).close()

    
    if verbose: print "...UNLOCKED..."
    con = sqlite.connect( db )
    cur = con.cursor()

    sql = "select name from hosts;"
    cur.execute(sql)

    for node in  cur.fetchall():
        if verbose: print "============================"

        cmd = "ping -c 2 -W 5 %s > /dev/null 2>&1" % node[0]

        res = os.system( cmd )

        sql = "select ping_count,ping_counter from hosts where name='%s';" % node

        if verbose: print sql

        cur.execute(sql)


        data = cur.fetchone()
        count = int(data[0])
        counter = int(data[1])

        if res == 0:
            if counter > 0:
                counter = counter -1
                sql = "update hosts set status='DOWN',ping_counter=%d,touched=datetime(\'NOW\') where name='%s';" % (counter,node[0])

                if verbose: print sql

                cur.execute(sql)
                con.commit()
            else:
                sql = "update hosts set status='UP',ping_counter=-1,touched=datetime(\'NOW\') where name='%s';" % node
        else:
            if counter < 0:
                counter = count

            sql = "update hosts set status='DOWN',ping_counter=%d,touched=datetime(\'NOW\') where name='%s';" % (counter,node[0])

        if verbose:
            print cmd
            print sql

        cur.execute(sql)
        con.commit()
        if verbose: print "============================"

    sql = """select hosts.name,
        hosts.on_value, hosts.off_value,hosts.reboot_value,
        outlets.name,outlets.oid,requested_state,hosts.rw_community,hosts.ro_community
        from 
        hosts,outlets 
        where hosts.status='UP' and hosts.idx = outlets.hostidx and outlets.requested_state <> 'NA';"""

    if verbose: print sql

    cur.execute( sql )

    for r in cur.fetchall():
        pduName = r[0]
        onValue = r[1]
        offValue = r[2]
        rebootValue = r[3]
        outletName = r[4]
        oid = r[5]
        requestedState = r[6]
        rw = r[7]
        ro = r[8]

        if verbose:
            print "================"
            print "PDU Name        : %s" % pduName
            print "Outlet Name     : %s" % outletName
            print "ON Value        : %s" % onValue
            print "OFF Value       : %s" % offValue
            print "REBOOT Value    : %s" % rebootValue
            print "OID             : %s" % oid
            print "Requested State : %s" % requestedState
            print "rw community    : %s" % rw
            print "ro community    : %s" % ro

        if requestedState == "ON":
            tmp = onValue
        elif requestedState == "REBOOT":
            tmp = rebootValue
        else:
            tmp = offValue
        #
        # Set output to requested state.
        #
        cmd = "snmpset -t 10 -v1 -c %s %s %s i %d > /dev/null 2>&1" % (rw,pduName,oid,tmp)

        if verbose: print cmd

        res = 0
        if not test:
            res =  os.system( cmd )
        #
        # Read output to confirm change.
        #

        if tmp == rebootValue:
            res = 0
            ret = rebootValue
            requestedState = "ON"
        else:
            cmd = "snmpget -OvQ -t 10 -v1 -c %s %s %s 2> /dev/null" % ( ro,pduName,oid)

            if verbose: print cmd
        
            if not test and res==0:
                t = os.popen(cmd)

                ret =  int( t.readline() )

        #
        # If state has changed the
        # Update database,set state to output state (ON/OFF) and requested state to NA
        # otherwise
        # Leave the request in the database for next time.
        #
        if ret == tmp:
           sql = "update outlets set state='%s',requested_state='NA',touched=datetime(\'NOW\') where name = '%s';" % ( requestedState, outletName)

           if verbose: print sql

           cur.execute(sql)
           con.commit()

    con.commit()
    con.close()

    remove(lock)
    
    if suspend == 0:
        sys.exit(-1)
    else:
        sys.exit(0)
        
main()
