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
    print("On Connect")

#    cursor = db.cursor()
#    sql = "update mqtt set state = '" + payload + "' where topic = '" + topic + "';"
#    print(sql)
#    cursor.execute( sql )
#    db.commit()
#
#    cursor.close()

    client.publish(topic, payload, qos=0, retain=True)

def usage():
    print("Help")

def main():
    global db
    global verbose
    global topic
    global payload

    try:
        opts,args = getopt.getopt(sys.argv[1:], "vc:hp:t:", ["verbose","config=","help","payload=","topic="])
        for o,a in opts:
            if o in ["-h","--help"]:
                usage()
                sys.exit()
            elif o in ["-c","--config"]:
                configFile = a
            elif o in ["-v","--verbose"]:
                verbose = True
            elif o in ["-t","--topic"]:
                topic = a
            elif o in ["-p","--payload"]:
                payload = a

    except getopt.GetoptError as err:
        print(err)
        usage()
        sys.exit(2)

    if verbose:
        print("Topic   :" + topic)
        print("Payload :" + payload)

    if topic == "" or payload == "":
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


    print(database)
    db = sql.connect(database, "automation","automation","automation")

    cursor = db.cursor()
    sqlCmd = "update mqtt set state = '" + payload + "' where topic = '" + topic + "';"
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
        print("Waiting ...")
        time.sleep(0.1)
        mqttClient.loop()

    # disconnect from server
    # mqttClient.loop_forever()
    mqttClient.loop()
    db.close()


main()
