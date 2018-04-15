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

                    try:
                        con = mysql.connect("localhost", "automation","automation","automation")
                    except:
                        print("\nProblem with database")
                        sys.exit(1)

                    sql = data.decode('utf-8')

                    try:
                        print("Open")
                        cur = con.cursor()

                        use = "use automation"
                        if verbose:
                            print("sql:" + use)
                            cur.execute( use )

                        if verbose:
                            print("rx: ",sql)
                        cur.execute(sql)
    
                        if cur.description == None:
                            con.commit()
                            tmpString = "^ok\n"
                            connection.sendall( bytes(tmpString,'utf-8') )
                        else:
                            fLen = len(cur.description)
    
                            if verbose:
                                print("Number of fields ",fLen)
                                print("Number of rows   ",cur.rowcount)
        
                            tmpString = "^rows=" + str(cur.rowcount)+"\n"
                            connection.sendall( bytes(tmpString,'utf-8' ))
        
                            field_names = [i[0] for i in cur.description]
        
                            seperator=","
        
                            tmpString = "^columns=" + seperator.join(field_names) + '\n'
                            if verbose:
                                print(tmpString)
        
                            connection.sendall( bytes(tmpString,'utf-8') )
                            for point in cur.fetchall():
                                if verbose:
                                    print(point)
                                tmpString = "(" + seperator.join(point) + ")\n"
                                connection.sendall( bytes(tmpString,'utf-8') )

                    except Exception :
                        print(sys.exc_info()[0])
                        tmpString="^error\n"
                        connection.sendall( bytes(tmpString,'utf-8') )
                    finally:
                        print("Close")
                        cur.close()
                        con.close()

                else:
                    break

        finally:
            connection.close()


main()

