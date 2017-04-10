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
    print("Usage: dPower.py <on|off|statsus> <outlet name>")
    print()
    print("e.g. dPower.py on garden")
    print('     dPower.py status "ga*"')
    print()


def main():
    cmd=""
    verbose=False

    configFile="/etc/mqtt/bridge.ini"
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

    if len(sys.argv) < 3:
        usage()
        sys.exit(0)

    request = sys.argv[1].upper()
    out = sys.argv[2]

    if request == "STATUS":
        print("Status")
        topics=rc.keys( "*/" + out + "/power" )

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

        topics=rc.keys( "*/" + out + "/power" )
        for t in topics:
            k=t.decode(encoding='UTF-8')
            print( k +" " + request )
            localClient.publish(k, request, qos=0, retain=True)

            time.sleep(0.05)


main()
