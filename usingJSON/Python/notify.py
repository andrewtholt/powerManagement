#!/usr/bin/env python3

import sys 
import os.path
import time
import getopt

from pushbullet import Pushbullet
import pymysql as sql 
import json

db = None

connected=False

def getToMap(conn, sqlCmd):
    print("getToMap")

def stateToLogic( state ) :

    if state in ["ON","TRUE"]:
        return "TRUE"

    if state in ["OFF","FALSE"]:
        return "FALSE"

def main():
    global db
    configFile="/etc/mqtt/bridge.json"
    database = 'localhost'

    old = 0
    verbose = False
    testing = False

    cache = {}

    try:
        opts,args = getopt.getopt(sys.argv[1:], "c:hvt", ["config=","help","verbose","test"])
        for o,a in opts:
            if o in ["-h","--help"]:
                usage()
                sys.exit()
            elif o in ["-c","--config"]:
                configFile = a
            elif o in ["-v","--verbose"]:
                verbose=True
            elif o in ["-t","--test"]:
                testing=True

    except getopt.GetoptError as err:
        print(err)
        usage()
        sys.exit(2)

    token=""
    tokenFile = open("token.txt","r")

    if tokenFile.mode =="r":
        token=(tokenFile.read()).rstrip("\n\r")

    tokenFile.close()

    if testing == True:
        print("Would have sent " + token )
    else:
        pb = Pushbullet(token)

    if os.path.exists(configFile):
        with open( configFile, 'r') as f:
            cfg = json.load(f)

        database = cfg['database']['name']
        useDb    = cfg['database']['db']
        user     = cfg['database']['user']
        passwd   = cfg['database']['passwd']
    else:
        print(configFile + " not found..")
        print(".. using defaults")

#    cache['mqtt']     = {'old':0, 'stamp':0}
#    cache['internal'] = {'old':0, 'stamp':0}
    cache['io_point']     = {'old':0, 'stamp':0}

    print("Database is " + database)
    while True:
        db = sql.connect(database, user,passwd, useDb)
        sqlCmd=""
        for table in cache.keys():
            cursor = db.cursor()
            sqlCmd = "select name,state,unix_timestamp(logtime),notify from " + table + " order by logtime desc limit 1;"
            cursor.execute( sqlCmd )
            data = cursor.fetchone()
    
            name    = data[0]
            state   = data[1]
            logtime = data[2]
            notify  = data[3]
    
            if verbose:
                print(sqlCmd)
                print("Name   :" + name)
                print("State  :" + state)
                print("Logtime:" , logtime )
    
            if cache[ table ]['old'] == 0:
#                print("First time")
                cache[ table ]['old'] = logtime
                cache[ table ]['stamp'] = logtime
            else:
#                print("Been here before")
#                print("logtime:",logtime )
#                print("Old    :" , cache[ table ]['old']) 

                if logtime > cache[ table ]['old'] :

                    if verbose:
                        print( name + " was set to " + state + " at ", logtime )
                    cache[ table ]['old'] = logtime
#                    push = pb.push_note("This is the title", "This is the body")
                    if notify == 'YES':
                        if testing:
                            print("Would have published " + name + ":" + state)
                        else:
                            push = pb.push_note(name, state)
                            print(push)
            
        cursor.close()
        db.close()
        time.sleep(10)


main()
