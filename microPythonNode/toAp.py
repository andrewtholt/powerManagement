#!/usr/bin/env python3

import json
import sys
import os.path

def main():
    if len(sys.argv) !=2 :
        print("Usage : " + sys.argv[0] + " <ap name>")
        sys.exit(1);
    
    configFile = './ap.json'
    apName = sys.argv[1]
    
    if os.path.exists(configFile):
        with open(configFile,'r') as f:
            cfg = json.load(f)
    
        if apName in cfg:
            tst={}
            tst[apName] = cfg[apName]

#            print(tst)
#            print("accessPoint =",cfg[apName])
            print("accessPoint =",tst)
        else:
            print('No access point named "' + apName + '"')


main()


