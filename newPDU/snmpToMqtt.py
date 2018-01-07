#!/usr/bin/env python3
import sqlite3 as sqlite
import paho.mqtt.client as mqtt
import configparser as cp
import os
import sys 
import getopt

verbose=False
database="/opt/power/data/power.db"
sql=None


sqlCmd="select hosts.ip,outlets.name, outlets.oid, hosts.on_value,off_value from outlets, hosts  where outlets.hostidx=hosts.idx;"

snmpTemplate="snmpget -OQven  -v1 -c private %s %s"

def on_message(client, userdata, msg):
    print("on_message")

def on_connect(client, userdata, flags, rc):
    pass
#    print("on_connect")

def on_publish(client, userdata, mid):
    print("Message "+str(mid)+" published.")

def usage():
    print("Help")

def main():
    global verbose

    topicTemplate="/home/office/%s/power"
    configFile="/etc/mqtt/bridge.ini"

    try:
        opts, args = getopt.getopt(sys.argv[1:], "c:hv",["config=","help","verbose"])
        for o,a in opts:
            if o in ["-h","--help"]:
                usage()
                sys.exit()
            elif o in ["-c","--config"]:
                configFile = a 
            elif o in ["-v","--verbose"]:
                verbose=True

    except getopt.GetoptError as err:
        print(err)
        usage()
        sys.exit(2)

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

    sql = sqlite.connect( database )
    c=sql.cursor()

    c.execute( sqlCmd )

    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message

    client.connect(mqttBroker, mqttPort, 60) 

    res=c.fetchall()

    for n in res:
        ip=n[0]
        name=n[1]
        oid=n[2]
        onValue=n[3]
        offValue=n[4]

        snmpCmd=snmpTemplate % (ip, oid)

        if verbose:
            print("IP   :",ip)
            print("Name :",name)
            print("OID  :",oid)
            print("ON   :",onValue)
            print("OFF  :",offValue)
            print(snmpCmd)
            print("=======")

        f=os.popen( snmpCmd )

        state=int(f.read() )

        topic= topicTemplate % name 

        if state == onValue:
            msg = "ON"
        elif state == offValue:
            msg = "OFF"

        if verbose:
            print( topic)
            print(msg)

        client.publish(topic, msg, retain=True )
    client.loop()


main()




