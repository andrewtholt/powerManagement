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
    print("Usage: syncMQTT.py -h|--help -c <cfg>|--config=<cfg>")
    print("\t-h|--help\t\tHelp.")
    print("\t-c <cfg>|--config=<cfg>\tConfig file")
    print("")

def stateToLogic( state ) :

    if state in ["ON","TRUE"]:
        return "ON"

    if state in ["OFF","FALSE"]:
        return "OFF"


def on_message(client, userData,msg):
    print("On Message")
    global database

    print("Database " + database)
    db = sql.connect(database, "automation","automation","automation")

    topic = msg.topic

    state = (msg.payload).decode("utf-8")

    sqlCmd = "select state, direction, data_type from mqttQuery where topic = '" + topic + "';"
    print( sqlCmd )

    cursor = db.cursor()
    cursor.execute(sqlCmd)

    data = cursor.fetchone()

    cursor.close()
    db.close()

    dbState = data[0]
    dbDirection = data[1]
    dataType = data[2]
#
# Test for type and if bool
#
    print("Data Type : " + dataType)
    if dataType == 'BOOL':
        devState = stateToLogic(state)
    elif dataType == 'STRING':
        devState = state

    print("Device ",devState)
    print("Db     ",dbState)
    print("Dir   >"+dbDirection+"<")

    if dbDirection == "OUT":
        print("OUT")
        if devState == dbState:
            print("OK")
        else:
            print("Publish:" + dbState)
            # 
            # If I have receieved a message from something I believe I am controlling
            # Then set it it to what I say it should be.
            # 
            client.publish(topic, dbState, qos=0, retain=True)
    elif dbDirection == "IN":
        db = sql.connect(database, "automation","automation","automation")

#        sqlCmd = "update mqtt set state = '" + devState + "' where topic = '" + topic + "';"
        sqlCmd = "update mqtt,io_point set io_point.old_state = io_point.state,io_point.state = '"+devState+"' where mqtt.topic = '"+topic+"' and mqtt.name=io_point.name ;"

        print( sqlCmd )
        cursor = db.cursor()
        cursor.execute( sqlCmd );
        db.commit()
        cursor.close()
        db.close()

def on_connect(client, userdata, flags, rc):
    global connected
    global database

    connected=True
    print("On Connect, In ...")
    print(database)

    db = sql.connect(database, "automation","automation","automation")
    cursor = db.cursor()
#    cursor.execute("select name,direction,topic,state from mqttQuery where direction = 'OUT';")
    cursor.execute("select name,direction,topic,state, enabled from mqttQuery ;")

    data = cursor.fetchall()
    for row in data:
        if row[4] == "YES":
            print("Subscribing")
            client.subscribe( row[2] )
        else:
            print("Not Subscribing")

        print("Name      :", row[0], )
        print("Direction :", row[1])
        print("Topic     :", row[2])
        print("State     :", row[3])
        print("Enabled   :", row[4], "\n")


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
