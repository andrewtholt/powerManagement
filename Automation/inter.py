#!/usr/bin/env python3

import msgParser
import getopt 
import sys

def usage():
    print("Usage:")

def main():
    tst = msgParser.msgParser()

    runFlag=True
    cmdFile=None
    verbose=False

    try:
        opts,args = getopt.getopt(sys.argv[1:],"vhc:",["verbose","help","cmds="])
    except:
        print("Argument error")
        usage()
        sys.exit(2)

    for o,a in opts:
        if o in ("-h","--help"):
            usage()
            sys.exit(0)
        elif o in ("-c","--cmds"):
            cmdFile=a
        elif o in ("-v","--verbose"):
            verbose = True

    if cmdFile != None:
        count=0
        with open(cmdFile) as fp:
            line=fp.readline()
            while line:
                count+=1
                rc=tst.parseMsg(line)
                if verbose:
                    print(count,":" + line)
                line=fp.readline()

    while runFlag:
        cmd = input("cmd>")
        rc=tst.parseMsg(cmd)

        runFlag=not rc[0]

main()
