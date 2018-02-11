#!/usr/bin/python3

import configparser as cp
from  dWemo import wemo
import getopt
import paho.mqtt.client as mqtt
import sys
import os

verbose=True

def on_connect(client, userdata, flags, rc):

    global wd

    client.publish(topic, wd.getStatus())

    client.subscribe( topic )

def on_message(client, userdata, msg):
    global topic

    m=(msg.payload).decode()

    if verbose:
        print("Payload = ",m)

    wd.setStatus( m )

def usage():
    print()
    print("Usage: dPubWemo.py -h|-c <config file>|-v|-d <WeMo name>\n")
    print("\t-c <file>|--config=<file>\tConfig file")
    print("\t-h|--help\t\t\tHelp.")
    print("\t-v|--verbose\t\t\tverbose.")
    print("\t-d <name>|--device=<name>\tWeMo device name.")

    print("Default Action:")
    print("\tdPubWemo.py -c ./bridge.ini\n")

def main():
    global verbose

#    configFile="./bridge.ini"
    configFile="/etc/mqtt/bridge.ini"
    device = "UNKNOWN"
    verbose=False

    topicTemplate="/home/outside/%s/power"

    global topic
    topic=""

    global wd


    try:
        opts,args = getopt.getopt(sys.argv[1:],"c:d:hv", ["config=","device=","help","verbose"])

        for o,a in opts:
            if o in ["-h","--help"]:
                usage()
                sys.exit()
            elif o in ["-v","--verbose"]:
                verbose=True
            elif o in ["-c","--config"]:
                configFile = a
            elif o in ["-d","--device"]:
                device = a
                topic=topicTemplate % (device)


    except getopt.GetoptError as err:
        usage()
        sys.exit(1)

    if verbose:
        print("Config : " + configFile)
        print("Device : " + device)
        print("Topic  : " + topic)

    if os.path.exists(configFile):
        cfg = cp.ConfigParser()
        cfg.read( configFile )
    else:
        print('No such file as ' + configFile, file=sys.stderr)
        usage()
        sys.exit(2)

    mqttBroker=cfg.get('local','name')
    mqttPort=int(cfg.get('local','port'))

    if verbose:
        print("Broker : " + mqttBroker)
        print("Port   : %d " % (mqttPort))

    wd = wemo(device)
    wd.setVerbose(verbose)

    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message

    client.connect(mqttBroker, mqttPort, 60)

    iam=os.getpid()

    if not verbose:
        print(iam)

    client.loop_forever()
    
#    device='PorchLight'
#    porchLight = wemo(device)



main()
