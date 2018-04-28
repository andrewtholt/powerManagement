#!/usr/bin/env python3

# import msgParser
from msgParser import msgParser, databaseType

import getopt 
import sys

def usage():
    print("Usage: interp.py -v -h -c <file name>")
    print("\t-v|--verbose\tVerbose.")
    print("\t-h|--help\tHelp")
    print("\t-c <file>|--commands=<file> Read commands from given file.\n")

    print("Commands")
    print("\tNOTE: All commands are prefixed with a ^")
    print()
    print("\thelp\t\t\tThis.")
    print("\tprint\t\t\tDump settings to stdout.")
    print("\tconnect\t\t\tConnect to the database, using the defined settings.")
    print("\tget-row\t\t\tGet the current row resulting from a previous query.")
    print("\tgo-first\t\t\tGo to the first row in the results of a previous query.")
    print("\tgo-last\t\t\tGo to the last row in the results of a previous query.")
    print("\tgo-prev\t\t\tGo to the previous row.")
    print("\tgo-next\t\t\tGo to the previous row.")

    print("\tget <name>\t\tGet the value of a setting.")
    print("\tget-col <name>\t\tGet the column of a result row by name.")
    print("\tload <file name>\tExecute commands from the named file.")
    print("\tset-database <db type>\tSet the database to be used currently MYSQL or SQLITE.")

    print("\tset <name> <value>\tSet the parameter <name> to <value>.")
    print()



def main():
    tst = msgParser()

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
    # 
    # Move this msgParser.
    #
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
    tst.output("DISCONNECTED")

main()
