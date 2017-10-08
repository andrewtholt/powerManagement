#!/usr/bin/env python3

import redis
import paho.mqtt.client as mqtt
import configparser as cp
import time

def state(rdis,key):
    value=(rdis.get(key)).decode('utf-8')
    
    rc = False
    
    if value.upper() in ["ON","TRUE"]:
        rc=True
    else:
        rc = False
        
    return rc
    
    
    
def main():
    cfg = cp.ConfigParser()
    cfg.sections()
    cfg.read('/etc/mqtt/bridge.ini')
    
    redisHost = cfg.get('common','redis-host',fallback='localhost')
    redisPort = cfg.getint('common','redis-port', fallback=6379)
    
    print("Host : " + redisHost)
    print("Port : " + str(redisPort))
    
    r = redis.StrictRedis(host=redisHost, port=redisPort, db=0)
#    r.connect(redisHost,redisPort,60)
    
    mqttHost = cfg.get('local','name',fallback='localhost')
    mqttPort = cfg.getint('local','port', fallback=1883)
    
    mqttClient = mqtt.Client()
    mqttClient.connect(mqttHost, mqttPort, 60)
    

#    for key in r.scan_iter("/*"):
#        k= key.decode('utf-8')
#        value=r.get(key)
#        v= value.decode('utf-8')
        
#        print(k)
#        if state(r,key):
#            print("\tTrue")
#        else:
#            print("\tFalse")

    t="/home/outside/BackFloodlight/cmnd/power"
    if state(r, "/home/environment/day"):
        msg="OFF"
    else:
        msg = "ON"
        
    
    print(t)
    print(msg)
    
    mqttClient.publish(t,msg,qos=0,retain=True)
    
    time.sleep(0.1)

main()


# bytesvalue.decode('utf-8')
# client.publish(t,msg,qos=0,retain=True)
#    localClient.connect(localMqttHost, localMqttPort, 60)
