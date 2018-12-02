#!/bin/bash

# set -x

export PATH="/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin"

SUNSET=$(sunwait list 1 sunset  angle 53.664386N -2.672186W)


TOPIC="/test/sunset"
MQTT_HOST=192.168.0.65


mosquitto_pub -h $MQTT_HOST -t $TOPIC -m $SUNSET -r

