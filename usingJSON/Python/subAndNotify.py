#!/usr/bin/env python3

import sys 
import os.path
import json
import paho.mqtt.client as mqtt
import time
import getopt
import notify2

connected=False

state = {}

def stateToLogic( state ) :

    if state in ["ON","TRUE"]:
        return "TRUE"

    if state in ["OFF","FALSE"]:
        return "FALSE"


def on_message(client, userData,msg):
    global state

    topic = msg.topic
    data = (msg.payload).decode("utf-8")

    print(topic,data)

    print(state)

    if topic in state:
        print("Found")
        if data == state[topic]:
            print("No change")
        else:
            print("change")
            notify2.init(topic)
            n = notify2.Notification(topic, data)
            n.show()
    else:
        print("Not Found")

    state[topic] = data


#    if data == userData:
#        sys.exit(0)

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

def main():
    topic = ""
    msg = ""

    configFile="/etc/mqtt/bridge.json"

    try:
        opts,args = getopt.getopt(sys.argv[1:], "c:ht:", ["config=","help","topic="])
        for o,a in opts:
            if o in ["-h","--help"]:
                usage(sys.argv[0])
                sys.exit()
            elif o in ["-c","--config"]:
                configFile = a
            elif o in ["-t","--topic"]:
                topic = a

    except getopt.GetoptError as err:
        print(err)
        usage()
        sys.exit(2)

    if topic == "" :
        print("Need a topic.")
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

main()
