#!/usr/bin/env python3

import configparser as cp

def main():
    cfg = cp.ConfigParser()
    cfg.sections()
    cfg.read('/etc/mqtt/bridge.ini')
    
    try:
        execPath = cfg.get('local','exec-path',fallback='/usr/local/bin')
        execScript = cfg.get('local','exec-script',fallback='noAction.py')
    
        print(execPath)
        print(execScript)
    except:
        print("Ooops")


main()

    
