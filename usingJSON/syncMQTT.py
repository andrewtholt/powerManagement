#!/usr/bin/env python3

import sys
import os.path
import pymysql as sql
import json
import paho.mqtt.client as mqtt
import time
import getopt

db = None
database = 'localhost'
connected = False

def usage():
    print("")
    print("Usage: timeSwitch.py -h|--help -c <cfg>|--config=<cfg>")
    print("\t-h|--help\t\tHelp.")
    print("\t-c <cfg>|--config=<cfg>\tConfig file")
    print("")

def stateToLogic( state ) :

    if state in ["ON","TRUE"]:
        return "TRUE"

    if state in ["OFF","FALSE"]:
        return "FALSE"


def on_message(client, userData,msg):
    print("On Message")
    global database

    print(database)
    db = sql.connect(database, "automation","automation","automation")

    topic = msg.topic
    print(topic)

    state = (msg.payload).decode("utf-8")
    print(state)

    sqlCmd = "select state from mqtt where topic = '" + topic + "';"
    print(sqlCmd)
    cursor = db.cursor()
    cursor.execute(sqlCmd)
    data = cursor.fetchone()
    cursor.close()
    db.close()

    dbState = data[0]
    devState = stateToLogic(state)

    print("Device ",devState)
    print("Db     ",dbState)

    if devState == dbState:
        print("OK")
    else:
        print("Publish")
        client.publish(topic, dbState, qos=0, retain=True)

def on_connect(client, userdata, flags, rc):
    global connected
    global database

    connected=True
    print("On Connect, In ...")
    print(database)

    db = sql.connect(database, "automation","automation","automation")
    cursor = db.cursor()
    cursor.execute("select name,direction,topic,state from mqttQuery where direction = 'OUT';")

    data = cursor.fetchall()
    for row in data:
        print("Name      :", row[0], )
        print("Direction :", row[1])
        print("Topic     :", row[2])
        print("State     :", row[3], "\n")

        client.subscribe( row[2] )

    cursor.close()
    db.close()
    print("... On Connect, Out")
    

def main():
    global database

    try:
        opts,args = getopt.getopt(sys.argv[1:], "c:h", ["config=","help"])
        for o,a in opts:
            if o in ["-h","--help"]:
                usage()
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


    print("Database is ", database)
#    db = sql.connect(database, "automation","automation","automation")

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
    mqttClient.loop_forever()
    db.close()


main()
