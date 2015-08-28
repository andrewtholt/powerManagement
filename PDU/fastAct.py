#!/usr/bin/env python 
#
# POWER_DB  Database
# PDIR      Base directory for utilities
#
# Basic template to modify for a new command.
#
import sys
import getopt
from pysqlite2 import dbapi2 as sqlite
from os import getenv

import os.path
from os import getenv,popen,remove
from time import sleep

def main():

    verbose=False
    test=False

    if len(sys.argv) > 1:
        verbose=True

    db = getenv("POWER_DB")

    pdir = getenv("PDIR")

    if db == None or pdir == None:
        print "FATAL ERROR: setup PDIR & POWER_DB env variables"
        sys.exit(1)

    con = sqlite.connect( db )
    cur = con.cursor()

    sql = """select hosts.name,
            hosts.on_value, hosts.off_value,hosts.reboot_value,
            outlets.name,outlets.oid,requested_state,hosts.rw_community,hosts.ro_community,
            hosts.ip from 
            hosts,outlets 
            where hosts.status='UP' and 
            hosts.idx = outlets.hostidx and 
            act_when < datetime('now') and 
            outlets.requested_state <> 'NA' and
            (hosts.type='apc' or hosts.type='cyc')
            ;"""
# Add select on hosts.type='apc' or hosts.type='cyc'

#    sql = """select 
#        hosts.name,outlets.name,outlets.state,hosts.ro_community,outlets.oid,
#        hosts.on_value,hosts.off_value,hosts.rw_community,hosts.status,
#        hosts.ip
#        from hosts,outlets where hosts.idx=outlets.hostidx;"""

    if verbose:
        print(sql)

    cur.execute(sql)
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
        ip = r[9]

        if verbose:
            print "================"
            print "PDU Name        : %s" % pduName
            print "PDU Address     : %s" % ip
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
        cmd = "snmpset -t 10 -v1 -c %s %s %s i %d > /dev/null 2>&1" % (rw,ip,oid,tmp)

        if verbose: print cmd

        res = 0
        if not test:
            res =  os.system( cmd )

        if tmp == rebootValue:
            res = 0
            ret = rebootValue
            requestedState = "ON"
        else:
            cmd = "snmpget -OvQ -t 10 -v1 -c %s %s %s 2> /dev/null" % ( ro,ip,oid)

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

    con.close()



main()






