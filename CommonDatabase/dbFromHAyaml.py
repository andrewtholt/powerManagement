#!/usr/bin/python3

import yaml

verbose = False

def printVerbose( str ):
    if verbose:
        print(str)

with open("/home/andrewh/Data/HomeAssistant/c1.yaml", 'r') as f:

    doc = yaml.safe_load(f)
# To access branch1 text you would use:

    printVerbose("===== sensor")
    txt = doc["sensor"]

    for n in txt:
        try:
            printVerbose(n['name'])
        except:
            pass

    printVerbose("===== binary sensor")
    txt = doc["binary_sensor"]

    for n in txt:
        try:
            printVerbose(n['name'])
        except:
            pass

    printVerbose("===== switch")
    txt = doc["switch"]

    for n in txt:
        try:
            if(n['platform']) == 'mqtt':
                name = ('switch.' + n['name'].replace(" ","_")).lower()
                sql = "REPLACE INTO io_point (name,direction, ioType) values("
                sql += '"' + name + '",'
                sql += '"OUT",'
                sql += "'MQTT');"
                print(sql);
                printVerbose("Name     :" +  name)

                printVerbose('Cmd   topic:' + n['command_topic'])
                printVerbose('State topic:' + n['state_topic'])

                sql = 'REPLACE INTO mqtt '
                sql += '(name, command_topic, state_topic,availability_topic) VALUES '

                sql += "("
                sql += "'" + name + "',"
                sql += "'" + n['command_topic'] + "',"
                sql += "'" + n['state_topic'] + "',"
                sql += "'" + n['availability_topic'] + "');"

                print(sql)
                printVerbose('=========')
        except:
            printVerbose('--> error')
