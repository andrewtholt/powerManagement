#!/usr/bin/env python3

import urllib.parse
import urllib.request
import xml.dom.minidom

def getText(nodelist):
    rc = ""
    for node in nodelist:
        if node.nodeType == node.TEXT_NODE:
            rc = rc + node.data
    return rc

password_mgr = urllib.request.HTTPPasswordMgrWithDefaultRealm()

top_level_url = 'http://127.0.0.1:2812/'

username="admin"
password="monit"

password_mgr.add_password("monit", top_level_url, username, password)
handler = urllib.request.HTTPBasicAuthHandler(password_mgr)
opener = urllib.request.build_opener(handler)


response = opener.open('http://127.0.0.1:2812/_status?format=xml')
xmlData = response.read()

dom = xml.dom.minidom.parseString(xmlData)

pretty_xml_as_string = dom.toprettyxml()

# print( pretty_xml_as_string )

tst = dom.getElementsByTagName("platform")
# tst1 = tst.getElementsByTagName("name")

print(tst.tagName)

    

print(tst)
