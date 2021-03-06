#!/usr/bin/env python3

import socketserver, subprocess
from threading import Thread
import getopt
import sys
import msgParser

verbose = False
serverCount=0

def usage(name):
    print("Usage: " + name )

class MyTCPHandler(socketserver.BaseRequestHandler):
    """
    The request handler class for our server.

    It is instantiated once per connection to the server, and must
    override the handle() method to implement communication to the
    client.
    """

    def handle(self):
        runFlag=True
        global serverCount
        global verbose
        
        parser=msgParser.msgParser()
        parser.setVerbose(True)

        if verbose:
            print("Client address:",self.client_address)
            print(self.request)
        serverCount += 1
        while runFlag:
        # self.request is the TCP socket connected to the client
            try:
                self.data = self.request.recv(1024).strip()
                if not self.data:
                    runFlag=False
                else:
                    print("{} wrote:".format(self.client_address[0]))
                    print(self.data)
                    rc=parser.parseMsg(self.data.decode('utf-8'))
                    print(rc)
            except:
                runFlag=False
                print("Ooops")
        serverCount -= 1

if __name__ == "__main__":
    HOST, PORT = "localhost", 9999

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
            HOST=a
        elif o in("-p","port"):
            PORT=int(a)
        elif o in("-v","--verbose"):
            print("Verbose")
            verbose = True

    if verbose:
        print("Address : " + HOST)
        print("Port    :" , PORT)

    # Create the server, binding to localhost on port 9999
    daemon_threads=True
    allow_reuse_address=True
    with socketserver.TCPServer((HOST, PORT), MyTCPHandler) as server:
        # Activate the server; this will keep running until you
        # interrupt the program with Ctrl-C
        server.serve_forever()
