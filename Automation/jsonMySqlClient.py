#!/usr/bin/env python3
import sys 
import pymysql as mysql
import getopt
import os.path
import socket

from os import getenv

verbose=False

def usage():
    print("mySqlClient.py .....")

def main():
    
    global verbose
    serverAddress = 'localhost'
    serverPort = 10010

    try:
        opts,args = getopt.getopt(sys.argv[1:], "hp:v",["help","port=","verbose"])
    except:
        print("Argument error")
        usage()
        sys.exit(2)

    for o,a in opts:
        if o in ("-v","--verbose"):
            verbose=True
            print("Verbose")
        elif o in ("-h","--help"):
            usage()
            sys.exit(0)
        elif o in ("-p","--port"):
            serverPort = int(a)

    try:
        con = mysql.connect("localhost", "automation","automation","automation")
        cur = con.cursor()

        sql = "use automation"
        if verbose:
            print("sql:" + sql)
        cur.execute( sql )
    except:
        print("\nProblem with database")
        sys.exit(1)

    # Create a TCP/IP socket
    # 
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    # Bind the socket to the port
    # 
    server = (serverAddress, serverPort)

    if verbose:
        print('starting up on {} port {}'.format(*serverAddress))

    sock.bind(server)
    sock.listen(1)

    while True:
        if verbose:
            print('waiting for a connection')

        connection, clientAddress = sock.accept()

        if verbose:
            print('connection from', clientAddress)

        try:
            while True:
                data = connection.recv(255)
                if data:
                    sql = data.decode('utf-8')
                    if verbose:
                        print("rx: ",sql)

                    cur.execute(sql)

                    fLen = len(cur.description)
                    print("Number of fields ",fLen)
                    field_names = [i[0] for i in cur.description]

#                    print(field_names)
                    print("{ ", end="")
                    for point in cur.fetchall():
                        for i in range(0,len(cur.description)):
#                            print(cur.description[i],point[i])

                            if i < (fLen-1):
                                print('"' + cur.description[i][0] + '":"' + point[i] + '",',end="")
                            else:
                                print('"' + cur.description[i][0] + '":"' + point[i] + '"',end="")
                    print(" }")
                else:
                    break

        finally:
            connection.close()


main()

