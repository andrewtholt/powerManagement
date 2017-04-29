#!/usr/bin/env python3

import configparser as cp
import getopt
import sys
import redis
import paho.mqtt.client as mqtt
import time

from os import getenv

def onConnect(client, userdata, flags, rc):
    print("Connected")

def usage():
    print()
    print("Usage:dPower.py -h|-c <cfg> -t <device> -o <status|on|off>")

    print("\t-c <cfg>\tConfig file.")
    print("\t-h\t\tHelp.")
    print("\t-t <device>\tDevice to access, wildcards allowed.")
    print("\t-v\t\tVerbose")
    print("\t-o <operation>\tOperation to perform (on|off|status).")
    print("\t-z <zone>\tSpecify zone in whiche operation will act, e.g. 'office'")
    print("Note: Default is:")
    print('\tdPower.py -t "*" -o status')
    print()

def main():
    cmd=""
    verbose=False

    configFile="/etc/mqtt/bridge.ini"

    target="*"
    operation="STATUS"

    location="home"
    zone="*"
    target="*"

    try:
        opts, args = getopt.getopt(sys.argv[1:], "c:ht:o:vz:", ["config=","help","target=","operation=","verbose","zone="])

        for o,a in opts:
            if o in ["-h","--help"]:
                usage()
                sys.exit()
            elif o in ["-c","--config"]:
                configFile = a
            elif o in ["-t","--target"]:
                target=a
            elif o in ["-o","--operation="]:
                operation=a
            elif o in ["-v","--verbose"]:
                verbose=True
            elif o in ["-z","--zone="]:
                zone=a


    except getopt.GetoptError as err:
        print(err)
        sys.exit(2)

    base="/" + location + "/" + zone + "/" + target + "/*"

    if verbose:
        print("Config   : " + configFile)
        print("Target   : " + target )
        print("Operation:" + operation)
        print("Base     :" + base)

    cfg = cp.ConfigParser()
    cfg.read( configFile )


    redisHost="localhost"
    redisPort=6379

    localMqttHost="localhost"
    localMqttPort=1883

    redisHost = cfg.get('common','redis-host')
    redisPort = cfg.get('common','redis-port')

    if verbose:
        print( redisHost )
        print( redisPort )

    rc = redis.StrictRedis(redisHost, port=redisPort, db=0)


    request = operation.upper()
    out = target

    if request == "STATUS":
        print("Status")
        topics=rc.keys( base )

        for t in topics:
            k=t.decode(encoding='UTF-8')
            v=rc.get( k )
            print( k + " " + v.decode(encoding='UTF-8'))
            
        print()

    elif request in ("ON", "OFF") :
        if verbose:
            print(request)

        localClient = mqtt.Client()
        localClient.on_connect = onConnect

        localMqttHost = cfg.get('local','name')
        localMqttPort = int(cfg.get('local','port'))

        localClient.connect(localMqttHost, localMqttPort, 60)

#        topics=rc.keys( "*/" + out + "/power" )
        topics=rc.keys( base )
        for t in topics:
            k=t.decode(encoding='UTF-8')
            print( k +" " + request )
            localClient.publish(k, request, qos=0, retain=True)

            time.sleep(0.05)


main()
