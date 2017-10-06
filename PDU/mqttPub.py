#!/usr/bin/env python3
# #!/data/data/com.termux/files/usr/bin/env python3

import configparser as cp
import getopt
import sys
# import redis
import paho.mqtt.client as mqtt
import time

from os import getenv



def usage():
    print()
    print("Usage:./dBridge.py -h|-c <cfg> -l|-r -v ")

    print("\t-c <cfg>\tConfig directory.")
    print("\t-t <topic>\tMQTT Topic.")
    print("\t-m <msg>\tMQTT Payload.")

    print("\t-h\t\tHelp.")
    print("\t-l\t\tLocal.")
    print("\t-r\t\tRemote.")
    print("Note: Default is:")
    print("\t./dBridge.py -l ")
    print()

def localOnConnect(client, userdata, flags, rc):
    global verbose

    print("localOnConnect ",verbose)
    
    if verbose:
        print("Local Connected with result code "+str(rc))
        print("Subscribe to /home/office/+/power" )
        print("             /home/outside/+/power" )
        print("             /home/outside/+/cmnd/power")



def remoteOnConnect(client, userdata, flags, rc):
    global verbose

    pfix="/" + remoteMqttPrefix

    if verbose:
        print("Remote Connected with result code "+str(rc))
        print("Subscribe to " + pfix + "/home/office/+/power" )
        print("             " + pfix + "/home/outside/+/power" )
        print("             " + pfix + "/home/outside/+/cmnd/power")
    

def localOnMessage(client, userdata, msg):

    global verbose
    print("localOnMessage ",verbose)
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

            topic = "/" + remoteMqttPrefix + msg.topic
            print(topic)
            print( remoteMqttPrefix )
            print( m )
            remoteClient.publish(topic, m, qos=0, retain=True)
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



def main():
    global verbose
    
    localBroker  = True
    remoteBroker = False
    verbose = False
    configFolder="/etc/mqtt"
#    configFile="/etc/mqtt/bridge.ini"

    topic=""
    msg=""

#    home=getenv("HOME")
#    cert = home + '/.certs/dioty_ca.crt'

    global rc

    try:
        opts, args = getopt.getopt(sys.argv[1:], "c:hlrvt:m:", ["config=","help","local","remote","verbose","topic","message"])

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
                configFolder = a
            elif o in ["-t","--topic"]:
                topic = a
            elif o in ["-m","--message"]:
                msg = a
            else:
                print("Unknown option ",a)
                usage()
                sys.exit(1)

    except getopt.GetoptError as err:
        print(err)
        sys.exit(2)

    if( topic == "" or msg == ""):
        print("\nNeed a topic AND a payload")
        usage()
        sys.exit(2)


    cert = configFolder + '/dioty_ca.crt'
    configFile=configFolder + "/bridge.ini"

    if localBroker:
        localMqttHost="localhost"
        localMqttPort=1883

    remoteMqttHost="fred"
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


    if localBroker:
        localMqttHost = cfg.get('local','name')
        localMqttPort = int(cfg.get('local','port'))

    global localMqttPrefix
    localMqttPrefix = cfg.get('local','prefix')

    global remoteMqttPrefix
    remoteMqttHost = cfg.get('remote','name')
    remoteMqttPort = int(cfg.get('remote','port'))
    remoteMqttPrefix = cfg.get('remote','prefix')
    remoteMqttPassword = cfg.get('remote','password')
    remoteMqttCert = cfg.get('remote','cert',)

    if verbose:
        print()
        if localBroker:
            print("Local  MQTT Broker  : " + localMqttHost)
            print("            Port    : " , localMqttPort)

            print("            Prefix  : " + localMqttPrefix)

        print("Remote MQTT Broker  : " + remoteMqttHost)
        print("            Port    : " + str(remoteMqttPort))
        print("            Prefix  : " + remoteMqttPrefix)
        print("            Password: " + remoteMqttPassword)
        print("            Cert    : " + remoteMqttCert)
        print("")
        print("            Topic   : " + topic)
        print("            Message : " + msg)


    global localClient

    if localBroker:
        localClient = mqtt.Client()
        localClient.on_connect = localOnConnect
        localClient.on_message = localOnMessage
        localClient.connect(localMqttHost, localMqttPort, 60)
        localClient.publish(topic, msg, qos=0, retain=True)


    global remoteClient

    if remoteBroker:
        certFile= configFolder + '/' + remoteMqttCert
        print("certFile is ", certFile)
        remoteClient = mqtt.Client()
        remoteClient.on_connect = remoteOnConnect
#        remoteClient.on_message = remoteOnMessage
        print("Remote",remoteMqttPrefix,remoteMqttPassword)
        remoteClient.username_pw_set(remoteMqttPrefix,remoteMqttPassword)
        remoteClient.tls_set(certFile)
    
        remoteClient.connect(remoteMqttHost, remoteMqttPort)

        remoteTopic = "/" + remoteMqttPrefix + topic
        if verbose:
            print("     Remote Topic   : " + remoteTopic)

        remoteClient.publish(remoteTopic, msg, qos=0, retain=True)

main()

