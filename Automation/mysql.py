#!/usr/bin/env python3

import pymysql as sql

def main():
    db = sql.connect("192.168.10.65", "automation","automation","automation")

    cursor = db.cursor()
    cursor.execute("select name,direction,topic,state from mqttQuery where direction = 'OUT';")
#    cursor.execute("SELECT VERSION()")
#    cursor.execute("SHOW TABLES")

    data = cursor.fetchall()

    print( data )
#    print ("Database version : %s " % data)

    # disconnect from server
    db.close()


main()
