#!/usr/bin/env python3

import sys
import getopt
import os.path
from os import getenv
import paho.mqtt.client as mqtt
import pymysql as sql

sys.path.append(".")

verbose=False
machine=None

subCount=0
oldState="UNKNOWN"

mqttBroker = 'localhost'
mqttPort = 1883

def usage():
    print("Usage: logger.py ...")


def on_message(client, userdata, msg):
    global subCount
    global oldState

    print("On Message")
    state = (msg.payload).decode("utf-8")


    if state != oldState:
        m = msg.topic+" "+ state

        print( ">>>" + m )
        oldState = state

def on_connect(client, userdata, flags, rc):
    global mqqtBroker
    print("On connect")
    print( mqttBroker )
    print("========")


def main():
    global verbose
    global db
    global subCount
    global machine

    global mqttBroker
    global mqqtPort

    configFile="/etc/mqtt/bridge.ini"

    mqttBroker = '192.168.0.65'
    mqttPort = 1883

    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message

    client.connect(mqttBroker, mqttPort, 60)

    client.subscribe( sys.argv[1] )

    client.loop_forever()


main()
