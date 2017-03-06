#!/usr/bin/env python3

import configparser as cp
import getopt
import sys
import redis
import paho.mqtt.client as mqtt



def usage():
    print()
    print("Usage:./dBridge.py -h|-c <cfg> -l|-r -v ")

    print("\t-c <cfg>\tConfig file.")
    print("\t-h\t\tHelp.")
    print("\t-l\t\tLocal.")
    print("\t-r\t\tRemote.")
    print("Note: Default is:")
    print("\t./dBridge.py -l ")
    print()

def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))

    client.subscribe("/home/office/+/power")



def on_message(client, userdata, msg):

    changed=False

    m=(msg.payload).decode()
#    print(msg.topic+" "+ m)
    d=rc.get(msg.topic)

    if d == None:
#        print("Not found")
        rc.set(msg.topic, m)
        changed=True
    else:
#        print("Found")
        d=d.decode()

        if m != d :
            print("Changed")
# Publish to remote broker
            rc.set(msg.topic, m)



def main():
    localBroker  = True
    remoteBroker = False
    verbose = False
#    configFile="/etc/mqtt/bridge.ini"
    configFile="./bridge.ini"
    global rc

    try:
        opts, args = getopt.getopt(sys.argv[1:], "c:hlrv", ["config=","help","local","remote","verbose"])

        for o,a in opts:
            if o in ["-h","--help"]:
                usage()
                sys.exit()
            elif o in ["-l","--local"]:
                localBroker=True
                remoteBroker=False
            elif o in ["-r","--remote"]:
                localBroker=False
                remoteBroker=True
            elif o in ["-v","--verbose"]:
                verbose=True
            elif o in ["-c","--config"]:
                configFile = a

    except getopt.GetoptError as err:
        print(err)
        sys.exit(2)

    localMqttHost="localhost"
    localMqttPort=1883

    remoteMqttHost="broken"
    remoteMqttPort=1883

    cfg = cp.ConfigParser()
    cfg.read( configFile )

    if verbose:
        print()
        if localBroker:
            print("Local broker")
        if remoteBroker:
            print("Remote broker")

        print("Config File:"+configFile)
        print()


    redisHost="fred"
    redisPort = 6379

    redisHost = cfg.get('common','redis-host')
    redisPort = cfg.get('common','redis-port')

    print( redisHost )
    print( redisPort )

    rc = redis.StrictRedis(redisHost, port=redisPort, db=0)

    for k in rc.scan_iter("/home/office/*"):
        print(k.decode(), (rc.get(k)).decode())

    localMqttHost = cfg.get('local','name')
    localMqttPort = int(cfg.get('local','port'))

    localClient = mqtt.Client()
    localClient.on_connect = on_connect
    localClient.on_message = on_message

#    client.connect("127.0.0.1", 1883, 60)
    localClient.connect(localMqttHost, localMqttPort, 60)

    localClient.loop_forever()

    return

main()

