#!/usr/bin/env python3

import urllib.parse
import urllib.request

import xml.etree.ElementTree as ET

password_mgr = urllib.request.HTTPPasswordMgrWithDefaultRealm()

top_level_url = 'http://127.0.0.1:2812/'

username="admin"
password="monit"

password_mgr.add_password("monit", top_level_url, username, password)
handler = urllib.request.HTTPBasicAuthHandler(password_mgr)
opener = urllib.request.build_opener(handler)


response = opener.open('http://127.0.0.1:2812/_status?format=xml')
xmlData = response.read()

root = ET.fromstring(xmlData)

tst = root[2]
# tst = root

print(tst[0].text)

count = 0
for i in tst:
    print("count ",count)

    print(">" + i.tag + "=" , i.text )
    count = count + 1
    if i.tag == "system":
        for j in i:
            print("\t>" + j.tag + "=" , j.text )
            print("\t\t>" + j[0].tag + "=", j[0].text)
            print("\t\t>" + j[1].tag + "=", j[1].text)

    print("=====")

