#!/usr/bin/python3
import os
import getopt
import sys
import time
import paho.mqtt.client as mqtt
import configparser as cp
import sqlite3 as sqlite  # datalogger


def on_connect(client, userdata, flags, rc):
    return

def on_message(client, userdata, msg):
    return

def usage():
    print("Help ... is not available\n")
    return

def main():

    verbose=False
    configFile="/etc/mqtt/bridge.ini"

    try:
        opts,args = getopt.getopt(sys.argv[1:],"c:hv",["config=","help","verbose"])

        for o,a in opts:
            if o in ["-h","--help"]:
                usage()
                sys.exit(0)
            elif o in ["-v","--verbose"]:
                verbose=True
            elif o in ["-c","--config"]:
                configFile=a
    except getopt.GetoptError as err:
        usage()
        sys.exit(1)

    if verbose:
        print("Config : " + configFile)

    if os.path.exists(configFile):
        cfg = cp.ConfigParser()
        cfg.read( configFile )
    else:
        print('No such file as ' + configFile, file=sys.stderr)
        usage()
        sys.exit(2)

    cmd='/sbin/apcaccess'
    dataDict={}

    res=os.popen( cmd )

    data=res.readlines()

    for l in data:
        line=l.strip()
        tmp = line.split(':')

        key=tmp[0].strip()
        val=tmp[1].strip()

        dataDict[key] = val

    mqttBroker=cfg.get('local','name')
    mqttPort=int(cfg.get('local','port'))

    if verbose:
        print("Broker : " + mqttBroker)
        print("Port   : %d " % (mqttPort))

    client = mqtt.Client()
    client.on_connect = on_connect
    client.connect(mqttBroker, mqttPort, 60)

    client.loop_start ()

    pdir = os.getenv("PDIR")

    db = pdir + "/data/supplyLog.db"
    con = sqlite.connect( db )
    cur = con.cursor()


    sqlTemplate = "insert into line_voltage_log (VAC) values (%s);"

    pubList = [ 'STATUS','LINEV', 'BATTV','OUTPUTV','MAXLINEV','MINLINEV','LOADPCT','TIMELEFT' ]
    # pubList = [ 'OUTPUTV' ]
    pfix="/home/office/ups/"

    for t in pubList:
        topic = pfix + t
        payload = (dataDict[t].split(' '))[0]

        if verbose:
            print(topic, payload)

        client.publish(topic, payload,qos=0,retain=True )

        if t == 'LINEV':
            sql = sqlTemplate %( payload )
            if verbose:
                print(sql)

            cur.execute( sql )
            con.commit()


    con.close()
    time.sleep(1)
    return

main()
