#!/usr/bin/python3

# Needs PySimpleGUI

import PySimpleGUI as sg
import os.path
import json
import pymysql as sql


from sys import exit

def main():
    database = 'localhost'
    mqttBroker = 'localhost'
    mqttPort = 1883

    configFile="/etc/mqtt/bridge.json"

    if os.path.exists(configFile):
        with open( configFile, 'r') as f:
            cfg = json.load(f)

        database = cfg['database']['name']
        db       = cfg['database']['db']
        user     = cfg['database']['user']
        passwd   = cfg['database']['passwd']
    else:
        print(configFile + " not found..")
        print(".. using defaults")

    db = sql.connect(database, user, passwd,db)
    cursor = db.cursor()

    print('Startup up...')
    # Very basic window.  Return values as a list      
    
    sg.ChangeLookAndFeel('Dark')
    layout = [      
            [sg.Text('Create IO Point')],      
            [sg.Text('Name', size=(10, 1)), sg.InputText()],
            [sg.Text('MQTT Topic', size=(10, 1)), sg.InputText()],
            [sg.Text('Direction', size=(10, 1))],
            [sg.InputCombo(('IN', 'OUT','INTERNAL','DISABLED'), size=(20, 5))],
            [sg.Text('Type', size=(10, 1))],
            [sg.InputCombo(('MQTT', 'INTERNAL'), size=(20, 5))],
            [sg.Text('Data Type', size=(10, 1))],
            [sg.InputCombo(('BOOL', 'STRING'), size=(20, 5))],
            [sg.Submit(), sg.Cancel()]      
            ]      
    window = sg.Window('Simple data entry window', layout)
    
    runFlag = True
    while runFlag:
        event, values = window.Read()
    
        if event == 'Cancel':
            runFlag = False
        else:
            name      = values[0]
            topic     = values[1]
            direction = values[2]
            ioType    = values[3]
            dataType  = values[4]
        
        # Need a name
            if name == '':
                runFlag = True
            else:
                if topic != '' and ioType == 'MQTT':
                    runFlag = False
                elif topic == '' and ioType == 'INTERNAL':
                    runFlag = False
                else:
                    runFlag = True
                    sg.Popup('Error\nIf INTERNAL no need to define a topic')
        
            print(event)
    
    print(event, values[0], values[1], values[2])  
    
    window.Close()

    if event == 'Cancel':
        exit(0)
    
    if ioType == 'MQTT':
#        sqlCmd = "replace into " + ioType.lower() + "(name, topic ) values ('" + name + "','"+topic+ "');"
        sqlCmd = "replace into " + ioType.lower() + "(name, topic, data_type ) values ('" + name + "','"+topic+ "','" + dataType + "');"
        print(sqlCmd)
        cursor.execute( sqlCmd )
    elif ioType == 'INTERNAL':
        sqlCmd = "replace into " + ioType.lower() + "(name) values ('" + name + "');"
        print(sqlCmd)
        cursor.execute( sqlCmd )
    
    sqlCmd = "replace into io_point (name,direction,io_type ) values ('" + name + "','"+ direction +"','" + ioType + "');"
    
    print(sqlCmd)
    cursor.execute( sqlCmd )

    sqlCmd = "update io_point," + ioType.lower() +  " set io_point.io_idx=" + ioType.lower() + ".idx where io_point.name = " + ioType.lower() + ".name;"
    print(sqlCmd)
    cursor.execute( sqlCmd )

    db.commit()
    cursor.close()
    db.close()



main()
