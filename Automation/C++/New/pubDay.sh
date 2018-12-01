#!/bin/bash

export PATH="/home/andrewh/bin:/home/andrewh/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/snap/bin"

TOPIC="/test/day"

if [ $# -gt 0 ]; then
    TOPIC=$1
fi

mosquitto_pub -t $TOPIC -m $(date "+%j") -h 192.168.0.65 -r




