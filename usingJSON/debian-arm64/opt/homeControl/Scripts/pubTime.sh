#!/bin/bash

# set -x

export PATH="/home/andrewh/bin:/home/andrewh/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/snap/bin"

MQTT=$(cat /etc/mqtt/bridge.json | jq -r .local.name )
TOPIC=$(cat /etc/mqtt/bridge.json | jq -r .topics.time)

if [ $# -gt 0 ]; then
    TOPIC=$1
fi

# mosquitto_pub -t $TOPIC -m $(date "+%H:%M:%S") -h $MQTT
mosquitto_pub -t $TOPIC -m $(date "+%H:%M") -h $MQTT




