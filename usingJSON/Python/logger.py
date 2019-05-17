#!/usr/bin/env python3
from time import gmtime, strftime
import paho.mqtt.client as mqtt
import sqlite3

# start_topic = "/home/#"
start_topic = "/test/#"

data = {}

def init():
    global data

    data["START"] = "OFF"

# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))

    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe(start_topic)
    
# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    global data
    shortName = ""

    theTime = strftime("%Y-%m-%d %H:%M:%S", gmtime())

    result = (msg.payload).decode("utf-8")

    if (msg.topic == start_topic):
        shortName = "START"
    # 
    # Update storage here
    # 
    print(theTime,shortName + " : ",msg.topic, result);
    return


init()

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

# client.connect("raspberrypi", 1883, 60)
client.connect("localhost", 1883, 60)
# client.connect("192.168.0.65", 1883, 60)

# Blocking call that processes network traffic, dispatches callbacks and
# handles reconnecting.
# Other loop*() functions are available that give a threaded interface and a
# manual interface.
client.loop_forever()
