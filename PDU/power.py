#!/usr/bin/env python 
#
# POWER_DB  Database
# PDIR      Base directory for utilities
#
import sys
import getopt
import sqlite3 as sqlite
import os.path
from os import getenv,popen

from string import strip

def usage():
    print "Usage: power"

def main():
    cmd = ""
    
    verbose = False
    locked = "NO"

    db = getenv("POWER_DB")

    if db == None:
        db = "/etc/local/power/data/power.db"

    pdir = getenv("PDIR")
    if pdir == None:
        pdir = "/usr/local/apps/power"

    if db == None or pdir == None:
        print "FATAL ERROR: setup PDIR & POWER_DB env variables"
        sys.exit(1)


    if len(sys.argv) != 3:
        usage()
        sys.exit(0)
    con = sqlite.connect( db )
    cur = con.cursor()

    request = sys.argv[1]
    out = sys.argv[2]

    tmp = request.split("=")
    
    if len(tmp) == 2:
        if tmp[0] == "pf_action":
            sql = "update outlets set pf_action='%s',touched=datetime(\'NOW\') where name='%s';" % (tmp[1].upper(), out )
        elif tmp[0] == "start":
            sql = "update outlets set start_state='%s',touched=datetime(\'NOW\') where name='%s';" % (tmp[1].upper(), out )
            
        cur.execute( sql )
        con.commit()
    else:
        request = sys.argv[1].upper()
        
        if  request == "STATUS":
            if verbose: print "status ..."

            sql = """select hosts.name,outlets.name,outlets.state,outlets.requested_state,outlets.start_state,outlets.locked,outlets.pf_action 
            from hosts,outlets
            where hosts.idx = outlets.hostidx"""

            if out == "all":
                sql = sql + ";"
            else:
                sql = sql + " and outlets.name = '%s';" % sys.argv[2]

            if verbose: print sql

            cur.execute( sql )
        
            print "+" + 8*'-' + "+" + 16*'-' + "+" + 8*'-' + "+" + 8*'-' + "+" + 7*"-" + "+" + 6*'-' + "+" + 6*'-' + '+' 
            print "|%-8s|%-16s|%-8s|%-8s|%-7s|%-6s|%-6s|" % ("PDU", "Name","State","Pending","Start","Locked","PF Act")
            print "+" + 8*'-' + "+" + 16*'-' + "+" + 8*'-' + "+" + 8*'-' + "+" + 7*"-" + "+" + 6*'-' + "+" + 6*'-' + '+' 
            
            for r in cur.fetchall():
                print "|%-8s|%-16s|%-8s|%-8s|%-7s|%-6s|%-6s|" % ( r[0],r[1],r[2],r[3],r[4],r[5],r[6])

            print "+" + 8*'-' + "+" + 16*'-' + "+" + 8*'-' + "+" + 8*'-' + "+" + 7*"-" + "+" + 6*'-' + "+" + 6*'-' + '+' 


        elif request == "ON" or request == "OFF" or request == "REBOOT":
            sql = "update outlets set requested_state='%s',touched=datetime(\'NOW\') where locked='NO'" % request

            if out == "all":
                sql = sql + ';'
            else:
                sql = sql + " and name = '%s';" % out

            if verbose: print sql

        cur.execute(sql)
        con.commit()

    con.close()
    
main()
