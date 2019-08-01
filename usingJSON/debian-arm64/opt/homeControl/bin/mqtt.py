#!/usr/bin/env python3
import sys
import posix_ipc
import json
import os
import getopt

def usage(name):
    print("Usage: " + name + " -v|--verbose -c|--config <cfg> -h|--help")

    print("Deal with MQTT messages")
    print("")
    print("\t-h\t\tHelp.")
    print("\t-v\t\tVerbose.")

    print("\t-c <cfg>\tUse config file.  Default is /etc/mqtt/bridge.json")

def main():
    configFile="/etc/mqtt/bridge.json"
    verbose=False

    BROKER = "127.0.0.1"
    BROKER_PORT = "1883"

    try:
        opts,args = getopt.getopt(sys.argv[1:], "vc:h", ["verbose=","config=","help"])
        for o,a in opts:
            if o in ["-h","--help"]:
                usage(sys.argv[0])
                sys.exit()
            elif o in ["-v","--verbose"]:
                verbose = True
            elif o in ["-c","--config"]:
                configFile = a

    except getopt.GetoptError as err:
        print(err)
        usage()
        sys.exit(2)

    if os.path.exists(configFile):
        with open( configFile, 'r') as f:
            cfg = json.load(f)

        BROKER = cfg['local']['name']
        BROKER_PORT = cfg['local']['port']
    else:
         print(configFile + " not found..")
         print(".. using defaults")

    if verbose:
        print("MQTT Broker : " + BROKER)
        print("     Port   : " + BROKER_PORT)

    mq = posix_ipc.MessageQueue("/toMQTT")
    
    print(mq)
    
    while True:
        s, _ = mq.receive()
        s = s.decode()
        
        if verbose:
            print(s)
        
        data = json.loads(s)
        
        topic = data['topic']
        state = data['state']
       
        if verbose:
            print('topic: ' + topic)
            print('state: ' + state)
    
        cmd = 'mosquitto_pub -h ' + BROKER + ' -t ' +  topic + ' -m ' + state
    
        if verbose:
            print(cmd)
    
        os.system(cmd)

main()
