#!/usr/bin/env python3

import socket
import sys
import os.path
import json
import time
import getopt

def usage(name):
    print("Usage: setPower.py" + name)


def main():
    verbose=False
    data=None

    HOST, PORT = "localhost", 9191
    configFile="/etc/mqtt/bridge.json"

    dataName = ""
    dataValue=""

    try:
        opts,args = getopt.getopt(sys.argv[1:],"c:d:hn:v", ["config=","def=","help","name=","verbose"])

        for o,a in opts:
            if o in ["-h","--help" ]:
                usage()
                sys.exit()
            elif o in ["-c", "--config"]:
                configFile = a 
            elif o in ["-d","--def"]:
                dataValue=a
            elif o in ["-n","--name"]:
                dataName = a 
            elif o in ["-v","--verbose"]:
                verbose=True

    except getopt.GetoptError as err:
        print(err)
        usage()
        sys.exit(2)

    if dataName=="" and dataValue=="":
        usage()
        sys.exit()

    cmd = "SET " + dataName + " " + dataValue

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
    
main()

