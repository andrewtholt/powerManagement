#!/usr/bin/env python3

import sys
from pushbullet import Pushbullet
import getopt
import notify2

def usage():
    print("Usage: pbNotify.py -h|--help -t <text>|--title=<text> -b <text>|-bodytext=<text>")

def main():
    
    try:
        opts, args = getopt.getopt(sys.argv[1:], "ht:b:", ["help","title=","body="])

        for o,a in opts:
            if o in ["-h","--help"]:
                usage()
                sys.exit(0)
            elif o in ["-t","--title"]:
                title = a
            elif o in ["-b","--body"]:
                body = a

    except getopt.GetoptError as err:
        print(err)
        usage()
        sys.exit(2)

    notify2.init(title)
    n = notify2.Notification(title, body)

    n.show()

main()


