#!/usr/bin/python
# 
# Listen to a redis Q named event.
# on recieveing a valid power related event
# act as defined in the power outlet database.
# 
# 

import redis
from pysqlite2 import dbapi2 as sqlite

import sys
import os.path
from os import getenv,popen,remove


def main():
    verbose=True

    redisHost = "macbook"

    if verbose:
        print "Connect to", redisHost

    db = getenv("POWER_DB")

    pdir = getenv("PDIR")

    if db == None or pdir == None:
        print "FATAL ERROR: setup PDIR & POWER_DB env variables"
        sys.exit(1)

        con = sqlite.connect( db )
        cur = con.cursor()

    if verbose:
        print "Connected to sqlite"

    state = 'UNKNOWN'
    runFlag = True

    r_server = redis.Redis( redisHost )

    while runFlag:
        event = r_server.blpop("event")
    
        print event[1]
    
        if event[1] == "PF" and state != "PF":

            print "Executing power fail action."

            con = sqlite.connect( db )
            cur = con.cursor()
            sql="""update outlets 
            set requested_state='OFF' 
            where name IN 
            (select name from outlets where pf_action = 'OFF');"""
    
            cur.execute( sql )
            con.commit()
            con.close()
            state = 'PF'



main()

