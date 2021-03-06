#!/usr/bin/env python3

import paho.mqtt.client as mqtt
import sqlite3 as sqlite
from os import getenv,system
import sys

# pdu is the ip address 
# oid is the oid
# state is the value to be written.

# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))

    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe("/home/office/+/power")

# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    print(msg.topic+" "+(msg.payload).decode() )

    db=getenv("POWER_DB")

    if db == None:
        print("Environment not setup.")
        sys.exit(1)

    path=msg.topic.split("/")
    act=(str(msg.payload)).split("'")[1]

    outlet=path[3]

    sql="select hosts.name,hosts.on_value,hosts.off_value,outlets.name,outlets.oid, hosts.type from hosts,outlets where outlets.name='%s' and hosts.idx=outlets.hostidx;" % ( outlet)

    con=sqlite.connect(db)
    cur=con.cursor()

    cur.execute(sql)

    data=cur.fetchone()

    print(data)

    if data != None:
        pdu=data[0]
    
        onValue=data[1]
        offValue=data[2]
        name=data[3]
        oid=data[4]
        deviceType = data[5]
    
        print("onValue:",onValue)
        print("offValue:",offValue)
    
    #    print("Device Type:" + deviceType)
    
        localType='unknown'
    
        if deviceType in ['apc','cyc','snmp']:
            localType='snmp'
        elif deviceType in ['mqtt']:
            localType='mqtt'
    
        rw='private'
    
        state=-1
        if act=='ON':
            state=onValue
        elif act=='OFF':
            state=offValue
    
        if localType == 'snmp':
            cmd = "snmpset -t 10 -v1 -c %s %s %s i %d > /dev/null 2>&1" % (rw,pdu,oid,state)
            print(cmd)
            system(cmd)

    con.close


def main():
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message

    client.connect("127.0.0.1", 1883, 60)
#
# Blocking call that processes network traffic, dispatches callbacks and
# handles reconnecting.
# Other loop*() functions are available that give a threaded interface and a
# manual interface.
    client.loop_forever()


main()

