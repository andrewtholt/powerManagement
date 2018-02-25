#!/usr/bin/env python3
# import sqlite3 as sqlite
import paho.mqtt.client as mqtt
import configparser as cp
import os
import sys
import getopt
import json

from myDevice import sonata,system


verbose=False
hosts={}
database="/opt/power/data/npower.db"

sql=None

result={}

def usage():
    print()
    print("Usage:mqttToSnmp.py -v -h|-c <cfg> ")


def on_message(client, userdata, msg):
    global verbose
    global sql
    global result
    global hosts

    update=True

    t=msg.topic
    m=(msg.payload).decode()

    devType="SYSTEM"

    if verbose:
        print("Message")
        print("Topic : " + t )
        print("Msg   : " + m )
        print("=======")

    tmp=t.split('/')
    base=tmp[1]
    location=tmp[2]
    device=tmp[3]

    if len(tmp) > 4:
        parameter=tmp[4]
    else:
        parameter=None

    if base not in result:
        result[base] = {}
#        print(result)

    if location != "environment":
        if location not in result[base]:
            result[base][location] = {}

        if device not in result[base][location]:
            result[base][location][device] = {}

#            print(result)

        if device == "ups":
            devType = "UPS"
            result[base][location][device]['type'] = 'UPS'
            result[base][location][device][parameter] = m

        if parameter in ("STATE","SENSOR","LWT","POWER"):
            devType="SONATA"

            if device not in hosts:
                fred = sonata()
                fred.setBase(base)
                fred.setLocation(location)
                hosts[device] = fred

            result[base][location][device]['type']= devType
            if parameter == "POWER":
                result[base][location][device]['power']= m
                hosts[device].setPower(m)

            elif parameter == "SENSOR":
                sensorData = json.loads( m )
                result[base][location][device]['sensor']= sensorData
                hosts[device].setSensor(sensorData)

            elif parameter == "STATE":
                print(m)
                stateData = json.loads( m )
                result[base][location][device]['state']= stateData
                hosts[device].setState(m)

        elif parameter in ("power","state"):
            print("param " + parameter)
            if device not in hosts:
                hosts[device] = system()
                hosts[device].setBase( base )
                hosts[device].setLocation( location )

            result[base][location][device] = {}
            result[base][location][device][parameter]= m

            if parameter == "power":
                hosts[device].setPower(m)
            elif parameter == 'state':
                hosts[device].setState(m)

            result[base][location][device]['type']= devType
    else:
        result[base][location] = {}
        result[base][location][device]=m


    if update:
        print(json.dumps(result, sort_keys=True, indent=4))

        for key, val in hosts.items():
            print()
            print(key)
            val.dumpState()



def on_connect(client, userdata, flags, rc):
    global verbose
    global sql

    if verbose:
        print("Connected")
        # 
        # TODO: Should this list be part of the config.
        # 
#    client.subscribe("/home/office/+/power")
#    client.subscribe("/home/outside/+/power")
#    client.subscribe("/home/outside/+/cmnd/power")
#    client.subscribe("/home/environment/#")

    client.subscribe("/home/#")

#    client.subscribe("/home/office/raspberrypi/#")
#    client.subscribe("/home/outside/BackFloodlight/#")

    return

def main():
    global verbose
    global sql

    verbose = False
    configFile="/etc/mqtt/bridge.ini"

    try:
        opts, args = getopt.getopt(sys.argv[1:], "c:hv", ["config=","help","verbose"])
        for o,a in opts:
            if o in ["-h","--help"]:
                usage()
                sys.exit(0)
            elif o in ["-c","--config"]:
                configFile = a
            elif o in ["-v","--verbose"]:
                verbose = True

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

    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message

    client.connect(mqttBroker, mqttPort, 60) 

    client.loop_forever()


main()

