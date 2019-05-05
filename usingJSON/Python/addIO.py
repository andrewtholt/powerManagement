#!/usr/bin/env python3

import sys 
import os.path
import pymysql as sql 
import json
import paho.mqtt.client as mqtt
import time
import getopt

from time import sleep

import curses

def main():
    database = 'localhost'
    mqttBroker = 'localhost'
    mqttPort = 1883


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

    stdscr = curses.initscr()

    begin_x = 20; begin_y = 7
    height = 25; width = 80

    errLine=15
    inColumn=22

    win = curses.newwin(height, width, begin_y, begin_x)

    while True:
        win.addstr(10,5, "Name           :")
        win.addstr(11,5, "Direction [IN] :")
        win.addstr(12,5, "IO Type [MQTT] :")

        name = (win.getstr(10, inColumn)).decode("utf-8")

        while True:
            direction = (win.getstr(11, inColumn)).decode("utf-8")
    
            if direction in ['IN', 'OUT', 'DISABLED', 'INTERNAL','']:
                if direction == '':
                    direction = 'IN'
                    win.move(11, inColumn)
                    win.addstr( direction )

                win.move(errLine,5)
                win.clrtoeol()
                break
            else:
                win.move(11,inColumn)
                win.clrtoeol()
                win.move(errLine,5)
                win.addstr("'IN', 'OUT', 'DISABLED', or 'INTERNAL' not '" + direction + "'" );
    
    
        while True:
            ioType = (win.getstr(12, inColumn)).decode("utf-8")
    
            if ioType in ['MQTT', 'INTERNAL','']:
                if ioType == '':
                    ioType = 'MQTT'
                    win.move(12,inColumn)
                    win.addstr( ioType )

                win.move(errLine,5)
                win.clrtoeol()
                break
            else:
                win.move(12,inColumn)
                win.clrtoeol()
                win.move(errLine,5)
                win.addstr("'MQTT', 'INTERNAL' not " + ioType );
    
        if ioType == "MQTT":
            win.addstr(13,5, "Topic          :")
            topic = (win.getstr(13, inColumn)).decode("utf-8")

        win.addstr(14,5, "OK [Y/n]       :")
    
        ok = (win.getstr(14, inColumn)).decode("utf-8")

        win.refresh()

        if ok in ["","Y","y"]:
            break
        else:
            win.clear()
    


    curses.endwin()

    cursor = db.cursor()

    if ioType == "MQTT":
        sqlCmd = "replace into " + ioType.lower() + "(name, topic ) values ('" + name + "','"+topic+ "');"
    elif ioType == "INTERNAL":
        sqlCmd = "replace into " + ioType.lower() + "(name) values ('" + name + "');"

    print(sqlCmd)
    cursor.execute( sqlCmd )

    sqlCmd = "replace into io_point (name,direction ) values ('" + name + "','"+ direction +"');"
    print(sqlCmd)
    cursor.execute( sqlCmd )

    sqlCmd = "update io_point," + ioType.lower() +  " set io_point.io_idx=" + ioType.lower() + ".idx where io_point.name = " + ioType.lower() + ".name;"

    print(sqlCmd)

    cursor.execute( sqlCmd )

    db.commit()
    cursor.close()
    db.close()





main()

