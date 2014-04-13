#!/usr/bin/env python 
#
# POWER_DB  Database
# PDIR      Base directory for utilities
#
import sys
import getopt
import sqlite3 as sqlite
import os.path
from os import getenv

from string import strip
from time import sleep

def usage():
    print "Usage: start.py "

def restore(db,pdir,verbose):

    if verbose:
        print "pdir    : %s" % pdir
        print "Database: %s" % db
        
    try:
        con = sqlite.connect( db )
        cur = con.cursor()
    except:
        print "\nProblem with database, re-run with -v"
        sys.exit(1)
        
        
    sql = "select name,state,initial_state,requested_state,start_state from outlets;"
    cur.execute( sql )

    for r in cur.fetchall():
        name       = r[0]
        curState   = r[1]
        initState  = r[2]
        reqState   = r[3]
        startState = r[4]

        if startState == "RESTORE":
            if curState == "ON":
                reqState = "ON"
            elif curState == "OFF":
                reqState = "OFF"
            else:
                reqState = initState

        elif startState == "ON":
            reqState = "ON"
        else:
            reqState = "OFF"
            

        if verbose:
            print "name       = ",name
            print "curState   = ",curState
            print "initState  = ",initState
            print "reqState   = ",reqState
            print "startState = ",startState
            print "======="
            
        sql = "update outlets set requested_state = '%s',touched=datetime(\'NOW\') where name = '%s';" % ( reqState, name)
        if verbose: print sql
        cur.execute(sql)

    con.commit()
    con.close()

def main():
    cmd = ""
    db = "/tmp/powerDB"
    pdir = "."
    verbose = False
    clean = False
    initFile = None
    upsFile = None
    suspend = 0;

    db = getenv("POWER_DB")
    if db == None:
        dbDir = "/etc/local/power/data"
        db = dbDir + "/power.db"
    else:
        dbDir = os.path.dirname(db)

    pdir = getenv("PDIR")
    if pdir == None:
        pdir = "/usr/local/apps/power"


    initFile = dbDir + "/pdu.txt"
    upsFile = dbDir + "/ups.txt"

    if db == None or pdir == None:
        print "FATAL ERROR: setup PDIR & POWER_DB env variables"
        sys.exit(1)

    try:
        opts,args = getopt.getopt(sys.argv[1:], "s:cd:hp:vi:",["suspend","clean","database","help","port","verbose","init"])
    except:
        print "Argument Error"
        usage()
        sys.exit(2)

    for o,a in opts:
        if o in ("-d","--database"):
            if verbose:
                print "Database         : %s" % a
            db = a
        elif o in ("-h","--help"):
            usage()
            sys.exit(0)
        elif o in ("-i","--init"):
            initFile = a
        elif o in ("-v","--verbose"):
            verbose = True
        elif o in ("-c","--clean"):
            clean=True
        elif o in ("-s","--suspend"):
            suspend = int(a)

    if suspend > 0:
        suspend = ((suspend + 60) / 60) * 60
    else:
        suspend = 0
        
    if verbose:
        print dbDir
        print db
        print pdir
        print initFile
        print upsFile
        print suspend

    sleep( suspend )

    if clean:
        if verbose:
            print "Clean:remove the existing database"
        try:
            os.unlink(db)
        except:
            pass

        if os.path.exists( pdir + "/setup.sql"):
            if verbose:
                print "SQL setup exists"
            cmd = "sqlite3 %s < %s" % ( db,(pdir + "/setup.sql"))

            if os.system( cmd ) != 0:
                print "FATAL ERROR:%s failed" % cmd
                sys.exit(1)
            else:
                if verbose:
                    print "%s success" % cmd
        else:
            print "FATAL Error: sql setup %s does not exist." % ( pdir + "/setup.sql")
            sys.exit(1)

        con = sqlite.connect( db )
        cur = con.cursor()

        if upsFile != None:
            if verbose:
                print "ups file specified %s ..." % upsFile

            tmp = open(upsFile).readlines()
            data=map(strip,tmp)

            for ent in data:
                tmp = ent.split(":")

                name = tmp[0]
                threshold=int(tmp[1])

                sql = "insert into ups (name,threshold) values ('%s',%d);" % ( name,threshold)
                cur.execute(sql)
                con.commit()

                if verbose: print sql

        if initFile != None:
            if verbose:
                print "Init file specified %s ..." % initFile
    
            if os.path.exists( initFile ):
                if verbose:
                    print "... exists"

                tmp = open(initFile).readlines()
                data=map(strip,tmp)

                for n in data:
                    tmp = n.split(":")

                    name = tmp[0]
                    ip   = tmp[1]
                    port = tmp[2]
                    type = tmp[3]
                    ro   = tmp[4]
                    rw   = tmp[5]
                    onValue = tmp[6]
                    offValue = tmp[7]
                    rebootValue = tmp[8]

                    sql = "insert into hosts (name,ip,type,port,rw_community,ro_community,on_value,off_value,reboot_value) values ('%s','%s','%s','%s','%s','%s',%s,%s,%s);" % (name,ip,port,type,ro,rw,onValue,offValue,rebootValue)
                    if verbose: print sql

                    cur.execute( sql )
                    con.commit()

                if os.path.exists(dbDir + "/outlets.txt"):
                    if verbose: print dbDir + "/outlets.txt" + " exists"

                    tmp = open(dbDir + "/outlets.txt").readlines()
                    data = map(strip,tmp)
                    for n in data:
                        tmp = n.split(":")

                        apcName    = tmp[0]
                        portName   = tmp[1]
                        portNumber = tmp[2]
                        delay      = tmp[3]
                        oid        = tmp[4]
                        initState  = tmp[5]
                        pfAction   = tmp[6]

                        sql = "select idx from hosts where name = '%s'" % apcName

                        if verbose: print sql

                        con = sqlite.connect( db )
                        cur = con.cursor()
                        cur.execute( sql )
                        hostidx =  cur.fetchone()[0]

                        sql = """insert into outlets
                            (hostidx,name,number,delay,oid,initial_state,pf_action)
                            values
                            (%s,'%s',%s,%s,'%s','%s','%s')""" % (hostidx,portName,portNumber,delay,oid,initState,pfAction);

                        if verbose: print sql

                        cur.execute( sql )
                        con.commit()

                else:
                    print "FATAL ERROR: %s does not exist" % (dbDir + "/outlets.txt")

            else:
                if verbose:
                    print "... does not exist"
    else:
        if verbose:
            print "Not Clean"
            
    restore(db,pdir,verbose)
    
main()
