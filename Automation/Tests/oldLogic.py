#!/usr/bin/env python3

import sys
import sqlite3 as sqlite
import getopt
import os.path
from os import getenv
import paho.mqtt.client as mqtt
import configparser as cp

class logicVM:
    ip=0
    logicFile=""
    db   = None
    pdir = None
    configFile="/etc/mqtt/bridge.ini"

    def __init__(self, lf="logic.txt"):
        self.logicFile = lf
        self.db = getenv("CTL_DB")
        self.pdir = getenv("CTL_PDIR")
        if self.db == None or self.pdir == None:
            print("FATAL ERROR: setup CTL_PDIR & CTL_DB env variables")
            sys.exit(1)




verbose=False
db=None

subCount=0

def usage():
    print("Usage: logic.py ...")

def engine():
    print("Engine")

def toOn( state ):
    if state == "TRUE" or state == "ON" or state == "YES":
        state="TRUE"
    elif state == "FALSE" or state == "OFF" or state == "NO":
        state="FALSE"
    else:
        state="UNKNOWN"
    return state

def on_message(client, userdata, msg):
    global subCount

    print("On Message")
#    print(msg.topic+" "+str(msg.payload))
    state = toOn((msg.payload).decode("utf-8"))
    print(msg.topic+" "+ state )

    # 
    # TODO how to test for rising/falling edge ?
    #
    sql = "update io_point set state='%s' where topic='%s'" %( state, msg.topic)
    print(">> " + sql)
    con = sqlite.connect( db )
    cur = con.cursor()

    cur.execute(sql)

    con.commit()
    con.close()

    subCount -=1

    if subCount < 0:
        subCount=0

    if subCount > 0:
        print("== subCount ",subCount)
    elif subCount == 0:
        engine()


def on_connect(client, userdata, flags, rc):
    print("On connect")

def main():
    global verbose
    global db
    global subCount

    configFile="/etc/mqtt/bridge.ini"

    db = getenv("CTL_DB")
    pdir = getenv("CTL_PDIR")

    if db == None or pdir == None:
        print("FATAL ERROR: setup CTL_PDIR & CTL_DB env variables")
        sys.exit(1)


    try:
        opts, args = getopt.getopt(sys.argv[1:], "c:hv", ["config=","help","verbose"])
        for o,a in opts:
            if o in ["-h","--help"]:
                usage()
                sys.exit(0)
            elif o in ["-c","--config"]:
                configFile = a 
            elif o in ["-v","--verbose"]:
                verbose = True

    except getopt.GetoptError as err:
        print(err)
        usage()
        sys.exit(2)

    machine = logicVM()

    if os.path.exists(configFile):
        cfg = cp.ConfigParser()
        cfg.read( configFile )
        mqttBroker=cfg.get('local','name')
        mqttPort=int(cfg.get('local','port'))
    else:
        print('No such file as ' + configFile, file=sys.stderr)
        sys.exit(2)

    if verbose:
        print("Broker : " + mqttBroker)
        print("Port   : " + str(mqttPort))

    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message

    client.connect(mqttBroker, mqttPort, 60)

    con = sqlite.connect( db )
    cur = con.cursor()

    sql = "select name,topic from io_point where direction = 'IN'"
    cur.execute(sql)

    subCount = 0
    for point in cur.fetchall():
        name  = point[0]
        topic = point[1]

        client.subscribe( topic )
        subCount=subCount+1

        if verbose:
            print("=========")
            print("Name  : " + name)
            print("Point : " + topic)
            print("Count : %d" % subCount)
            print("=========")


    client.loop_forever()


main()
