#!/usr/bin/env python3

import socket
import sys
import os.path
import json
import time
import getopt

def usage(name):
    print("Usage: " + name)

def main():
    verbose=True
    jsonOut = False
    cmd="TOGGLE "
    dataName=""
    dataValue=""

    HOST, PORT = "localhost", 9191
    configFile="/etc/mqtt/bridge.json"

    try:
        opts,args = getopt.getopt(sys.argv[1:],"c:hn:vj", ["config=","help","name=","verbose","json"])

        for o,a in opts:
            if o in ["-h","--help" ]:
                usage(sys.argv[0])
                sys.exit()
            elif o in ["-c", "--config"]:
                configFile = a 
            elif o in ["-n","--name"]:
                dataName = a 
                cmd += dataName
            elif o in ["-v","--verbose"]:
                verbose=True
            elif o in ["-j","--json"]:
                jsonOut=True

    except getopt.GetoptError as err:
        print(err)
        usage(sys.argv[0])
        sys.exit(2)

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
        print("Command : " + cmd)


    # Create a socket (SOCK_STREAM means a TCP socket)
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        # Connect to server and send data
        sock.connect((HOST, PORT))
        sock.sendall(bytes(cmd + "\n", "utf-8"))

        dataValue = str(sock.recv(1024), "utf-8")

        dataValue = dataValue.strip()

    if jsonOut:
        print('{ "name":"' + dataName + '","value":"' + dataValue + '" }')
    else:
        print("{}".format(dataValue),end="")


    
main()

