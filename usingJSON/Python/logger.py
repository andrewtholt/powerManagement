#!/usr/bin/env python3

import sys 
import os.path
import pymysql as sql 
import json
import paho.mqtt.client as mqtt
# import time
from time import gmtime, strftime, time
import getopt

db = None

connected=False
start_topic = "/test/#"
data = {}
verbose = True

logFd = None

def stateToLogic( state ) :

    if state in ["ON","TRUE"]:
        return "TRUE"

    if state in ["OFF","FALSE"]:
        return "FALSE"

def usage():
    print("Usage: logger.py -h|-l <file>| -c <cfg file>")

def on_message(client, userData,msg):
    global data
    global logFd
    
    shortName = ""

    theTime = strftime("%Y-%m-%d %H:%M:%S", gmtime())
    aTime = str(int(time()))

    result = (msg.payload).decode("utf-8")
    # 
    # Update storage here
    # 
    if logFd == None:
#        print(theTime,shortName + " : ",msg.topic, result);
        print(aTime,shortName + " : ",msg.topic, result);
    else:
#        logFd.write(theTime + ":")
        logFd.write(aTime + ":")
        logFd.write(msg.topic + " ")
        logFd.write(result )
        
        logFd.write("\r\n")
        logFd.flush()
    return

def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    global connected
    connected=True
    #
    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    #
    client.subscribe(start_topic)

def main():
#    global db
    global logFd

    configFile="/etc/mqtt/bridge.json"
    logFile = None

    try:
        opts,args = getopt.getopt(sys.argv[1:], "c:hl:", ["config=","help","logfile="])
        for o,a in opts:
            if o in ["-h","--help"]:
                usage()
                sys.exit()
            elif o in ["-c","--config"]:
                configFile = a
            elif o in ["-l","--logfile"]:
                logFile = a
                print(logFile)
                
    except getopt.GetoptError as err:
        print(err)
        usage()
        sys.exit(2)

    mqttBroker = 'localhost'
    mqttPort = 1883

    if os.path.exists(configFile):
        with open( configFile, 'r') as f:
            cfg = json.load(f)

        mqttBroker = cfg['local']['name']
        mqttPort   = int(cfg['local']['port'])
    else:
        print(configFile + " not found..")
        print(".. using defaults")

    if verbose:
        print("MQTT Broker : " + mqttBroker)
        print("       Port : " , mqttPort)
        print("      Topic : " , start_topic)
        print("      Log   : " , logFile)

    if logFile != None:
        logFd = open(logFile,"w+")
        
    mqttClient = mqtt.Client()
    mqttClient.on_connect = on_connect
    mqttClient.on_message = on_message

    mqttClient.connect(mqttBroker, mqttPort, 60)

    global connected
    mqttClient.loop_forever()

main()
