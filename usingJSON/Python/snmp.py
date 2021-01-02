#!/usr/bin/env python3

import sys
import posix_ipc
import json
import os
import getopt

def usage(name):
    print("Usage: " + name + " -v|--verbose -c|--config <cfg> -h|--help")

    print("Deal with messages to SNMP")
    print("")
    print("\t-h\t\tHelp.")
    print("\t-v\t\tVerbose.")


def main():
    verbose=False

    try:
        opts,args = getopt.getopt(sys.argv[1:], "vc:h", ["verbose=","config=","help"])
        for o,a in opts:
            if o in ["-h","--help"]:
                usage(sys.argv[0])
                sys.exit()
            elif o in ["-v","--verbose"]:
                verbose = True

    except getopt.GetoptError as err:
        print(err)
        usage()
        sys.exit(2)

    
    mq = posix_ipc.MessageQueue("/toSNMP")
    
    while True:
        s, _ = mq.receive()
        s = s.decode()
        
        if verbose:
            print(s)
        
        data = json.loads(s)
        
        print( data['oid'])
        print( data['rw_community'])
        print( data['state'])
        print( data['ipAddress'])
    
        oid = data['oid']
        dbState = data['state']

        if dbState == 'ON':
            state = data['on_value']
        elif dbState == 'OFF':
            state = data['off_value']


        ip = data['ipAddress']
        community = data['rw_community']
        
        cmd='snmpset -m ALL -v1 -c ' + community + ' ' + ip + ' ' + oid +'  i ' + state
        
        if verbose:
            print(cmd)
        
        os.system(cmd)


main()

