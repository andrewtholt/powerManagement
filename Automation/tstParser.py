#!/usr/bin/env python3

import msgParser
def main():
    tst = msgParser.msgParser()
    tst.setVerbose(True)

    tst.parseMsg("  ^help  ")

    out=tst.parseMsg('^set host 192.168.10.118')
    out=tst.parseMsg('^set database automation')
    out=tst.parseMsg('^set user automation')
    out=tst.parseMsg('^set password automation')

    print("======")
    print(out)
    print("======")

    out=tst.parseMsg('^get host')
    print("======")
    print(out)
    print("======")

    tst.parseMsg("^print")
    tst.parseMsg("^connect")

main()
