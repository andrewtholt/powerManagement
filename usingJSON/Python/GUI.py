#!/usr/bin/python3

# Needs PySimpleGUI

import PySimpleGUI as sg

print('Startup up...')
# Very basic window.  Return values as a list      

layout = [      
        [sg.Text('Create IO Point')],      
        [sg.Text('Name', size=(10, 1)), sg.InputText()],
        [sg.Text('MQTT Topic', size=(10, 1)), sg.InputText()],
        [sg.Text('Direction', size=(10, 1))],
        [sg.InputCombo(('IN', 'OUT','INTERNAL','DISABLED'), size=(20, 5))],
        [sg.Text('Type', size=(10, 1))],
        [sg.InputCombo(('MQTT', 'INTERNAL'), size=(20, 5))],
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
    
        print(event)

print(event, values[0], values[1], values[2])  

window.Close()

if ioType == 'MQTT':
    sqlCmd = "replace into " + ioType.lower() + "(name, topic ) values ('" + name + "','"+topic+ "');"
    print(sqlCmd)
elif ioType == 'INTERNAL':
    sqlCmd = "replace into " + ioType.lower() + "(name) values ('" + name + "');"
    print(sqlCmd)

sqlCmd = "replace into io_point (name,direction,io_type ) values ('" + name + "','"+ direction +"','" + ioType.lower() + "');"

print(sqlCmd)
sqlCmd = "update io_point," + ioType.lower() +  " set io_point.io_idx=" + ioType.lower() + ".idx where io_point.name = " + ioType.lower() + ".name;"
print(sqlCmd)
