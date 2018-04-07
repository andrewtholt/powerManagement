#!/usr/bin/env python3

import pymysql as sql

def main():
    db = sql.connect("localhost", "automation","automation","automation")

    cursor = db.cursor()
#    cursor.execute("SELECT VERSION()")
    cursor.execute("SHOW TABLES")

    data = cursor.fetchone()
    print ("Database version : %s " % data)

    # disconnect from server
    db.close()


main()
