#!/usr/bin/env python3

import configparser as cp
import getopt
import sys
import redis

def usage():
    print()
    print("Usage:./dBridge.py -h|-c <cfg> -l|-r -v ")

    print("\t-c <cfg>\tConfig file.")
    print("\t-h\t\tHelp.")
    print("\t-l\t\tLocal.")
    print("\t-r\t\tRemote.")
    print("Note: Default is:")
    print("\t./dBridge.py -l ")
    print()

def main():

    localBroker  = True
    remoteBroker = False
    verbose = False
#    configFile="/etc/mqtt/bridge.ini"
    configFile="./bridge.ini"

    try:
        opts, args = getopt.getopt(sys.argv[1:], "c:hlrv", ["config=","help","local","remote","verbose"])

        for o,a in opts:
            if o in ["-h","--help"]:
                usage()
                sys.exit()
            elif o in ["-l","--local"]:
                localBroker=True
                remoteBroker=False
            elif o in ["-r","--remote"]:
                localBroker=False
                remoteBroker=True
            elif o in ["-v","--verbose"]:
                verbose=True
            elif o in ["-c","--config"]:
                configFile = a

    except getopt.GetoptError as err:
        print(err)
        sys.exit(2)

    if verbose:
        print()
        if localBroker:
            print("Local broker")
        if remoteBroker:
            print("Remote broker")

        print("Config File:"+configFile)
        print()

    cfg = cp.ConfigParser()
    cfg.read( configFile )

    redisHost="fred"
    redisPort = 6379

    redisHost = cfg.get('common','redis-host')
    redisPort = cfg.get('common','redis-port')

    print( redisHost )
    print( redisPort )

    rc = redis.StrictRedis(redisHost, port=redisPort, db=0)

    for k in rc.scan_iter("/*"):
        print(k.decode(), (rc.get(k)).decode())

    return

main()

