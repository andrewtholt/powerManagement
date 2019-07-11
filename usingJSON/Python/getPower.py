#!/usr/bin/env python3

import socket
import sys
import os.path
import json
import time
import getopt

def usage(name):
    print("Usage: " + name + "  -c <file>|--config <file> -n <name>|--name <name> -v|--verbose")
    print("\t-c <file>\tConfig file, default is /etc/mqtt/bridge.json")
    print("\t-n <name>\tThe name of tha data to read.")
    print("\t-v\t\tVerbose.")

def main():
    verbose=False
    data=None

    portSet=False
    hostSet=False

    HOST, PORT = "localhost", 9191
    configFile="/etc/mqtt/bridge.json"

    try:
        opts,args = getopt.getopt(sys.argv[1:],"c:hn:v", ["config=","help","name=","verbose"])

        for o,a in opts:
            if o in ["-h","--help" ]:
                usage(sys.argv[0])
                sys.exit()
            elif o in ["-c", "--config"]:
                configFile = a
            elif o in ["-n","--name"]:
                data = "GET " + a
            elif o in ["-v","--verbose"]:
                verbose=True

    except getopt.GetoptError as err:
        print(err)
        usage(sys.argv[0])
        sys.exit(2)

#    if len(sys.argv) != 2:
#        print("Usage: getPower.py <name>")
#        exit(1)
#
#    data = "GET " + sys.argv[1] + "\n"

    if verbose:
        print("Config file:" + configFile)
        print("Data name  :" + data)

    if os.path.exists(configFile):
        with open( configFile, 'r') as f:
            cfg = json.load(f)
        HOST = cfg['socket']['name']
        PORT = int(cfg['socket']['port'])
    else:
         print(configFile + " not found..")
         print(".. using defaults")


    if verbose:
        print("Host    : " + HOST)
        print("Port    : " , PORT)
        print("Command : " + data)


    # Create a socket (SOCK_STREAM means a TCP socket)
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        # Connect to server and send data
        sock.connect((HOST, PORT))
        sock.sendall(bytes(data + "\n", "utf-8"))
    
        # Receive data from the server and shut down
        received = str(sock.recv(1024), "utf-8")
    if verbose:
        print("Sent:     {}".format(data))
        
    print("{}".format(received),end="")

main()

