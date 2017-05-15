#!/usr/bin/python3

import getopt
import sys

def usage():
    print("Usage: scan.py -h|-t <topic> -m <msg>")

def main():

    topic = ""
    msg = ""
    verbose = True
    host = "127.0.0.1"

    try:
        opts, args = getopt.getopt(sys.argv[1:], "ht:m:",["help","topic","message"])
    except getopt.GetoptError as err:
        print(err)
        sys.exit(2)

    for o,a in opts:
        if o in ("-h","--help"):
            usage()
            sys.exit(0)
        elif o in ("-t","--topic"):
            topic = a
        elif o in ("-m","--message"):
            msg = a

    if topic == "" or msg == "":
        usage()
        sys.exit(1)

    if verbose:
        print("Topic:" + topic)
        print("Msg  :" + msg)

    for t in topic.split():
        print(t)

main()
