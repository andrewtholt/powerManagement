#!/usr/bin/env python3
verbose=False
child=None
exitFlag=False

outlets={ '/home/office/proliant/power': '3' }

import sys
import signal
import time

import pexpect
import getopt
import paho.mqtt.client as mqtt
import configparser as cp

from os import getenv

def handler(signum, frame):
    global exitFlag
    print('Signal handler called with signal', signum)

    if signum == 1:
        print("Exit from pm8")
        child.sendline('')
        child.expect('pm>')
        child.sendline('exit')
        time.sleep(2)
        exitFlag=True
        sys.exit(0)



def usage():
    print()
    print("Usage:pm8.py -v -n <name> -p <port> -h|-c <cfg> ")
#        opts, args = getopt.getopt(sys.argv[1:], "c:hvn:p:", ["config=","help","verbose","host","port"])

def on_connect(client, userdata, flags, rc):
    global verbose
    global exitFlag

    if exitFlag:
        sys.exit(0)

    if verbose:
        print("Connected")

    for k,v in outlets.items():
#        print(k)
        client.subscribe( k )

#    client.subscribe("/home/office/proliant/power")

def on_message(client, userdata, msg):
    global verbose
    global outlets

    m=(msg.payload).decode()
    t=msg.topic

    if verbose:
        print("Message")
        print("Topic : " + t )
        print("Msg   : " + m )
        print("=======")


    out = str(outlets[ t ])

    pm8Switch( out , m )


def pm8Switch( out, state ):
    global verbose

    cmd = state.lower() + ' ' + str(out)

    if verbose:
        print( cmd )
    
    child.sendline( cmd )
#    child.expect('pm8')



def pm8Connect( host, port):
    global child;
    cmd = 'nc ' + host + ' ' + str(port)

    child = pexpect.spawn(cmd)
    
    child.sendline('')
    
    i=child.expect(['Username:','pm>'])
    
#    print("i=",i)
    
    if i == 0:
        child.sendline('admin')
        child.expect('Password:')
        child.sendline('pm8')
    elif i==1:
        print("Already logged in")
    
#    child.sendline("off 1")
    
#    child.expect("Outlet ")
    child.sendline('')
    child.expect('pm>')

#    print(child.before)



def main():
    global verbose

    pm8Host = 'localhost'
    pm8Port = 4001

    configFile = "/etc/mqtt/bridge.ini"

    try:
        opts, args = getopt.getopt(sys.argv[1:], "c:hvn:p:", ["config=","help","verbose","host","port"])

        for o,a in opts:
            if o in ["-h","--help"]:
                usage()
                sys.exit()
            elif o in ["-c","--config"]:
                configFile = a
            elif o in ["-v","--verbose"]:
                verbose=True
            elif o in ["-n","--host"]:
                pm8Host = a
            elif o in ["-p","--port"]:
                pm8Host = int(a)

    except getopt.GetoptError as err:
        print(err)
        usage()
        sys.exit(2)

    cfg = cp.ConfigParser()
    cfg.read( configFile )

    mqttBroker=cfg.get('local','name')
    mqttPort=int(cfg.get('local','port'))

    if verbose:
        print("MQTT Broker " + mqttBroker)
        print("MQTT Port   " + str(mqttPort))
    #   
    # Connect to mqtt broker.
    #   
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message

    client.connect(mqttBroker, mqttPort, 60) 

#    pm8Connect( 'proliant', 4001)
    pm8Connect( pm8Host, pm8Port)

    signal.signal(signal.SIGHUP, handler)

    client.loop_forever()

    print("HERE")

    sys.exit(0)

main()

