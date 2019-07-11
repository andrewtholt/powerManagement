#!/usr/bin/env python3

import socket
import sys
import os.path
import json
import time
import getopt

def usage(name):
    print("Usage:" + name + " -c <file>|--config <file> -n <name>|--name <name> -d <value>|--def <value> -v|--verbose")
    print("\t-c <file>\tConfig file, default is /etc/mqtt/bridge.json")
    print("\t-n <name>\tThe name of tha data to read.")
    print("\t-d <value>\tThe value to set.")
    print("\t-v\t\tVerbose.")

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
                usage(sys.argv[0])
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
        usage(sys.argv[0])
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

