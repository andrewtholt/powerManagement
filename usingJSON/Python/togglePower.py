#!/usr/bin/env python3

import socket
import sys
import os.path
import json
import time

def usage(name):
    print("Usage: " + name)

def main():
    verbose=True
    data=None

    HOST, PORT = "localhost", 9191
    configFile="/etc/mqtt/bridge.json"

    if len(sys.argv) != 2:
        print("Usage: togglePower.py <name>")
        exit(1)

    data = "TOGGLE " + sys.argv[1] + "\n"

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
    
main()

