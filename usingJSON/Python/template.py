#!/usr/bin/env python3

import sys 
import os.path
import pymysql as sql 
import json
import paho.mqtt.client as mqtt
import time
import getopt

db = None

connected=False

def stateToLogic( state ) :

    if state in ["ON","TRUE"]:
        return "TRUE"

    if state in ["OFF","FALSE"]:
        return "FALSE"


def on_message(client, userData,msg):
    print("On Message")

def on_connect(client, userdata, flags, rc):
    global connected
    connected=True
    print("On Connect")

def usage(name):
    print("Usage: " + name + " ")

def main():
    global db

    try:
        opts,args = getopt.getopt(sys.argv[1:], "c:h", ["config=","help"])
        for o,a in opts:
            if o in ["-h","--help"]:
                usage(sys.argv[0])
                sys.exit()
            elif o in ["-c","--config"]:
                configFile = a
    except getopt.GetoptError as err:
        print(err)
        usage()
        sys.exit(2)

    database = 'localhost'
    mqttBroker = 'localhost'
    mqttPort = 1883

    configFile="/etc/mqtt/bridge.json"

    if os.path.exists(configFile):
        with open( configFile, 'r') as f:
            cfg = json.load(f)

        mqttBroker = cfg['local']['name']
        mqttPort   = int(cfg['local']['port'])

        database = cfg['database']['name']
    else:
        print(configFile + " not found..")
        print(".. using defaults")


    db = sql.connect(database, "automation","automation","automation")

    mqttClient = mqtt.Client()
    mqttClient.on_connect = on_connect
    mqttClient.on_message = on_message

    mqttClient.connect(mqttBroker, mqttPort, 60)

    global connected

    while not connected:
        print("Waiting ...")
        mqttClient.loop()
        time.sleep(0.1)

    # disconnect from server
    # mqttClient.loop_forever()
    db.close()

