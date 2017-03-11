#!/usr/bin/env python3
#
# POWER_DB  Database
# PDIR      Base directory for utilities
#
# Read each defined port and set the actual state in the database.
#
import sys
import getopt
import sqlite3 as sqlite
import redis

# from pysqlite2 import dbapi2 as sqlite
from os import getenv

import os.path
from os import getenv,popen,remove
from time import sleep

import paho.mqtt.client as mqtt

def on_connect(client, userdata,flags,rc):
    print("Connected with result code "+str(rc))

def on_message(client, userdata, msg):
    print(msg.topic+" "+str(msg.payload))

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

def main():

    verbose=False
    if len(sys.argv) > 1:
        verbose=True

    db = getenv("POWER_DB")

    pdir = getenv("PDIR")

    home = getenv("HOME");

    certFile = home + "/.certs/dioty_ca.crt"

    if db == None or pdir == None:
        print ("FATAL ERROR: setup PDIR & POWER_DB env variables")
        sys.exit(1)

    client = mqtt.Client()

    client.tls_set(certFile)

    client.on_connect = on_connect
    client.on_message = on_message
    client.connect("127.0.0.1", 1883, 60)
    print(client)
    client.loop()

    con = sqlite.connect( db )
    cur = con.cursor()

    red = redis.StrictRedis(host='localhost', port=6379, db=0)
    
    sql = """select 
        hosts.name,outlets.name,outlets.state,hosts.ro_community,outlets.oid,
        hosts.on_value,hosts.off_value,hosts.rw_community,hosts.status,
        hosts.ip
        from hosts,outlets where hosts.idx=outlets.hostidx and hosts.status='UP';"""

    if verbose:
        print (sql)

    executeSql(cur,sql)
    for r in cur.fetchall():
#        print (r[0],r[1],r[2],r[3])
        
        pduName=r[0]
        outletName =r[1]
        outletState = r[2]
        ro=r[3]
        oid=r[4]
        
        onValue=r[5]
        offValue = r[6]
        rw=r[7]
        hostStatus = r[8]
        ip=r[9]

        if verbose:
            print(ip+" Status is "+ hostStatus)
        
        if hostStatus != "DOWN":
            cmd = "snmpget -OvQ -t 10 -v1 -c %s %s %s 2> /dev/null" % ( ro, ip,oid)
            if verbose:
                print (cmd)
        
            t = popen( cmd )
            ret =  int( t.readline() )
        
            cState="UNKNOWN"
            if ret == int(onValue):
                cState="ON"
            elif ret == int(offValue):
                cState="OFF"

#            print ("Ret value ",ret)
#            print ("On Value  ",int(onValue))
#            print ("Off Value ",int(offValue))

            if verbose:
                print ("Host %s Host status %s" % ( outletName,cState ))

            topic="/home/office/%s/power" % ( outletName )

            payload=cState

            print(topic)
            print(payload)

            red.set(topic, payload)

            client.publish(topic, payload, qos=0, retain=True)

    con.close()



main()

