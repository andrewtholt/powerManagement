#!/usr/bin/env python3 
#
# POWER_DB  Database
# PDIR      Base directory for utilities
#
import sys
import getopt
import sqlite3 as sqlite
import os.path
from os import getenv,popen,path
from time import sleep

import paho.mqtt.client as mqtt


# from string import strip

def usage():
    print("Usage: power <cmd> <device>")
    print("\tcmd is one of:")
    print("\t\tstatus\tStatus of named device.")
    print("\t\tqstatus\tStatus of named device.")
    print("\t\t\tReturns exit code of 0, if off, and 1 if on." )
    print("\t\ton\tSwitch named device on.")
    print("\t\toff\tSwitch named device off.\n")
    print("\tdevice is the name of a PDU port, or the special cases\n")
    print("\t\t'all'      - List the state of all the ports.")
    print("\t\t'on'       - List the ports that are powered on.")
    print("\t\t'off'      - List the ports that are powered off.")
    print("\t\t'pending'  - List the ports that have pending operations.\n")

    print("\tExamples:")
    print("\t\tpower status fred")
    print("\t\tpower off fred")
    print("\t\tpower status all")
    print("\t\tpower off fred in 5 minutes\n")

def executeSql(cur,sql):
    failed = True
    while failed:
        try:
            cur.execute( sql )
            failed=False
        except sqlite.OperationalError:
            print("Database Locked.")
            failed=True
            sleep(1)
    return

def onConnect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))

def onMessage(client, userdata, msg):
    print(msg.topic+":"+str(msg.payload))

def main():
    cmd = ""
    
    verbose = True
    locked = "NO"

    db = None
    pdir = None
    now = True # Update act_when with current time if this is true.

    db = getenv("POWER_DB")

    pdir = getenv("PDIR")

    if db == None or pdir == None:
        print("FATAL ERROR: setup PDIR & POWER_DB env variables")
        sys.exit(1)


    if len(sys.argv) < 3:
        usage()
        sys.exit(0)
    
    con = sqlite.connect( db )
    cur = con.cursor()

    client = mqtt.Client()
    client.on_connect = onConnect
    client.on_message = onMessage

    client.connect("127.0.0.1", 1883, 60)

    client.loop()

    
    request = sys.argv[1]
    out = sys.argv[2]

    if len(sys.argv) >= 5:
        now=False
        delay= sys.argv[4]

    if len(sys.argv) == 6:
        units= sys.argv[5]
    else:
        units = 'minutes'

    if now == False:
        print("Delay", delay)
        print("Units", units)


    tmp = request.split("=")
    
    if len(tmp) == 2:
        if tmp[0] == "pf_action":
            sql = "update outlets set pf_action='%s',touched=datetime(\'NOW\') where name='%s';" % (tmp[1].upper(), out )
        elif tmp[0] == "start":
            sql = "update outlets set start_state='%s',touched=datetime(\'NOW\') where name='%s';" % (tmp[1].upper(), out )
            
        executeSql(cur,sql)
#        cur.execute( sql )
        con.commit()
    else:
        request = sys.argv[1].upper()
        
        if request == "QSTATUS":
            sql = "select state from outlets where name ='%s';" % sys.argv[2]
            print(sql)
            executeSql(cur,sql)

            if  cur.fetchone()[0] == "ON":
                sys.exit(1)
            else:
                sys.exit(0)

        elif  request == "STATUS":
            if verbose: 
                print("status ...")

            sql = """select hosts.name,outlets.name,outlets.state,
            outlets.requested_state,outlets.start_state,outlets.locked,
            outlets.pf_action, outlets.pf_state
            from hosts,outlets
            where hosts.idx = outlets.hostidx"""

            if out == "all":
                sql = sql + ";"
            elif out == "on":
                print("List outlets switched on.")
                sql = sql + " and outlets.state ='ON';"
            elif out == "off":
                print("List outlets switched off.")
                sql = sql + " and outlets.state ='OFF';"
            elif out == "pending":
                print("List outlets pending")
                sql = sql + " and outlets.requested_state <> 'NA';"
            else:
                sql = sql + " and outlets.name = '%s';" % sys.argv[2]

            if verbose:
                print(sql)

            executeSql(cur,sql)
#            cur.execute( sql )
        
            print("+" + 8*'-' + "+" + 16*'-' + "+" + 8*'-' + "+" + 8*'-' + "+" + 8*'-' + "+" + 7*"-" + "+" + 6*'-' + "+" + 6*'-' + '+')
            print("|%-8s|%-16s|%-8s|%-8s|%-8s|%-7s|%-6s|%-6s|" % ("PDU", "Name","State","PF State","Pending","Start","Locked","PF Act"))
            print("+" + 8*'-' + "+" + 16*'-' + "+" + 8*'-' + "+" + 8*'-' + "+" + 8*'-' + "+" + 7*"-" + "+" + 6*'-' + "+" + 6*'-' + '+')
            
            for r in cur.fetchall():
                print("|%-8s|%-16s|%-8s|%-8s|%-8s|%-7s|%-6s|%-6s|" % ( r[0],r[1],r[2],r[7],r[3],r[4],r[5],r[6]))

            print("+" + 8*'-' + "+" + 16*'-' + "+" + 8*'-' + "+" + 8*'-' + "+" + 8*'-' + "+" + 7*"-" + "+" + 6*'-' + "+" + 6*'-' + '+')


        elif request == "ON" or request == "OFF" or request == "REBOOT":
            # 
            # Power has failed, so DON'T update pf_state
            #
            if path.exists( '/var/tmp/powerfail.txt'):
                sql = "update outlets set requested_state='%s',touched=datetime(\'NOW\') where locked='NO'" % request
            else:

                print("Here")

                topic="/home/office/%s/power" % ( out )
                payload=request

                print(topic)
                print(payload)
                client.publish(topic, payload,retain=1)
                sql = "update outlets set requested_state='%s',pf_state='%s',touched=datetime(\'NOW\') " % (request,request)

                if now:
                    sql = sql +",act_when=datetime(\'NOW\') "
                else:
                    sql = sql +",act_when=datetime(\'NOW\','+%s %s') " % (delay,units)


                sql = sql + "where locked='NO'" 

            if out == "all":
                sql = sql + ';'
            else:
                sql = sql + " and name = '%s';" % out

            if verbose: print(sql)

        executeSql(cur,sql)
#        cur.execute(sql)
        con.commit()

    con.close()
    
main()
