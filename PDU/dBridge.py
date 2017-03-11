#!/usr/bin/env python3

import configparser as cp
import getopt
import sys
import redis
import paho.mqtt.client as mqtt
import time

from os import getenv



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

def localOnConnect(client, userdata, flags, rc):
    print("Local Connected with result code "+str(rc))

    client.subscribe("/home/office/+/power")

def remoteOnConnect(client, userdata, flags, rc):
    print("Remote Connected with result code "+str(rc))

    pfix="/" + remoteMqttPrefix

    print("Subscribe to " + pfix+"/home/office/+/power" )
    client.subscribe(pfix+"/home/office/+/power")



def localOnMessage(client, userdata, msg):

    changed=False

    m=(msg.payload).decode()
    d=rc.get(msg.topic)

    if d == None:
        rc.set(msg.topic, m)
        changed=True
    else:
        d=d.decode()

        if m != d :
            print("Changed")
            rc.set(msg.topic, m)

            topic = "/" + remoteMqttPrefix + msg.topic
            print(topic)
            print( remoteMqttPrefix )
            print( m )
            remoteClient.publish(topic, m, qos=0, retain=True)

def remoteOnMessage(client, userdata, msg):
    print("Remote message")
    pfix="/" + remoteMqttPrefix

    payload=(msg.payload).decode()

    print(msg.topic)
    print(payload)
    print((msg.topic).split("/",2))
    tmp=(msg.topic).split("/",2)

    key="/" + tmp[2]
    print( key )
    d=rc.get(key)

    if d != None:
        val=d.decode()
        if val != payload:
            print("Remote change")
            localClient.publish(key, payload, qos=0, retain=True)
        else:
            print("Remote NO change")



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

    remoteMqttHost="fred"
    remoteMqttPort=1883

    home=getenv("HOME")


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

    global localMqttPrefix
    localMqttPrefix = cfg.get('local','prefix')

    global remoteMqttPrefix
    remoteMqttHost = cfg.get('remote','name')
    remoteMqttPort = int(cfg.get('remote','port'))
    remoteMqttPrefix = cfg.get('remote','prefix')
    remoteMqttPassword = cfg.get('remote','password')

    if verbose:
        print()
        print("Local  MQTT Broker  : " + localMqttHost)
        print("            Port    : " , localMqttPort)
        print("            Prefix  : " + localMqttPrefix)
        print("Remote MQTT Broker  : " + remoteMqttHost)
        print("            Port    : " , remoteMqttPort)
        print("            Prefix  : " + remoteMqttPrefix)
        print("            Password: " + remoteMqttPassword)


    global localClient
    localClient = mqtt.Client()
    localClient.on_connect = localOnConnect
    localClient.on_message = localOnMessage

    certFile= home + "/.certs/dioty_ca.crt"
    global remoteClient
    remoteClient = mqtt.Client()
    remoteClient.on_connect = remoteOnConnect
    remoteClient.on_message = remoteOnMessage
    print("Remote",remoteMqttPrefix,remoteMqttPassword)
    remoteClient.username_pw_set(remoteMqttPrefix,remoteMqttPassword)
    remoteClient.tls_set(certFile)


#    client.connect("127.0.0.1", 1883, 60)
    remoteClient.connect(remoteMqttHost, remoteMqttPort)
    localClient.connect(localMqttHost, localMqttPort, 60)

#    remoteClient.publish("/andrewtholt60@gmail.com/home/office/relay1/power", "UNKNOWN", qos=0, retain=True)


    lrc=localClient.loop_start()
    rrc=remoteClient.loop_start()

    print(lrc)
    print(rrc)


    time.sleep(3000)
#    localClient.loop_forever()

    return

main()

