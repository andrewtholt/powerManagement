#!/usr/bin/env python3

import os.path
import json
import getopt
import sys
import paho.mqtt.client as mqtt

import datetime
import time

from os import getenv

def usage():
    print("")
    print("Usage: timeSwitch.py -c|--config=<cfg> -v|--verbose --time=<ff:ff-tt:tt> -t|--topic=<mqtt topic> -i|--invert")
    print("\t-v\t\t\tVerbose.")
    print("\t-c|--config=<cfg>\tConfig file for MQTT broker settings.")
    print("\t--time ff:ff-tt:tt\tTime range, earlier first.")
    print("\t-i\t\t\tSet the message to OFF if the current time is outside the time range.")
    print("")
    print("Default Settings")
    print("\t--config=/etc/mqtt/bridge.ini")
    print("")

def main():
    verbose=False
    invert = False

    ready=False

    configFile="/etc/mqtt/bridge.ini"
    timeRange=[]

    topic=None
    timeRange=None

    try:
        opts, args = getopt.getopt(sys.argv[1:], "c:ht:vi", ["time=","config=","help","topic=","verbose","invert"])

        for o,a in opts:
            if o in ["-h","--help"]:
                usage()
                sys.exit()
            elif o in ["-c","--config"]:
                configFile = a 
            elif o in ["-t","--topic"]:
                topic = a
            elif o in ["-v","--verbose"]:
                verbose=True
            elif o in ["--time"]:
                timeRange=a.split("-")
            elif o in ["-i","--invert"]:
                invert = True
    except getopt.GetoptError as err:
        print(err)
        usage()
        sys.exit(2)

    ready = (topic != None) and (timeRange != None)

    if not ready:
        usage()
        sys.exit(1)

    if os.path.exists(configFile):
        with open( configFile, 'r') as f:
            cfg = json.load(f)

        mqttBroker = cfg['local']['name']
        mqttPort   = int(cfg['local']['port'])
    else:
        print(configFile + " not found..")
        sys.exit(3)

    if verbose:
        print("Verbose")
        print("Start time : " + str( timeRange[0]))
        print("End   time : " + str( timeRange[1]))
        print("Topic      : " + topic)
        print("Config file: " + configFile)
        print("MQTT Broker: " + mqttBroker)
        print("MQTT Port  : " + str(mqttPort))

        print()

    mqttClient = mqtt.Client()
    mqttClient.connect(mqttBroker, mqttPort, 60)

    pattern = "%Y-%m-%d %H:%M"

    currentTime = time.gmtime()

    dt=datetime.datetime(currentTime.tm_year,currentTime.tm_mon,currentTime.tm_mday,23,59)
    midnight=int(time.mktime(dt.timetuple()))


#    now = (time.strftime(pattern, time.gmtime())).split(" ")
    now = (time.strftime(pattern, time.gmtime())).split(" ")
    nowSeconds=int(time.time())+120

    fromTime = [None] * 2
    toTime = [None] * 2

    fromTime[0] = now[0]
    fromTime[1] = timeRange[0]

    toTime[0] = now[0]
    toTime[1] = timeRange[1]

#    print(fromTime)
#    print(now)
#    print(toTime)

    fromSeconds = int(time.mktime(time.strptime(" ".join(fromTime), pattern)))
    toSeconds   = int(time.mktime(time.strptime(" ".join(toTime), pattern)))
    tts = fromSeconds - nowSeconds
    tte = toSeconds - nowSeconds
    ttm = midnight - nowSeconds

#    fromSeconds = (" ".join(fromTime))

    if verbose:
        print("From    :",fromSeconds)
        print("Now     :",nowSeconds)
        print("To      :",toSeconds)
        print("Midnight:",midnight)

        print("Time to start   : ", tts)
        print("Time to end     : ", tte)
        print("Time to midnight: ", ttm)

        if( tts > 0 and tte > 0):
            print("\nStart and end both in the future")
            print("Sleep for", abs(tts) , "Seconds")
        elif ( tts < 0 and tte > 0):
            print("\nStart in past and end in the future")
            print("Sleep for", abs(tte) , "Seconds")
        elif( tts < 0 and tte < 0):
            print("\nStart and end both in the past")
            print("Sleep for", ttm)


    flag = (nowSeconds >=fromSeconds and nowSeconds<=toSeconds)

    msg="ERROR"
    if flag :
        if invert:
            print("False")
            msg="OFF"
        else:
            print("True")
            msg="ON"
    if not flag:
        if invert:
            print("ON")
            msg="ON"
        else:
            print("OFF")
            msg="OFF"

    mqttClient.publish(topic, msg, qos=0, retain=True)
    time.sleep(0.05)
    if verbose:
        print("Message    : " + msg)

#    if nowSeconds >=fromSeconds and nowSeconds<=toSeconds:
#        print("TRUE")
#    else:
#        print("FALSE")
#


main()
