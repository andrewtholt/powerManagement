#!/usr/bin/env python3

import socketserver
import getopt
import sys

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
        # just send back the same data, but upper-cased
                self.request.sendall(self.data.upper())
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
    with socketserver.TCPServer((HOST, PORT), MyTCPHandler) as server:
        # Activate the server; this will keep running until you
        # interrupt the program with Ctrl-C
        server.serve_forever()
