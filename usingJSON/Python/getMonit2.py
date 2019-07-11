#!/usr/bin/env python3

import urllib.parse
import urllib.request

import pprint
import json

import xml.etree.cElementTree as ET

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

from copy import copy

def dictify(r,root=True):
    if root:
        return {r.tag : dictify(r, False)}
    d=copy(r.attrib)
    if r.text:
        d["_text"]=r.text
    for x in r.findall("./*"):
        if x.tag not in d:
            d[x.tag]=[]
        d[x.tag].append(dictify(x,False))
    return d

tst = (dictify(root))

# pprint.pprint(tst)
print(json.dumps(tst))

# print( tst['monit']['platform'][0]['name'])


