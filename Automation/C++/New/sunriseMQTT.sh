#!/bin/bash

# set -x

export PATH="/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin"

RISE=$(sunwait list 1 sunrise  angle 53.664385N 2.672008W)

TOPIC="/test/sunrise"
MQTT_HOST=192.168.0.65


mosquitto_pub -h $MQTT_HOST -t $TOPIC -m $RISE -r

