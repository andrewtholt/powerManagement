#!/usr/bin/env python3

import socket
import sys
import getopt

from Sun import Sun
import time


def usage():
    print("Usage:echoServer.py -i <address> -p <port> ")
    print("\t-i <address>\tIP address or name of host this is running on.")
    print("\t-p <port>\tPort that this listens on. ");
    print("\nDefault behaviour is -i localhost -p 10000\n")

def main():

    serverAddress = 'localhost'
    serverPort = 10000
    try:
        opts, args = getopt.getopt(sys.argv[1:], "i:p:vh", ["ip=","port=","verbose","help"])

        for o,a in opts:
            if o in ["-h","--help"]:
                usage()
                sys.exit(0);
            elif o in ["-i","--ip"]:
                serverAddress = a
            elif o in ["-p","--port"]:
                serverPort = int(a)
    except getopt.GetoptError as err:
        print(err)
        usage()
        sys.exit(2)

    sun = Sun()
    coords = {'longitude' : -2.6, 'latitude' : 53.6 }


    # Create a TCP/IP socket
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    
    # Bind the socket to the port
    server = (serverAddress, serverPort)

    print ("Start on " + serverAddress + " port %d" % serverPort )
#    print('starting up on {} port {}'.format(*serverAddress))
    sock.bind(server)
    
    # Listen for incoming connections
    sock.listen(1)
    
    while True:
        # Wait for a connection
        print('waiting for a connection')
        connection, clientAddress = sock.accept()
        try:
            print('connection from', clientAddress)
    
            hoursSr=int(sun.getSunriseTime( coords )['hr'])
            minsSr=int(sun.getSunriseTime( coords )['min'])

            hoursSs=int(sun.getSunsetTime( coords )['hr'])
            minsSs=int(sun.getSunsetTime( coords )['min'])

            sr = sun.getSunriseTime( coords )['decimal']
            ss = sun.getSunsetTime( coords )['decimal']

            out = '{ "sunrise": "%d:%d","sunset": "%d:%d" }\n' % ( hoursSr,minsSr, hoursSs, minsSs)
#            out = '{ "sunrise": "%s","sunset": "%s" }\n' % ( sun.toHMS(sr), sun.toHMS(ss))
            connection.sendall(bytes(out,'utf-8'))
        finally:
            # Clean up the connection
            connection.close()

main()

