#!/usr/bin/python3

import paho.mqtt.client as mqtt
import configparser as cp
import getopt
import sys
import os
import time
import redis

verbose=False
redisClient=None

def usage():
    print("Help")

def on_connect(client, userdata, flags, rc):
    global verbose

    if verbose:
        print("Connected")
    # 
    # TODO: SHould this list be part of the config.
    # 
    client.subscribe("/home/office/+/power")
    client.subscribe("/home/outside/+/power")
    client.subscribe("/home/outside/+/cmnd/power")
    client.subscribe("/home/environment/#")
    return

def on_message(client, userdata, msg):
    global redisClient
    global verbose

    t=msg.topic
    m=(msg.payload).decode()
   
    if verbose:
        print("Message")
        print("Topic : " + t )
        print("Msg   : " + m )
        print("=======")

    redisClient.set(t,m)

def main():
    global verbose
    configFile="/etc/mqtt/bridge.ini"
    global verbose
    global redisClient

    opts,args = getopt.getopt(sys.argv[1:],"c:hv",["config=","help","verbose"])

    for o,a in opts:
        if o in ["-h","--help"]:
            usage()
            sys.exit(0)
        elif o in ["-c","--config"]:
            configFile = a
        elif o in ["-v","--verbose"]:
            verbose=True


    if os.path.exists(configFile):
        cfg = cp.ConfigParser()
        cfg.read( configFile )
        mqttBroker=cfg.get('local','name')
        mqttPort=int(cfg.get('local','port'))
    else:
        print('No such file as ' + configFile, file=sys.stderr)
        usage()
        sys.exit(2)

    if verbose:
        print("Broker : " + mqttBroker)
        print("Port   : " + str(mqttPort))

    # 
    # Connect to mqtt broker.
    #
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message

    client.connect(mqttBroker, mqttPort, 60)
    # 
    # OK, now connect to redis.
    # 

    redisHost = cfg.get('common','redis-host',fallback='localhost')
    redisPort = cfg.getint('common','redis-port', fallback=6379)

    if verbose:
        print("Redis Host : " + redisHost)
        print("      Port : " + str(redisPort) )
   
    redisClient = redis.StrictRedis(redisHost, port=redisPort, db=0)
    
    client.loop_start()
    time.sleep(3)

    

main()                


