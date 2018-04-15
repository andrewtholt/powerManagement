#!/usr/bin/env python3

import socket
import threading
import msgParser
import getopt
import sys

verbose=False
serverCount=0

class ThreadedServer(object):
    def __init__(self, host, port):
        self.host = host
        self.port = port
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.sock.bind((self.host, self.port))

    def listen(self):
        global verbose
        self.sock.listen(5)
        while True:
            client, address = self.sock.accept()
            client.settimeout(60)
            threading.Thread(target = self.listenToClient,args = (client,address)).start()

    def listenToClient(self, client, address):
        global verbose
        size = 1024
        runFlag=True
        print("Connected:",address)
        parser=msgParser.msgParser()
        parser.setVerbose(True)

        while runFlag:
            try:
                data = client.recv(size)
                if data:
                    # Set the response to echo back the recieved data 
                    response = data

                    message=data.decode('utf-8')
                    if verbose:
                        print("rx:" + message)

                    rc=parser.parseMsg( message )

                    if parser.getEcho():
                        client.send(response)
                else:
                    raise error('Client disconnected')

            except:
                print("listenToClient exception")
                client.close()
#                return False
                runFlag=False
        print("disconnected:",address)

def usage(name):
    print("Usage: " + name )

def main():
    global verbose
    port_num = 9090
    host = 'localhost'
    runFlag=True

    try:
        opts, args = getopt.getopt(sys.argv[1:],"i:hp:v",["ip=","help","port=","verbose"])
    except:
        usage(sys.argv[0])
        sys.exit(2)

    for o,a in opts:
        if o in ("-h","--help"):
            usage(sys.argv[0])
            sys.exit(0)
        elif o in("-i","ip="):
            host=a
        elif o in("-p","port"):
            port_num=int(a)
        elif o in("-v","--verbose"):
            print("Verbose")
            verbose = True

    if verbose:
        print("Address : " + host)
        print("Port    : " , port_num)

    while runFlag:
#        port_num = input("Port? ")
#        try:
#            port_num = int(port_num)
#            break
#        except ValueError:
#            pass

#        ThreadedServer('',port_num).listen()
        ThreadedServer(host,port_num).listen()


main()





