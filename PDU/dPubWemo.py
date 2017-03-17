#!/usr/bin/python3

import configparser as cp
from  dWemo import wemo
import getopt
import paho.mqtt.client as mqtt
from sys import exit, argv


def on_connect(client, userdata, flags, rc):

    global wd

    client.publish(topic, wd.getStatus())

    client.subscribe( topic )

def on_message(client, userdata, msg):
    global topic

    m=(msg.payload).decode()

    wd.setStatus( m )

def usage():
    print("Help\n")

def main():

    configFile="./bridge.ini"
    device = "UNKNOWN"
    verbose=False

    topicTemplate="/home/outside/%s/power"

    global topic
    topic=""

    global wd


    try:
        opts,args = getopt.getopt(argv[1:],"c:d:hv", ["config=","device=""help","verbose"])

        for o,a in opts:
            if o in ["-h","--help"]:
                usage()
                exit()
            elif o in ["-v","--verbose"]:
                verbose=True
            elif o in ["-c","--config"]:
                configFile = a
            elif o in ["-d","--device"]:
                device = a
                topic=topicTemplate % (device)


    except getopt.GetoptError as err:
        print(err)
        exit(1)

    if verbose:
        print("Config : " + configFile)
        print("Device : " + device)
        print("Topic  : " + topic)

    cfg = cp.ConfigParser()
    cfg.read( configFile )

    mqttBroker=cfg.get('local','name')
    mqttPort=int(cfg.get('local','port'))

    if verbose:
        print("Broker : " + mqttBroker)
        print("Port   : %d " % (mqttPort))

    wd = wemo(device)

    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message

    client.connect(mqttBroker, mqttPort, 60)

    client.loop_forever()


    
    
#    device='PorchLight'
#    porchLight = wemo(device)



main()
