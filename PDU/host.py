#!/usr/bin/env python 
#
# POWER_DB  Database
# PDIR      Base directory for utilities
#
import sys
import getopt
import sqlite3 as sqlite
import os.path
from os import getenv,popen,remove,system
from time import sleep

from string import strip

def printf(format, *args):
    sys.stdout.write(format % args)

def usage():
    print "Usage:\n"
    print "\thost type=<pdu type> <hostname>"
    print "\thost port=<port number> <hostname>"
    print "\thost rw=<R/W SNMP community name> <hostname>"
    print "\thost ro=<RO SNMP community name> <hostname>"
    print "\thost counter=<ping counter> <hostname>"
    print "\thost ip=<IP Address> <hostname>"
    print "\n"
    print "\thost add <hostname>"
    print "\thost delete <hostname>"
    print "\thost up <hostname>"
    print "\thost down <hostname>"
    print "\thost status <hostname>|all"
    print "\thost update <hostname>|all"

def main():
    verbose = False
    
    db = getenv("POWER_DB")

    pdir = getenv("PDIR")

    if db == None or pdir == None:
        print "FATAL ERROR: setup PDIR & POWER_DB env variables"
        sys.exit(1)

    if len(sys.argv) != 3:
        usage()
        sys.exit(0)
 
    con = sqlite.connect( db )
    cur = con.cursor()
               
    request = sys.argv[1]
    hostName = sys.argv[2]

    tmp = request.split("=")
    sql = ""

#    if hostName != 'all':
#        sql = "select count(*) from hosts where name='%s';" % hostName
#
#        cur.execute( sql )
#        count = (cur.fetchone())[0]
#
#        if count == 0:
#            sys.exit(1)
    
    if len(tmp) == 2:
        if tmp[0] == "type":
            if tmp[1] == "cyc":
                on = 1
                off = 0
                reboot = 2
            elif tmp[1] == "apc":
                on = 1
                off = 2
                reboot = 3
            else:
                on = 0
                off = 0
                reboot = 0
                
            sql = """update hosts 
            set type='%s',on_value=%d,off_value=%d,reboot_value=%d,touched=datetime(\'NOW\') 
            where name='%s';""" % ( tmp[1], on,off,reboot, hostName )
            
            if verbose:
                print sql
        elif tmp[0] == "port":
            sql = "update hosts set port=%s where name='%s',touched=datetime(\'NOW\');" % ( tmp[1], hostName )
            if verbose:
                print sql       
        elif tmp[0] == "rw":
            sql = "update hosts set rw_community='%s' where name='%s',touched=datetime(\'NOW\');" % ( tmp[1], hostName )
            if verbose:
                print sql 
        elif tmp[0] == "ro":
            sql = "update hosts set ro_community='%s' where name='%s',touched=datetime(\'NOW\');" % ( tmp[1], hostName )
            if verbose:
                print sql 
        elif tmp[0] == "counter":
            sql = "update hosts set ping_count=%s where name='%s',touched=datetime(\'NOW\');" % ( tmp[1], hostName )
            if verbose:
                print sql 
        elif tmp[0] == "ip":
            sql = "update hosts set ip='%s',touched=datetime(\'NOW\') where name='%s';" % ( tmp[1], hostName )
            if verbose:
                print sql
                
        cur.execute( sql )
        con.commit()
    else:    
        request = sys.argv[1].upper()
        
        if request == "ADD":
            sql = "insert into hosts (name) values ('%s');" % hostName
            cur.execute( sql )
            con.commit()
        elif request=="DELETE":
            sql = "delete from hosts where name='%s';" % hostName
            if verbose:
                print sql
            cur.execute( sql )
            con.commit()
        elif request == "UP":
            sql = "update hosts set status='%s' where name='%s';" % ( request, hostName )
            cur.execute( sql )
            con.commit()
        elif request == "DOWN":
            sql = "update hosts set status='%s' where name='%s';" % (request, hostName )
            cur.execute( sql )
            con.commit()
        elif request == "UPDATE":
            cmd = "fping %s > /dev/null" % hostName
            res = os.system( cmd )
            if res == 0:
                sql = "update hosts set status='UP' where name='%s';" %  hostName
            else:
                sql = "update hosts set status='DOWN' where name='%s';" %  hostName

            cur.execute( sql )
            con.commit()

        elif request == "STATUS":
            sql = """select name,ip,type,port,rw_community,ro_community,on_value,off_value,reboot_value,status,ping_count,mac
            from hosts"""
            
            if hostName == "all":
                sql = sql +';'
            else:
                sql = sql + " where name = '%s';" % hostName
                
            if verbose:
                print sql
                
            cur.execute( sql )
            
            print "+================+================+==================+==========+==========+"
            printf("|%-16s", " Name")
            printf("|%-16s", " IP" )
            printf("|%-18s", " MAC" )
            printf("|%-10s", " Type" )
            printf("|%-10s|", " Status" )
            print
            print "+================+================+==================+==========+==========+"
            for r in cur.fetchall():
                name   = r[0]
                ip     = r[1]
                type   = r[2]
                port   = r[3]
                rw     = r[4]
                ro     = r[5]
                on     = r[6]
                off    = r[7]
                reboot = r[8]
                status = r[9]
                counter= r[10]
                mac    = r[11]
                
                printf("|%-16s" , name)
                printf("|%-16s" , ip)
                printf( "|%-18s" , mac)
                printf( "|%-10s" , type)

#                printf("|%-5s" , port)
#                print "RW Community: %s" % rw
#                print "RO Community: %s" % ro
#                print "ON          : %s" % on
#                print "OFF         : %s" % off
#                print "REBOOT      : %s" % reboot
                printf( "|%-10s|\n" , status)
#                print "Ping Counter: %s" % counter
            print "+================+================+==================+==========+==========+"
                
                    
                
                
main()
