#!/usr/bin/env python3
import sqlite3 as sqlite
import paho.mqtt.client as mqtt
import configparser as cp
import os


sqlCmd="select hosts.ip,outlets.name, outlets.oid, hosts.on_value,off_value from outlets, hosts  where outlets.hostidx=hosts.idx and outlets.name='%s';"

snmpCmd="snmpset -OQven  -v1 -c private %s %s i %d"

verbose=True
hosts=[]
database="/opt/power/data/power.db"

sql=None

def on_message(client, userdata, msg):
    global verbose
    global sql

    t=msg.topic
    m=(msg.payload).decode()

    if verbose:
        print("Message")
        print("Topic : " + t )
        print("Msg   : " + m )
        print("=======")

    tmp=t.split('/')

    name=tmp[3]

#    sql = sqlite.connect( database )
    c=sql.cursor()

    cmd = sqlCmd % name

    c.execute( cmd )
    res=c.fetchone()

    if res == None:
        return

    if len(res) > 0:
#        print(cmd)
#        print( res)
        ip=res[0]
        name=res[1]
        oid=res[2]
        onValue=res[3]
        offValue=res[4]

#        [('192.168.10.51', 'Fans', '.1.3.6.1.4.1.318.1.1.4.4.2.1.3.8', 1, 2)]
        if m == "ON":
            snmp = snmpCmd % (ip, oid, onValue)
        else:
            snmp = snmpCmd % (ip, oid, offValue)

        if verbose:
            print(snmp)

        os.system(snmp)

def on_connect(client, userdata, flags, rc):
    global verbose
    global sql

    if verbose:
        print("Connected")
        # 
        # TODO: SHould this list be part of the config.
        # 
    client.subscribe("/home/office/+/power")
    client.subscribe("/home/outside/+/power")
    client.subscribe("/home/outside/+/cmnd/power")
    client.subscribe("/home/environment/#")

#    cmd = "select name, ip, status from hosts; "
#
#    c=sql.cursor()
#    c.execute( cmd )
#
#    for r in c.fetchall():
#        print( r )
    return

def main():
    global verbose
    global sql

    sql = sqlite.connect( database )

    configFile="/etc/mqtt/bridge.ini"

    if os.path.exists(configFile):
        cfg = cp.ConfigParser()
        cfg.read( configFile )
        mqttBroker=cfg.get('local','name')
        mqttPort=int(cfg.get('local','port'))
    else:
        print('No such file as ' + configFile, file=sys.stderr)
        sys.exit(2)

    if verbose:
        print("Broker : " + mqttBroker)
        print("Port   : " + str(mqttPort))

    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message

    client.connect(mqttBroker, mqttPort, 60) 

    client.loop_forever()


main()

