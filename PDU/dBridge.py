#!/usr/bin/env python3

import configparser as cp
import getopt
import sys
import redis
import paho.mqtt.client as mqtt
import time

import signal
import os
import os.path
from time import sleep


def usage():
    print()
    print("Usage:./dBridge.py -h|-c <cfg folder> -l|-r -v ")

    print("\t-c <cfg>\tConfig Folder.")
    print("\t-h\t\tHelp.")
    print("\t-l\t\tLocal.")
    print("\t-r\t\tRemote.")
    print("\t-x\t\tExecute script on events.")
    print("Note: Default is:")
    print("\t./dBridge.py -l ")
    print()

def localOnConnect(client, userdata, flags, returnCode):
    global verbose
    
    if verbose:
        print("Local Connected with result code "+str(rc))
        print("Subscribe to /home/office/+/power" )
        print("             /home/outside/+/power" )
        print("             /home/outside/+/cmnd/power")

    client.subscribe("/home/office/+/power")
    client.subscribe("/home/outside/+/power")
    client.subscribe("/home/outside/+/cmnd/power")
    client.subscribe("/home/environment/#")

def remoteOnConnect(client, userdata, flags, rc):
    global verbose

    pfix="/" + remoteMqttPrefix

    if verbose:
        print("Remote Connected with result code "+str(rc))
        print("Subscribe to " + pfix + "/home/office/+/power" )
        print("             " + pfix + "/home/outside/+/power" )
        print("             " + pfix + "/home/outside/+/cmnd/power")
    
    
    client.subscribe(pfix + "/home/office/+/power")
    client.subscribe(pfix + "/home/outside/+/power")
    client.subscribe(pfix + "/home/outside/+/cmnd/power")



def localOnMessage(client, userdata, msg):

    global verbose
    changed=False

    m=(msg.payload).decode()
    d=rc.get(msg.topic)

    if d == None:
        rc.set(msg.topic, m)
        changed=True
    else:
        d=d.decode()

        if m != d :
            print("Local: Change")
            rc.set(msg.topic, m)
            sleep(0.5)

            topic = "/" + remoteMqttPrefix + msg.topic
            print(topic)
            print( remoteMqttPrefix )
            print( m )
            remoteClient.publish(topic, m, qos=0, retain=True)
            
#            sleep(2)
        else:
            print("Local: NO Change")

def remoteOnMessage(client, userdata, msg):
    global verbose
    print("Remote message")
    pfix="/" + remoteMqttPrefix

    payload=(msg.payload).decode()

    tmp=(msg.topic).split("/",2)

    key="/" + tmp[2]

    d=rc.get(key)

    if d != None:
        val=d.decode()
        if val != payload:
            print("Remote: Change")
            localClient.publish(key, payload, qos=0, retain=True)
        else:
            print("Remote: NO change")



def process():
    global verbose
    
    localBroker  = True
    remoteBroker = False
    verbose = False
    execute = False
    configFolder="/etc/mqtt"
#    configFile="/etc/mqtt/bridge.ini"
    global rc

    try:
        opts, args = getopt.getopt(sys.argv[1:], "c:hlrvx", ["config=","help","local","remote","verbose","execute"])

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
#                configFile = a
                configFolder = a
            elif o in ["-x","--execute"]:
                execute = True

    except getopt.GetoptError as err:
        print(err)
        sys.exit(2)

    localMqttHost="localhost"
    localMqttPort=1883

    remoteMqttHost="fred"
    remoteMqttPort=1883

    home=os.getenv("HOME")

    configFile=configFolder + "/bridge.ini"

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


#    redisHost='localhost'
#    redisPort = 6379

    redisHost = cfg.get('common','redis-host',fallback='localhost')
    redisPort = cfg.getint('common','redis-port', fallback=6379)

    print( redisHost )
    print( redisPort )

    rc = redis.StrictRedis(redisHost, port=redisPort, db=0)

    for k in rc.scan_iter("/home/office/*"):
        print(k.decode(), (rc.get(k)).decode())

    localMqttHost = cfg.get('local','name',fallback='localhost')
    localMqttPort = cfg.getint('local','port', fallback=1883)

    global localMqttPrefix
    localMqttPrefix = cfg.get('local','prefix',fallback='')

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

#    certFile= home + "/.certs/dioty_ca.crt"
    certFile= configFolder + "/dioty_ca.crt"

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


    while True:
        lrc=localClient.loop_start()
        rrc=remoteClient.loop_start()
    
#        print(lrc)
#        print(rrc)

        time.sleep(3000)

    return

def handler():
    print("HUP")

def main():

    signal.signal(signal.SIGHUP, handler)

    pidFileName="/var/run/dBridge.pid"
    iam = os.getpid()
    pidFile = open(pidFileName,'w')
    iamStr = str(iam) + '\n'

    pidFile.write(iamStr)
    pidFile.close()

    try:
        process()
    except:
        os.remove(pidFileName)
        sys.exit(1)


main()


