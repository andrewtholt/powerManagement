#!/bin/bash

set -x

STATE=$1

MQTT_BROKER=$(cat /etc/mqtt/bridge.json | jq -r .local.name)

LONGTITUDE=$(cat /etc/mqtt/bridge.json | jq -r .location.long)
LATITUDE=$(cat /etc/mqtt/bridge.json | jq -r .location.lat)

export PATH="/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin"

sunwait wait set offset 1:00:00 ${LONGTITUDE}N ${LATITUDE}W ; mosquitto_pub -h $MQTT_BROKER -t "/home/outside/PorchLight/power" -m $STATE
