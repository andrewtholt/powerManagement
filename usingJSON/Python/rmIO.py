#!/usr/bin/env python3

import sys 
import os.path
import pymysql as sql 
import json
import paho.mqtt.client as mqtt
import time
import getopt

def main():
    database = 'localhost'
    mqttBroker = 'localhost'
    mqttPort = 1883

    if len(sys.argv) != 2:
        print("Usage: rmIO.py <io point name>")

        sys.exit(1)

    name =sys.argv[1]

    configFile="/etc/mqtt/bridge.json"

    if os.path.exists(configFile):
        with open( configFile, 'r') as f:
            cfg = json.load(f)

        mqttBroker = cfg['local']['name']
        mqttPort   = int(cfg['local']['port'])

        database = cfg['database']['name']
        db       = cfg['database']['db']
        user     = cfg['database']['user']
        passwd   = cfg['database']['passwd']
    else:
        print(configFile + " not found..")
        print(".. using defaults")

    db = sql.connect(database, user, passwd,db)


    cursor = db.cursor()

    sqlCmd = "select io_type,io_idx from io_point where name = '" + name + "';"
    print(sqlCmd)
    cursor.execute( sqlCmd )
    data = cursor.fetchone()

    print(data)

    ioType = data[0]
    ioIdx = data[1]

    sqlCmd = "delete from io_point where name = '" + name + "';"
    print(sqlCmd)
    cursor.execute( sqlCmd )

    sqlCmd = "delete from " + ioType.lower() + " where idx = " + str(ioIdx) + ";"
    print(sqlCmd)
    cursor.execute( sqlCmd )

    db.commit()
    cursor.close()
    db.close()





main()

