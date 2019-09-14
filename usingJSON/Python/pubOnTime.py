#!/usr/bin/env python3

import signal
import os
import os.path
import sys

import json
import sys 
import paho.mqtt.client as mqtt

import time
import daemon 

from datetime import datetime # datetime object containing current date and time

connectedFlag = False

def on_connect(client, userdata, flags, rc):
    global connectedFlag
    connectedFlag = True
#    print("Connected")
#    print("=========")

def on_message(client, userdata, msg):
    print("on_message")

def on_publish(client, userdata, mid):
    print("Message "+str(mid)+" published.")


secondsInDay = 24 * 60 * 60 

class timedOut(Exception):
    pass;

def handler(signum, frame):
    global timedOut
    
    print('Signal handler called with signal', signum)
    raise timedOut

def doCalc(triggerTime):

    triggerTimeList = triggerTime.split(":")
    triggerOffset = ((int( triggerTimeList[0]) * 60) + int( triggerTimeList[1])) * 60

    now = datetime.now()

    nowHH  = now.hour
    nowMM  = now.minute
    nowSec = now.second

    nowOffset = (((nowHH * 60) + nowMM) * 60) + nowSec # in seconds

    delay = triggerOffset - nowOffset 

#    print("Delay          =", delay) 

    if delay < 0:
        delay = secondsInDay + delay

#    print("now offset     =", nowOffset) 
#    print("trigger offset =", triggerOffset) 
#
#    print("Delay          =", delay) 
#    print("Midnight       =", secondsInDay) 

    return delay


def main():

#    print(sys.argv)

    if len(sys.argv) != 4:
        print("Usage: tst.py on|off hh:mm <mqtt topic>")
        sys.exit(1)

    configFile="/etc/mqtt/bridge.json"

    if os.path.exists(configFile):
        with open( configFile, 'r') as f:
            cfg = json.load(f)

        mqttBroker = cfg['local']['name']
        mqttPort   = int(cfg['local']['port'])
    else:
        print(configFile + " not found..")
        sys.exit(3)

    mqttClient = mqtt.Client()
    mqttClient.on_connect = on_connect

    mqttClient.connect(mqttBroker, mqttPort, 60) 

    global connectedFlag

    while not connectedFlag: #wait in loop
        mqttClient.loop()
#        print("In wait loop")

        time.sleep(0.1)

    while True:
        state = (sys.argv[1]).upper()
    
        triggerTime = sys.argv[2]
        topic = sys.argv[3]
    
        delay = doCalc( triggerTime )
    
        print("Delay=",delay)
    
        # Set the signal handler and a 5-second alarm
        signal.signal(signal.SIGALRM, handler)
        signal.alarm(delay)
        
        try:
            time.sleep(secondsInDay)
        except timedOut:
            pass
    
#        print( state + " to " + topic )
    
        mqttClient.publish(topic, state, retain=True)
        mqttClient.loop()

        time.sleep(1)
        
        signal.alarm(0)          # Disable the alarm

with daemon.DaemonContext():
    main()


