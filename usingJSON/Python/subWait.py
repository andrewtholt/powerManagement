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
    topic = msg.topic
    data = (msg.payload).decode("utf-8")

    if data == userData:
        sys.exit(0)

def on_connect(client, userdata, flags, rc):
    global connected
    connected=True

def usage(name):
    print("Usage: " + name + " -c|--config <cfg> -h|--help  -t|--topic <topic> -m|--msg <msg?")

    print("Subscribe to the topic and wait until a message is received with the content specified.")
    print("")
    print("\t-h\t\tHelp.")
    print("\t-c <cfg>\tUse config file.  Default is /etc/mqtt/bridge.json")
    print("\t-t <topic>\tTopic of interest, no wild cards.")
    print("\t-m <msg>\tWhen message content matches this exit with status 0.")

def main():
    global db

    topic = ""
    msg = ""

    configFile="/etc/mqtt/bridge.json"

    try:
        opts,args = getopt.getopt(sys.argv[1:], "c:ht:m:", ["config=","help","topic=","msg"])
        for o,a in opts:
            if o in ["-h","--help"]:
                usage(sys.argv[0])
                sys.exit()
            elif o in ["-c","--config"]:
                configFile = a
            elif o in ["-t","--topic"]:
                topic = a
            elif o in ["-m","--msg"]:
                msg = a

    except getopt.GetoptError as err:
        print(err)
        usage()
        sys.exit(2)

    if topic == "" or msg == "":
        print("Need a topic and a msg")
        usage(sys.argv[0])
        sys.exit(1)

    database = 'localhost'
    mqttBroker = 'localhost'
    mqttPort = 1883


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

    mqttClient = mqtt.Client(userdata=msg)
    mqttClient.on_connect = on_connect
    mqttClient.on_message = on_message

    mqttClient.connect(mqttBroker, mqttPort, 60)

    global connected
    while not connected:
#        print("Waiting ...")
        mqttClient.loop()
        time.sleep(0.1)

    mqttClient.subscribe(topic)
    # disconnect from server

    mqttClient.loop_forever()
    db.close()

main()
