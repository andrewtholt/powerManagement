#!/usr/bin/python

import sys

from powerClass import powerClass as power

def main():
    if len(sys.argv) == 1:
        host='all'
    else:
        host=sys.argv[1]

    print host
    control=power()
    control.cancel( host )

main()


