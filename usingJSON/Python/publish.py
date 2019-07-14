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
verbose=False
topic = ""
payload = ""

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

    client.publish(topic, payload, qos=0, retain=True)

def usage(name):
    print("Usage:" + name + " -v|--verbose -c <cfg>|--config <cfg> -t <topic>|--topic <topic> -m <msg>|--msg <msg>")
    print("\t-v\t\tVerbose.")
    print("\t-c <cfg>\tLoad config file.  Default is /etrc/mqtt/bridge.json")
    print("\t-t <topic>\tPublish to topic.")
    print("\t-m <msg>\tMsg to publish.")

def main():
    global db
    global verbose
    global topic
    global payload

    configFile="/etc/mqtt/bridge.json"
    try:
        opts,args = getopt.getopt(sys.argv[1:], "vc:hm:t:", ["verbose","config=","help","msg=","topic="])
        for o,a in opts:
            if o in ["-h","--help"]:
                usage(sys.argv[0])
                sys.exit()
            elif o in ["-c","--config"]:
                configFile = a
            elif o in ["-v","--verbose"]:
                verbose = True
            elif o in ["-t","--topic"]:
                topic = a
            elif o in ["-m","--msg"]:
                payload = a

    except getopt.GetoptError as err:
        print(err)
        usage(sys.argv[0])
        sys.exit(2)

    if verbose:
        print("Topic   :" + topic)
        print("Payload :" + payload)

    if topic == "" or payload == "":
        usage(sys.argv[0])
        sys.exit(2)

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


    if verbose:
        print("Database: " + database)

    db = sql.connect(database, "automation","automation","automation")

    cursor = db.cursor()
    sqlCmd = "update mqtt set state = '" + payload + "' where topic = '" + topic + "';"

    if verbose:
        print(sqlCmd)

    cursor.execute( sqlCmd )
    db.commit()
    cursor.close()

    mqttClient = mqtt.Client()
    mqttClient.on_connect = on_connect
    mqttClient.on_message = on_message

    mqttClient.connect(mqttBroker, mqttPort, 60)

    global connected

    while not connected:
#        print("Waiting ...")
        time.sleep(0.1)
        mqttClient.loop()

    # disconnect from server
    # mqttClient.loop_forever()
    mqttClient.loop()
    db.close()


main()
