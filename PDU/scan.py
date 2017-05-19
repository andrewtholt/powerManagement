#!/usr/bin/python3

import getopt
import sys
import paho.mqtt.client as mqtt

from time import sleep


def usage():
    print("Usage: scan.py -h|-t <topic> -m <msg>")

def main():

    topic = ""
    msg = ""
    verbose = True
    host = "127.0.0.1"
    port = 1883

    try:
        opts, args = getopt.getopt(sys.argv[1:], "?t:m:p:h:",["help","topic","message","port","host"])
    except getopt.GetoptError as err:
        print(err)
        sys.exit(2)

    for o,a in opts:
        if o in ("-?","--help"):
            usage()
            sys.exit(0)
        elif o in ("-t","--topic"):
            topic = a
        elif o in ("-m","--message"):
            msg = a
        elif o in ("-h","--host"):
            host = a
        elif o in ("-p","--port"):
            port = int(a)

    if topic == "" or msg == "":
        usage()
        sys.exit(1)

    if verbose:
        print("Topic:" + topic)
        print("Msg  :" + msg)
        print("Host :" + host)
        print("Port :", port)

    client = mqtt.Client()
    client.connect(host,port,60)

    for t in topic.split():
        print(t)
        print(msg)

        client.publish(t,msg,qos=0,retain=True)
        sleep(0.1)

main()
