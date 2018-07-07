#!/usr/bin/env python3

import sys
import getopt
import os.path
from os import getenv
import paho.mqtt.client as mqtt
import configparser as cp
import datetime

sys.path.append(".")

import logicVM as logic


verbose=False
machine=None

subCount=0
oldState="UNKNOWN"

def usage():
    print("Usage: logger.py ...")


def on_message(client, userdata, msg):
    global subCount
    global oldState

#    print("On Message")
    state = (msg.payload).decode("utf-8")


    if state != oldState:
        now = datetime.datetime.now()
        print( now, end="" )
        print(">>> " + msg.topic+" "+ state )
        oldState = state


def on_connect(client, userdata, flags, rc):
    print("On connect")

def main():
    global verbose
    global db
    global subCount
    global machine

    configFile="/etc/mqtt/bridge.ini"

    print( sys.argv[1] )

    if os.path.exists(configFile):
        cfg = cp.ConfigParser()
        cfg.read( configFile )
        mqttBroker=cfg.get('local','name')
        mqttPort=int(cfg.get('local','port'))
    else:
        print('No such file as ' + configFile, file=sys.stderr)
        sys.exit(2)

    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message

    client.connect(mqttBroker, mqttPort, 60)

    client.subscribe( sys.argv[1] )

    client.loop_forever()


main()
