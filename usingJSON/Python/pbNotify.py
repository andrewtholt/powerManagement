#!/usr/bin/env python3

from pushbullet import Pushbullet

def main():
    print("Hello")

    token=""
    tokenFile = open("token.txt","r")

    if tokenFile.mode =="r":
        token=(tokenFile.read()).rstrip("\n\r")

    print(">" + token + "<")

    tokenFile.close()

    pb = Pushbullet(token)

    push = pb.push_note("This is the title", "This is the body")

main()


