#!/usr/bin/env python3

import sys
import pymysql as mysql
import getopt
import os.path
from os import getenv
import paho.mqtt.client as mqtt
import configparser as cp

sys.path.append(".")

import logicVM as logic


verbose=False
machine=None

subCount=0

def usage():
    print("Usage: logic.py ...")

def toOn( state ):
    if state == "TRUE" or state == "ON" or state == "YES":
        state="TRUE"
    elif state == "FALSE" or state == "OFF" or state == "NO":
        state="FALSE"
    else:
        state="UNKNOWN"
    return state

def updateOutput(client):
    con = mysql.connect("localhost", "automation","automation","automation")
    cur = con.cursor()

    sql = "select topic,state from io_point where direction = 'OUT' and state <> 'UNKNOWN'"

    print("\t " + sql)

    cur.execute( sql )

    for point in cur.fetchall():
        topic = point[0]
        msg = point[1]

        print("\t " + topic)
        print("\t " + msg)

        client.publish(topic, msg, qos=0, retain=True)

    con.commit()
    cur.close()
    con.close()


def on_message(client, userdata, msg):
    global subCount

    print("On Message")

    state = toOn((msg.payload).decode("utf-8"))
    print(msg.topic+" "+ state )


    con = mysql.connect("localhost", "automation","automation","automation")
    cur = con.cursor()

    sql = "use automation"
    cur.execute(sql)
    # 
    # TODO how to test for rising/falling edge ?
    #
    sql = "update io_point set state='%s' where topic='%s'" %( state, msg.topic)
    print(">> " + sql)
    cur.execute(sql)

    con.commit()
    cur.close()
    con.close()

    subCount -=1

    if subCount < 0:
        subCount=0

#    if subCount > 0:
#        print("== subCount ",subCount)
#    elif subCount == 0:
#        machine.run()
#        updateOutput(client)

    machine.run()
    updateOutput(client)


def on_connect(client, userdata, flags, rc):
    print("On connect")

def main():
    global verbose
    global db
    global subCount
    global machine

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

    machine = logic.logicVM()
    machine.verbose()
    machine.load()

    con,cur = machine.getDb()
    print(con)
    print(cur)

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

    # 
    # All conected, now run the machine to set o/ps
    # and get i/ps
    # 
    machine.run()
    updateOutput(client)

    sql = "select name,topic from io_point where direction = 'IN'"
    cur.execute(sql)

    subCount = 0
    for point in cur.fetchall():
        name  = point[0]
        topic = point[1]

        if len(topic) > 0:
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
