#!/usr/bin/env python3
# import sqlite3 as sqlite
import paho.mqtt.client as mqtt
import configparser as cp
import os
import sys
import getopt
import json


verbose=False
hosts=[]
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

    t=msg.topic
    m=(msg.payload).decode()

    devType="system"

    if verbose:
        print("Message")
        print("Topic : " + t )
        print("Msg   : " + m )
        print("=======")

    tmp=t.split('/')

    if tmp[1] not in result:
        result[tmp[1]] = {}
        print(result)

    if tmp[2] != "environment":
        if tmp[2] not in result[tmp[1]]:
            result[tmp[1]][tmp[2]] = {}

        if tmp[3] not in result[tmp[1]][tmp[2]]:
            result[tmp[1]][tmp[2]][tmp[3]] = {}
            print(result)

        if tmp[3] == "ups":
            result[tmp[1]][tmp[2]][tmp[3]]['type'] = 'UPS'
            result[tmp[1]][tmp[2]][tmp[3]][tmp[4]] = m


        if tmp[4] in ("STATE","SENSOR","LWT","POWER"):
            devType="sonata"
            result[tmp[1]][tmp[2]][tmp[3]]['type']= devType
            if tmp[4] == "POWER":
                result[tmp[1]][tmp[2]][tmp[3]]['power']= m
            elif tmp[4] == "SENSOR":
                sensorData = json.loads( m )
                result[tmp[1]][tmp[2]][tmp[3]]['sensor']= sensorData
            elif tmp[4] == "STATE":
                stateData = json.loads( m )
                result[tmp[1]][tmp[2]][tmp[3]]['state']= stateData

        elif tmp[4] in ("power","state"):
            result[tmp[1]][tmp[2]][tmp[3]] = {}
            result[tmp[1]][tmp[2]][tmp[3]][tmp[4]]= m
            result[tmp[1]][tmp[2]][tmp[3]]['type']= devType
    else:
        print("tmp[3]",tmp[3])
        result[tmp[1]][tmp[2]] = {}
        result[tmp[1]][tmp[2]][tmp[3]]=m


    print(json.dumps(result, sort_keys=True, indent=4))


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

